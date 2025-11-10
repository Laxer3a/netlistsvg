// Eclipse Layout Kernel - C++ Port
// Layered layout implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/alg/layered/layered_layout.h"
#include <algorithm>
#include <queue>
#include <limits>
#include <iostream>

namespace elk {
namespace layered {

LayeredLayoutProvider::LayeredLayoutProvider() {}

void LayeredLayoutProvider::layout(Node* graph, ProgressCallback progress) {
    if (!graph) return;

    std::vector<LNode*> nodes;
    std::vector<LEdge*> edges;
    std::vector<Layer> layers;

    // Phase 1: Import
    if (progress) progress("Importing graph", 0.0);
    importGraph(graph, nodes, edges);

    if (nodes.empty()) return;

    // Phase 2: Break cycles
    if (progress) progress("Breaking cycles", 0.15);
    breakCycles(nodes, edges);

    // Phase 3: Assign layers
    if (progress) progress("Assigning layers", 0.30);
    assignLayers(nodes, layers);

    // Phase 4: Insert dummy nodes
    if (progress) progress("Processing long edges", 0.45);
    insertDummyNodes(nodes, edges, layers);

    // Phase 5: Minimize crossings
    if (progress) progress("Minimizing crossings", 0.60);
    minimizeCrossings(layers);

    // Phase 6: Assign coordinates
    if (progress) progress("Placing nodes", 0.75);
    assignCoordinates(layers);

    // Phase 7: Route edges
    if (progress) progress("Routing edges", 0.90);
    routeEdges(layers, edges);

    // Apply back to original graph
    applyLayout(nodes, edges);

    // Calculate and set graph size
    calculateGraphSize(graph, nodes);

    // Cleanup
    cleanup(nodes, edges);

    if (progress) progress("Layout complete", 1.0);
}

void LayeredLayoutProvider::importGraph(Node* graph, std::vector<LNode*>& nodes, std::vector<LEdge*>& edges) {
    std::unordered_map<Node*, LNode*> nodeMap;
    std::unordered_map<Port*, LPort*> portMap;

    std::cerr << "\n=== IMPORT GRAPH ===\n";
    std::cerr << "Graph has " << graph->children.size() << " children, " << graph->edges.size() << " edges\n";

    // Create LNode for each child
    for (auto& child : graph->children) {
        LNode* lnode = new LNode();
        lnode->originalNode = child.get();
        lnode->size = child->size;
        lnode->position = child->position;
        nodes.push_back(lnode);
        nodeMap[child.get()] = lnode;

        std::cerr << "  Node " << child->id << " has " << child->ports.size() << " ports\n";

        // Create LPorts for each port
        for (auto& port : child->ports) {
            LPort* lport = new LPort();
            lport->originalPort = port.get();
            lport->side = port->side;
            lport->size = port->size;
            lport->position = port->position;
            lport->node = lnode;
            lnode->ports.push_back(lport);
            portMap[port.get()] = lport;
        }
    }

    // Create LEdge for each edge - edges connect ports, not nodes!
    int edgeCount = 0;
    for (auto& edge : graph->edges) {
        if (edge->sourcePorts.empty() || edge->targetPorts.empty()) {
            std::cerr << "  Edge skipped (no ports)\n";
            continue;
        }

        Port* srcPort = edge->sourcePorts[0];
        Port* tgtPort = edge->targetPorts[0];

        auto srcIt = portMap.find(srcPort);
        auto tgtIt = portMap.find(tgtPort);

        if (srcIt != portMap.end() && tgtIt != portMap.end()) {
            LEdge* ledge = new LEdge();
            ledge->setSource(srcIt->second);  // Use setter to maintain port edge lists
            ledge->setTarget(tgtIt->second);
            ledge->originalEdge = edge.get();
            edges.push_back(ledge);
            edgeCount++;
        } else {
            if (srcIt == portMap.end()) std::cerr << "  Edge skipped (source port not found)\n";
            if (tgtIt == portMap.end()) std::cerr << "  Edge skipped (target port not found)\n";
        }
    }
    std::cerr << "Created " << edgeCount << " edges\n";

    // Debug: Check edge connectivity for a few nodes
    for (auto& child : graph->children) {
        auto it = nodeMap.find(child.get());
        if (it != nodeMap.end()) {
            LNode* lnode = it->second;
            auto incoming = lnode->getIncomingEdges();
            auto outgoing = lnode->getOutgoingEdges();
            if ((incoming.size() > 0 || outgoing.size() > 0) && child->id.find("$sub") != std::string::npos) {
                std::cerr << "  " << child->id << ": in=" << incoming.size() << " out=" << outgoing.size() << "\n";
                for (LPort* port : lnode->ports) {
                    std::cerr << "    Port (side=" << (int)port->side << "): in=" << port->getIncomingEdges().size()
                              << " out=" << port->getOutgoingEdges().size() << "\n";
                }
            }
        }
    }
}

void LayeredLayoutProvider::breakCycles(std::vector<LNode*>& nodes, std::vector<LEdge*>& edges) {
    // Simple greedy cycle breaking: reverse edges that create cycles
    std::unordered_set<LNode*> visited;
    std::unordered_set<LNode*> recursionStack;

    std::function<bool(LNode*)> hasCycle = [&](LNode* node) -> bool {
        visited.insert(node);
        recursionStack.insert(node);

        for (LEdge* edge : node->getOutgoingEdges()) {
            if (!edge->reversed) {
                LNode* targetNode = edge->getTarget()->getNode();
                if (recursionStack.find(targetNode) != recursionStack.end()) {
                    // Cycle detected, reverse this edge
                    edge->reversed = true;
                    // Swap source and target ports
                    LPort* temp = edge->source;
                    edge->source = edge->target;
                    edge->target = temp;
                    return true;
                }
                if (visited.find(targetNode) == visited.end()) {
                    if (hasCycle(targetNode)) {
                        return true;
                    }
                }
            }
        }

        recursionStack.erase(node);
        return false;
    };

    for (LNode* node : nodes) {
        if (visited.find(node) == visited.end()) {
            hasCycle(node);
        }
    }
}

void LayeredLayoutProvider::assignLayers(std::vector<LNode*>& nodes, std::vector<Layer>& layers) {
    int maxLayer = assignLayersLongestPath(nodes);

    // Create layer structures
    layers.resize(maxLayer + 1);

    // Assign nodes to layers
    for (LNode* node : nodes) {
        if (node->layerIndex >= 0 && node->layerIndex <= maxLayer) {
            layers[node->layerIndex].nodes.push_back(node);
        }
    }

    // Debug: Print layer assignment
    std::cerr << "\n=== LAYER ASSIGNMENT ===\n";
    std::cerr << "Total layers: " << layers.size() << "\n";
    for (size_t i = 0; i < layers.size(); ++i) {
        std::cerr << "Layer " << i << ": " << layers[i].nodes.size() << " nodes\n";
    }
}

int LayeredLayoutProvider::assignLayersLongestPath(std::vector<LNode*>& nodes) {
    // Topological ordering
    std::vector<LNode*> sorted;
    std::unordered_set<LNode*> visited;

    for (LNode* node : nodes) {
        if (visited.find(node) == visited.end()) {
            topologicalSortUtil(node, visited, sorted);
        }
    }

    std::reverse(sorted.begin(), sorted.end());

    std::cerr << "\n=== LONGEST PATH LAYERING ===\n";
    std::cerr << "Topological order (" << sorted.size() << " nodes):\n";
    for (size_t i = 0; i < sorted.size() && i < 20; ++i) {
        auto* node = sorted[i];
        std::cerr << "  " << i << ": " << (node->originalNode ? node->originalNode->id : "external")
                  << " (in:" << node->getIncomingEdges().size()
                  << " out:" << node->getOutgoingEdges().size() << ")\n";
    }

    // Assign layers using longest path
    int maxLayer = 0;
    for (LNode* node : sorted) {
        int maxPredLayer = -1;
        for (LEdge* edge : node->getIncomingEdges()) {
            if (!edge->reversed) {
                LNode* sourceNode = edge->getSource()->getNode();
                if (sourceNode) {
                    maxPredLayer = std::max(maxPredLayer, sourceNode->layerIndex);
                }
            }
        }
        node->layerIndex = maxPredLayer + 1;
        maxLayer = std::max(maxLayer, node->layerIndex);

        if (node->originalNode) {
            std::cerr << "  " << node->originalNode->id << " -> layer " << node->layerIndex
                      << " (maxPred=" << maxPredLayer << ")\n";
        }
    }

    return maxLayer;
}

void LayeredLayoutProvider::insertDummyNodes(std::vector<LNode*>& nodes, std::vector<LEdge*>& edges,
                                             std::vector<Layer>& layers) {
    std::vector<LEdge*> longEdges;

    // Find edges spanning multiple layers
    for (LEdge* edge : edges) {
        LNode* srcNode = edge->getSource()->getNode();
        LNode* tgtNode = edge->getTarget()->getNode();
        if (srcNode && tgtNode) {
            int layerDiff = std::abs(tgtNode->layerIndex - srcNode->layerIndex);
            if (layerDiff > 1) {
                longEdges.push_back(edge);
            }
        }
    }

    // Insert dummy nodes for long edges
    for (LEdge* edge : longEdges) {
        LNode* srcNode = edge->getSource()->getNode();
        LNode* tgtNode = edge->getTarget()->getNode();

        int startLayer = std::min(srcNode->layerIndex, tgtNode->layerIndex);
        int endLayer = std::max(srcNode->layerIndex, tgtNode->layerIndex);

        LPort* prevPort = edge->getSource();
        for (int layer = startLayer + 1; layer < endLayer; ++layer) {
            // Create dummy node
            LNode* dummy = new LNode();
            dummy->type = NodeType::LONG_EDGE;
            dummy->layerIndex = layer;
            dummy->size = Size(1, 1);  // Minimal size
            nodes.push_back(dummy);
            layers[layer].nodes.push_back(dummy);

            // Create ports for dummy (in and out)
            LPort* dummyIn = new LPort();
            dummyIn->node = dummy;
            dummyIn->side = PortSide::WEST;
            dummy->ports.push_back(dummyIn);

            LPort* dummyOut = new LPort();
            dummyOut->node = dummy;
            dummyOut->side = PortSide::EAST;
            dummy->ports.push_back(dummyOut);

            // Create edge segment
            LEdge* segment = new LEdge();
            segment->setSource(prevPort);
            segment->setTarget(dummyIn);
            edges.push_back(segment);

            prevPort = dummyOut;
        }

        // Update original edge to connect from last dummy
        edge->setSource(prevPort);
    }
}

void LayeredLayoutProvider::minimizeCrossings(std::vector<Layer>& layers) {
    if (crossingStrategy_ == CrossingMinimizationStrategy::NONE) return;

    // Simple layer sweep with barycenter heuristic
    int iterations = 5;
    for (int iter = 0; iter < iterations; ++iter) {
        // Forward sweep
        for (size_t i = 1; i < layers.size(); ++i) {
            barycenterHeuristic(layers[i], true);  // Use incoming edges
        }
        // Backward sweep
        for (int i = static_cast<int>(layers.size()) - 2; i >= 0; --i) {
            barycenterHeuristic(layers[i], false);  // Use outgoing edges
        }
    }
}

void LayeredLayoutProvider::barycenterHeuristic(Layer& layer, bool useIncoming) {
    struct NodeWithBarycenter {
        LNode* node;
        double barycenter;
    };

    std::vector<NodeWithBarycenter> nodePositions;

    for (LNode* node : layer.nodes) {
        double sum = 0.0;
        int count = 0;

        const auto& edges = useIncoming ? node->getIncomingEdges() : node->getOutgoingEdges();
        for (LEdge* edge : edges) {
            LNode* other = useIncoming ? edge->getSource()->getNode() : edge->getTarget()->getNode();
            if (other && other->orderInLayer >= 0) {
                sum += other->orderInLayer;
                count++;
            }
        }

        double barycenter = (count > 0) ? sum / count : node->orderInLayer;
        nodePositions.push_back({node, barycenter});
    }

    // Sort by barycenter
    std::sort(nodePositions.begin(), nodePositions.end(),
              [](const auto& a, const auto& b) { return a.barycenter < b.barycenter; });

    // Update layer and order
    layer.nodes.clear();
    for (size_t i = 0; i < nodePositions.size(); ++i) {
        nodePositions[i].node->orderInLayer = static_cast<int>(i);
        layer.nodes.push_back(nodePositions[i].node);
    }
}

void LayeredLayoutProvider::assignCoordinates(std::vector<Layer>& layers) {
    linearSegmentPlacement(layers);
}

void LayeredLayoutProvider::linearSegmentPlacement(std::vector<Layer>& layers) {
    std::cerr << "\n=== LINEAR SEGMENT PLACEMENT ===\n";
    std::cerr << "Number of layers: " << layers.size() << "\n";
    std::cerr << "Direction: " << (int)direction_ << " (0=RIGHT, 1=LEFT, 2=DOWN, 3=UP)\n";
    std::cerr << "Node spacing: " << nodeSpacing_ << ", Layer spacing: " << layerSpacing_ << "\n";

    // Different logic based on direction
    if (direction_ == Direction::DOWN || direction_ == Direction::UP) {
        // Pass 1: Pre-calculate port extents for all layers
        for (size_t layerIdx = 0; layerIdx < layers.size(); ++layerIdx) {
            Layer& layer = layers[layerIdx];
            double maxPortExtentBelow = 0.0;
            double maxPortExtentAbove = 0.0;

            for (LNode* node : layer.nodes) {
                for (LPort* port : node->ports) {
                    double extentBelow = port->position.y - node->size.height;
                    double extentAbove = -port->position.y;
                    maxPortExtentBelow = std::max(maxPortExtentBelow, extentBelow);
                    maxPortExtentAbove = std::max(maxPortExtentAbove, extentAbove);
                }
            }

            layer.maxPortExtentBelow = maxPortExtentBelow;
            layer.maxPortExtentAbove = maxPortExtentAbove;
        }

        // Pass 2: Position layers accounting for port extents
        double currentY = 0.0;

        for (size_t layerIdx = 0; layerIdx < layers.size(); ++layerIdx) {
            Layer& layer = layers[layerIdx];
            double currentX = 0.0;
            double maxHeight = 0.0;

            std::cerr << "Layer " << layerIdx << " has " << layer.nodes.size() << " nodes, Y=" << currentY << "\n";

            for (LNode* node : layer.nodes) {
                node->position.x = currentX;
                node->position.y = currentY;

                std::cerr << "  Node " << (node->originalNode ? node->originalNode->id : "dummy")
                          << " at (" << currentX << ", " << currentY << ") size="
                          << node->size.width << "x" << node->size.height << "\n";

                currentX += node->size.width + nodeSpacing_;
                maxHeight = std::max(maxHeight, node->size.height);
            }

            // Calculate spacing to next layer
            double effectiveSpacing = layerSpacing_;
            effectiveSpacing += layer.maxPortExtentBelow;  // This layer's ports extending down
            if (layerIdx + 1 < layers.size()) {
                effectiveSpacing += layers[layerIdx + 1].maxPortExtentAbove;  // Next layer's ports extending up
            }

            std::cerr << "  Layer maxHeight=" << maxHeight
                      << ", portExtentBelow=" << layer.maxPortExtentBelow;
            if (layerIdx + 1 < layers.size()) {
                std::cerr << ", nextLayerPortExtentAbove=" << layers[layerIdx + 1].maxPortExtentAbove;
            }
            std::cerr << ", effectiveSpacing=" << effectiveSpacing
                      << ", next Y=" << (currentY + maxHeight + effectiveSpacing) << "\n";

            currentY += maxHeight + effectiveSpacing;
        }
    } else {
        // Layers go from left to right (or right to left)
        double currentX = 0.0;

        for (size_t layerIdx = 0; layerIdx < layers.size(); ++layerIdx) {
            Layer& layer = layers[layerIdx];
            double currentY = 0.0;
            double maxWidth = 0.0;

            std::cerr << "Layer " << layerIdx << " has " << layer.nodes.size() << " nodes, X=" << currentX << "\n";

            for (LNode* node : layer.nodes) {
                node->position.x = currentX;
                node->position.y = currentY;

                std::cerr << "  Node " << (node->originalNode ? node->originalNode->id : "dummy")
                          << " at (" << currentX << ", " << currentY << ") size="
                          << node->size.width << "x" << node->size.height << "\n";

                currentY += node->size.height + nodeSpacing_;
                maxWidth = std::max(maxWidth, node->size.width);
            }

            std::cerr << "  Layer maxWidth=" << maxWidth << ", next X=" << (currentX + maxWidth + layerSpacing_) << "\n";
            currentX += maxWidth + layerSpacing_;
        }
    }
}

void LayeredLayoutProvider::routeEdges(const std::vector<Layer>& layers, std::vector<LEdge*>& edges) {
    for (LEdge* edge : edges) {
        LPort* srcPort = edge->getSource();
        LPort* tgtPort = edge->getTarget();

        if (srcPort && tgtPort) {
            LNode* srcNode = srcPort->getNode();
            LNode* tgtNode = tgtPort->getNode();

            if (srcNode && tgtNode) {
                Point start = srcNode->position;
                start.x += srcNode->size.width;
                start.y += srcNode->size.height / 2;

                Point end = tgtNode->position;
                end.y += tgtNode->size.height / 2;

                edge->bendPoints.clear();
                // Simple straight routing for now
            }
        }
    }
}

void LayeredLayoutProvider::applyLayout(const std::vector<LNode*>& nodes, const std::vector<LEdge*>& edges) {
    std::cerr << "\n=== APPLY LAYOUT ===\n";
    std::cerr << "Total nodes: " << nodes.size() << "\n";

    // ELK uses 12px default padding/insets for graphs
    const double graphPadding = 12.0;

    // Apply node layout (transpiled from ElkGraphLayoutTransferrer.applyNodeLayout)
    int appliedCount = 0;
    int skippedCount = 0;
    for (const LNode* lnode : nodes) {
        if (lnode->originalNode && lnode->type == NodeType::NORMAL) {
            // Set node position with padding offset (Java lines 164-165)
            lnode->originalNode->position.x = lnode->position.x + graphPadding;
            lnode->originalNode->position.y = lnode->position.y + graphPadding;

            // Set port positions (Java lines 179-186)
            for (const LPort* lport : lnode->ports) {
                if (lport->originalPort) {
                    // Port position is relative to node position in ELK
                    lport->originalPort->position = lport->position;
                }
            }
            appliedCount++;
        } else {
            if (lnode->originalNode) {
                std::cerr << "  Skipped node " << lnode->originalNode->id
                          << " (type=" << (int)lnode->type << ")\n";
            }
            skippedCount++;
        }
    }
    std::cerr << "Applied layout to " << appliedCount << " nodes, skipped " << skippedCount << "\n";

    // Apply edge layout (transpiled from ElkGraphLayoutTransferrer.applyEdgeLayout)
    for (const LEdge* ledge : edges) {
        if (ledge->originalEdge) {
            LPort* srcPort = ledge->getSource();
            LPort* tgtPort = ledge->getTarget();

            if (srcPort && tgtPort) {
                // Get bendPoints from LEdge (Java line 238)
                std::vector<Point> bendPoints = ledge->bendPoints;

                // Add source port absolute anchor (Java lines 246-260)
                Point sourcePoint = srcPort->getAbsoluteAnchor();
                bendPoints.insert(bendPoints.begin(), sourcePoint);

                // Add target port absolute anchor (Java lines 263-267)
                Point targetPoint = tgtPort->getAbsoluteAnchor();
                bendPoints.push_back(targetPoint);

                // Create edge section with bendPoints (Java lines 273-276)
                EdgeSection section;
                section.startPoint = bendPoints.front();
                section.endPoint = bendPoints.back();

                // Copy intermediate bendPoints (exclude start and end)
                if (bendPoints.size() > 2) {
                    section.bendPoints.assign(bendPoints.begin() + 1, bendPoints.end() - 1);
                }

                ledge->originalEdge->sections = {section};
            }
        }
    }
}

void LayeredLayoutProvider::calculateGraphSize(Node* graph, const std::vector<LNode*>& nodes) {
    if (nodes.empty()) {
        graph->size = Size{40, 40};  // Default minimum size
        return;
    }

    // ELK uses 12px default padding/insets for graphs
    const double graphPadding = 12.0;

    // Calculate bounding box from all node positions and sizes
    double maxX = 0.0;
    double maxY = 0.0;

    for (const LNode* lnode : nodes) {
        if (lnode->type == NodeType::NORMAL && lnode->originalNode) {
            double nodeRight = lnode->position.x + lnode->size.width;
            double nodeBottom = lnode->position.y + lnode->size.height;
            maxX = std::max(maxX, nodeRight);
            maxY = std::max(maxY, nodeBottom);
        }
    }

    // Set graph size with padding on all sides (2x for both edges)
    graph->size.width = maxX + (2 * graphPadding);
    graph->size.height = maxY + (2 * graphPadding);
}

void LayeredLayoutProvider::cleanup(std::vector<LNode*>& nodes, std::vector<LEdge*>& edges) {
    // Delete ports first
    for (LNode* node : nodes) {
        for (LPort* port : node->ports) {
            delete port;
        }
    }

    // Delete nodes
    for (LNode* node : nodes) {
        delete node;
    }

    // Delete edges
    for (LEdge* edge : edges) {
        delete edge;
    }
}

} // namespace layered
} // namespace elk
