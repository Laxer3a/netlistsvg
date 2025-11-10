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
    std::vector<LNode*> sorted = assignLayersLongestPath(nodes);

    // Calculate max layer from nodes
    int maxLayer = 0;
    for (LNode* node : nodes) {
        maxLayer = std::max(maxLayer, node->layerIndex);
    }

    // Create layer structures
    layers.resize(maxLayer + 1);

    // Assign nodes to layers in topological order (to match Java behavior)
    for (LNode* node : sorted) {
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

std::vector<LNode*> LayeredLayoutProvider::assignLayersLongestPath(std::vector<LNode*>& nodes) {
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

    return sorted;  // Return sorted vector for layer assignment
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

    std::cerr << "\n=== MINIMIZE CROSSINGS ===\n";

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

    // Log final node order
    for (size_t i = 0; i < layers.size(); ++i) {
        std::cerr << "Layer " << i << " final order: ";
        for (LNode* node : layers[i].nodes) {
            std::cerr << (node->originalNode ? node->originalNode->id : "dummy") << " ";
        }
        std::cerr << "\n";
    }
}

void LayeredLayoutProvider::barycenterHeuristic(Layer& layer, bool useIncoming) {
    struct NodeWithBarycenter {
        LNode* node;
        double barycenter;
    };

    std::vector<NodeWithBarycenter> nodePositions;

    std::cerr << "  Barycenter (layer " << layer.index << ", " << (useIncoming ? "incoming" : "outgoing") << "):\n";

    for (LNode* node : layer.nodes) {
        double sum = 0.0;
        int count = 0;

        const auto& edges = useIncoming ? node->getIncomingEdges() : node->getOutgoingEdges();
        for (LEdge* edge : edges) {
            LNode* other = useIncoming ? edge->getSource()->getNode() : edge->getTarget()->getNode();
            if (other && other->orderInLayer >= 0) {
                sum += other->orderInLayer;
                count++;
                std::cerr << "      " << (node->originalNode ? node->originalNode->id : "dummy")
                          << " connects to " << (other->originalNode ? other->originalNode->id : "dummy")
                          << " at position " << other->orderInLayer << "\n";
            }
        }

        double barycenter = (count > 0) ? sum / count : node->orderInLayer;
        std::cerr << "    Node " << (node->originalNode ? node->originalNode->id : "dummy")
                  << ": sum=" << sum << ", count=" << count << ", barycenter=" << barycenter << "\n";
        nodePositions.push_back({node, barycenter});
    }

    // Sort by barycenter (use stable_sort to match Java Collections.sort behavior)
    std::stable_sort(nodePositions.begin(), nodePositions.end(),
                     [](const auto& a, const auto& b) { return a.barycenter < b.barycenter; });

    std::cerr << "    After sort: ";
    for (const auto& np : nodePositions) {
        std::cerr << (np.node->originalNode ? np.node->originalNode->id : "dummy") << "(" << np.barycenter << ") ";
    }
    std::cerr << "\n";

    // Update layer and order
    layer.nodes.clear();
    for (size_t i = 0; i < nodePositions.size(); ++i) {
        nodePositions[i].node->orderInLayer = static_cast<int>(i);
        layer.nodes.push_back(nodePositions[i].node);
    }
}

void LayeredLayoutProvider::assignCoordinates(std::vector<Layer>& layers) {
    std::cerr << "\n=== ASSIGN COORDINATES ===\n";

    // For DOWN direction, layers go top-to-bottom (along Y)
    // For RIGHT direction, layers go left-to-right (along X)
    // LinearSegmentsNodePlacer places nodes along Y within layers (for RIGHT)
    // For DOWN, we need to transform: swap layer axis and node axis

    if (direction_ == Direction::DOWN || direction_ == Direction::UP) {
        // For DOWN: layers along Y, nodes within layer along X
        // Need to account for port extents just like the old algorithm

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

            for (LNode* node : layer.nodes) {
                node->position.x = currentX;
                node->position.y = currentY;
                currentX += node->size.width + nodeSpacing_;
                maxHeight = std::max(maxHeight, node->size.height);
            }

            // Calculate spacing to next layer
            double effectiveSpacing = layerSpacing_;
            effectiveSpacing += layer.maxPortExtentBelow;  // This layer's ports extending down
            if (layerIdx + 1 < layers.size()) {
                effectiveSpacing += layers[layerIdx + 1].maxPortExtentAbove;  // Next layer's ports extending up
            }

            currentY += maxHeight + effectiveSpacing;
        }
    } else {
        // For RIGHT/LEFT: layers along X, nodes within layer along Y
        // Use LinearSegmentsNodePlacer for Y, then assign layer X

        // Port of LinearSegmentsNodePlacer.place() (Java line 206)
        // Phase 1: Sort linear segments with dependency graph
        std::vector<LinearSegment*> linearSegments = sortLinearSegments(layers);

        // Phase 2: Create unbalanced placement (assigns Y within layers)
        createUnbalancedPlacement(layers, linearSegments);

        // Phase 3: Balance placement (SKIPPED for now - can add later)
        // balancePlacement(layers, linearSegments);

        // Now assign X coordinates for layers
        double currentX = 0.0;
        for (Layer& layer : layers) {
            double maxWidth = 0.0;
            for (LNode* node : layer.nodes) {
                node->position.x = currentX;
                maxWidth = std::max(maxWidth, node->size.width);
            }
            currentX += maxWidth + layerSpacing_;
        }

        // Cleanup segments
        for (LinearSegment* seg : linearSegments) {
            delete seg;
        }
    }

    std::cerr << "Coordinate assignment complete\n";
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

// ============================================================================
// LinearSegmentsNodePlacer Algorithm (Faithful port from Java)
// Source: org.eclipse.elk.alg.layered.p4nodes.LinearSegmentsNodePlacer
// ============================================================================

std::vector<LinearSegment*> LayeredLayoutProvider::sortLinearSegments(std::vector<Layer>& layers) {
    std::cerr << "\n=== SORT LINEAR SEGMENTS ===\n";

    // Step 1: Set identifier and input/output priority for all nodes (Java lines 217-236)
    std::vector<LinearSegment*> segmentList;
    for (Layer& layer : layers) {
        for (LNode* node : layer.nodes) {
            node->segmentId = -1;
            int inprio = INT_MIN, outprio = INT_MIN;

            for (LPort* port : node->ports) {
                for (LEdge* edge : port->incomingEdges) {
                    // PRIORITY_STRAIGHTNESS default is 0 (LayeredOptions.java)
                    // In netlistsvg, edges don't have priority property, so use default
                    int prio = 0;  // edge->getProperty(PRIORITY_STRAIGHTNESS) would be 0
                    inprio = std::max(inprio, prio);
                }
                for (LEdge* edge : port->outgoingEdges) {
                    int prio = 0;  // Default PRIORITY_STRAIGHTNESS
                    outprio = std::max(outprio, prio);
                }
            }

            node->inputPriority = inprio;
            node->outputPriority = outprio;
        }
    }

    // Step 2: Create linear segments (Java lines 238-251)
    int nextLinearSegmentID = 0;
    for (Layer& layer : layers) {
        for (LNode* node : layer.nodes) {
            // Test for node ID; fillSegment calls may have set it
            if (node->segmentId < 0) {
                LinearSegment* segment = new LinearSegment();
                segment->id = nextLinearSegmentID++;
                fillSegment(node, segment);
                segmentList.push_back(segment);

                std::cerr << "  Segment " << segment->id << ": " << segment->nodes.size() << " nodes, type=" << (int)segment->nodeType << "\n";
            }
        }
    }

    std::cerr << "Created " << segmentList.size() << " linear segments\n";

    // Step 3: Create and initialize segment ordering graph (Java lines 253-259)
    std::vector<std::vector<LinearSegment*>> outgoingList(segmentList.size());
    std::vector<int> incomingCountList(segmentList.size(), 0);

    // Step 4: Create edges for segment ordering graph (Java line 262)
    createDependencyGraphEdges(layers, segmentList, outgoingList, incomingCountList, nextLinearSegmentID);

    // Step 5: Gather sources of segment ordering graph (Java lines 276-282)
    std::vector<LinearSegment*> noIncoming;
    for (size_t i = 0; i < segmentList.size(); ++i) {
        if (incomingCountList[i] == 0) {
            noIncoming.push_back(segmentList[i]);
        }
    }

    // Step 6: Topological sort (Java lines 284-298)
    int nextRank = 0;
    std::vector<int> newRanks(segmentList.size());

    while (!noIncoming.empty()) {
        LinearSegment* segment = noIncoming.front();
        noIncoming.erase(noIncoming.begin());
        newRanks[segment->id] = nextRank++;

        while (!outgoingList[segment->id].empty()) {
            LinearSegment* target = outgoingList[segment->id].front();
            outgoingList[segment->id].erase(outgoingList[segment->id].begin());
            incomingCountList[target->id]--;

            if (incomingCountList[target->id] == 0) {
                noIncoming.push_back(target);
            }
        }
    }

    // Step 7: Apply new ordering (Java lines 300-311)
    std::vector<LinearSegment*> sortedSegments(segmentList.size());
    for (size_t i = 0; i < segmentList.size(); ++i) {
        LinearSegment* ls = segmentList[i];
        int rank = newRanks[i];
        sortedSegments[rank] = ls;
        ls->id = rank;
        for (LNode* node : ls->nodes) {
            node->segmentId = rank;
        }
    }

    std::cerr << "Sorted " << sortedSegments.size() << " segments\n";
    return sortedSegments;
}

bool LayeredLayoutProvider::fillSegment(LNode* node, LinearSegment* segment) {
    // Faithful port from Java lines 461-505
    NodeType nodeType = node->type;

    if (node->segmentId >= 0) {
        // Node already part of another segment
        return false;
    } else {
        // Add node to segment
        node->segmentId = segment->id;
        segment->nodes.push_back(node);
    }
    segment->nodeType = nodeType;

    // For LONG_EDGE and NORTH_SOUTH_PORT dummies, try to extend segment (Java lines 474-502)
    if (nodeType == NodeType::LONG_EDGE || nodeType == NodeType::NORTH_SOUTH_PORT) {
        for (LPort* sourcePort : node->ports) {
            // Get successor ports (ports connected via outgoing edges)
            for (LEdge* edge : sourcePort->outgoingEdges) {
                LPort* targetPort = edge->target;
                if (!targetPort) continue;

                LNode* targetNode = targetPort->node;
                if (!targetNode) continue;

                NodeType targetNodeType = targetNode->type;

                // Only extend if nodes are in different layers (Java line 489)
                if (node->layerIndex != targetNode->layerIndex) {
                    // Check if target is also LONG_EDGE or NORTH_SOUTH_PORT (Java lines 491-492)
                    if (targetNodeType == NodeType::LONG_EDGE ||
                        targetNodeType == NodeType::NORTH_SOUTH_PORT) {
                        if (fillSegment(targetNode, segment)) {
                            // Added another node to segment
                            return true;
                        }
                    }
                }
            }
        }
    }

    return true;
}

void LayeredLayoutProvider::createDependencyGraphEdges(
        std::vector<Layer>& layers,
        std::vector<LinearSegment*>& segmentList,
        std::vector<std::vector<LinearSegment*>>& outgoingList,
        std::vector<int>& incomingCountList,
        int& nextLinearSegmentID) {

    // Faithful port from Java lines 332-449
    std::cerr << "Creating dependency graph edges\n";

    int layerIndex = 0;
    for (Layer& layer : layers) {
        std::vector<LNode*>& nodes = layer.nodes;
        if (nodes.empty()) {
            continue;  // Ignore empty layers
        }

        int indexInLayer = 0;
        LNode* previousNode = nullptr;

        for (size_t nodeIdx = 0; nodeIdx < nodes.size(); ++nodeIdx) {
            LNode* currentNode = nodes[nodeIdx];
            LinearSegment* currentSegment = segmentList[currentNode->segmentId];

            // Check for cycles (Java lines 382-419)
            if (currentSegment->indexInLastLayer >= 0) {
                LinearSegment* cycleSegment = nullptr;

                // Look ahead in current layer for segments that would create cycle
                for (size_t cycleIdx = nodeIdx + 1; cycleIdx < nodes.size(); ++cycleIdx) {
                    LNode* cycleNode = nodes[cycleIdx];
                    LinearSegment* testSegment = segmentList[cycleNode->segmentId];

                    if (testSegment->lastLayer == currentSegment->lastLayer &&
                        testSegment->indexInLastLayer < currentSegment->indexInLastLayer) {
                        cycleSegment = testSegment;
                        break;
                    }
                }

                // If cycle detected, split current segment (Java lines 400-418)
                if (cycleSegment != nullptr) {
                    // Update dependencies before split
                    if (previousNode != nullptr) {
                        incomingCountList[currentNode->segmentId]--;
                        auto& prevOut = outgoingList[previousNode->segmentId];
                        prevOut.erase(std::remove(prevOut.begin(), prevOut.end(), currentSegment),
                                     prevOut.end());
                    }

                    // Split segment at current node
                    LinearSegment* newSegment = currentSegment->split(currentNode, nextLinearSegmentID++);
                    segmentList.push_back(newSegment);
                    outgoingList.push_back(std::vector<LinearSegment*>());

                    if (previousNode != nullptr) {
                        outgoingList[previousNode->segmentId].push_back(newSegment);
                        incomingCountList.push_back(1);
                    } else {
                        incomingCountList.push_back(0);
                    }

                    currentSegment = newSegment;
                }
            }

            // Add dependency to next node (Java lines 421-429)
            if (nodeIdx + 1 < nodes.size()) {
                LNode* nextNode = nodes[nodeIdx + 1];
                LinearSegment* nextSegment = segmentList[nextNode->segmentId];

                outgoingList[currentNode->segmentId].push_back(nextSegment);
                incomingCountList[nextNode->segmentId]++;
            }

            // Update segment's layer information (Java lines 432-433)
            currentSegment->lastLayer = layerIndex;
            currentSegment->indexInLastLayer = indexInLayer++;

            previousNode = currentNode;
        }

        layerIndex++;
    }
}

void LayeredLayoutProvider::createUnbalancedPlacement(
        std::vector<Layer>& layers,
        const std::vector<LinearSegment*>& linearSegments) {

    // Faithful port from Java lines 516-559
    std::cerr << "\n=== CREATE UNBALANCED PLACEMENT ===\n";

    // Track how many nodes are placed in each layer (Java line 518)
    std::vector<int> nodeCount(layers.size(), 0);

    // Track most recent node type and node in each layer (Java lines 521-522)
    std::vector<NodeType> recentNodeType(layers.size(), NodeType::NORMAL);
    std::vector<LNode*> recentNode(layers.size(), nullptr);

    // Track layer size (accumulated Y in each layer) - Java uses layer.getSize().y
    std::vector<double> layerSize(layers.size(), 0.0);

    // Iterate through linear segments in sorted order (Java line 525)
    for (LinearSegment* segment : linearSegments) {
        // Determine uppermost placement for this segment (Java line 527)
        double uppermostPlace = 0.0;

        for (LNode* node : segment->nodes) {
            int layerIndex = node->layerIndex;
            if (layerIndex < 0 || layerIndex >= (int)layers.size()) continue;

            nodeCount[layerIndex]++;

            // Calculate spacing from previous node in this layer (Java lines 532-540)
            // NOTE: In full ELK, this would use spacings.getVerticalSpacing(recentNode, node)
            // which accounts for node types (NORMAL, LONG_EDGE, etc.) and individual properties
            // For now, using simplified spacing with nodeSpacing_ as default
            double spacing = nodeSpacing_;  // Default edge-edge spacing
            if (nodeCount[layerIndex] > 0 && recentNode[layerIndex] != nullptr) {
                // TODO: Full implementation would call spacings.getVerticalSpacing()
                // spacing = spacings.getVerticalSpacing(recentNode[layerIndex], node);
                spacing = nodeSpacing_;  // Simplified for now
            }

            // Get current layer size (Java line 542: node.getLayer().getSize().y + spacing)
            uppermostPlace = std::max(uppermostPlace, layerSize[layerIndex] + spacing);
        }

        // Apply uppermost placement to all nodes in segment (Java lines 546-557)
        for (LNode* node : segment->nodes) {
            int layerIndex = node->layerIndex;
            if (layerIndex < 0 || layerIndex >= (int)layers.size()) continue;

            // Set node position with margin (Java line 548)
            // In Java, margin.top is space reserved above node for ports/labels
            node->position.y = uppermostPlace + node->margin.top;

            // Update layer size (Java lines 551-553)
            // layer.getSize().y = uppermostPlace + margin.top + node.size + margin.bottom
            layerSize[layerIndex] = uppermostPlace + node->margin.top + node->size.height + node->margin.bottom;

            recentNodeType[layerIndex] = node->type;
            recentNode[layerIndex] = node;

            std::cerr << "  Segment " << segment->id << ", node at layer " << layerIndex
                      << ", Y=" << node->position.y << ", layerSize=" << layerSize[layerIndex] << "\n";
        }
    }

    std::cerr << "Unbalanced placement complete\n";
}

} // namespace layered
} // namespace elk
