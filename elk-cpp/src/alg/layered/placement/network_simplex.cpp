// Eclipse Layout Kernel - C++ Port
// Network Simplex implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/alg/layered/placement/network_simplex.h"
#include <algorithm>
#include <cmath>
#include <queue>

namespace elk {
namespace layered {
namespace placement {

void NetworkSimplexPlacer::place(std::vector<Layer>& layers,
                                const NetworkSimplexConfig& config) {
    if (layers.empty()) return;

    // Phase 1: Build constraint graph
    std::vector<SimplexEdge> edges;
    auto nodes = buildGraph(layers, edges, config);

    if (nodes.empty()) return;

    // Phase 2: Create initial feasible tree
    createFeasibleTree(nodes, edges);

    // Phase 3: Run network simplex
    runSimplex(nodes, edges, config);

    // Phase 4: Extract and apply positions
    extractPositions(nodes, layers);
}

std::vector<SimplexNode> NetworkSimplexPlacer::buildGraph(
    const std::vector<Layer>& layers,
    std::vector<SimplexEdge>& edges,
    const NetworkSimplexConfig& config) {

    std::vector<SimplexNode> nodes;
    ConstraintGraphBuilder::build(layers, nodes, edges, config);
    return nodes;
}

void NetworkSimplexPlacer::createFeasibleTree(
    std::vector<SimplexNode>& nodes,
    std::vector<SimplexEdge>& edges) {

    SpanningTreeBuilder::build(nodes, edges);
}

void NetworkSimplexPlacer::runSimplex(
    std::vector<SimplexNode>& nodes,
    std::vector<SimplexEdge>& edges,
    const NetworkSimplexConfig& config) {

    for (int iter = 0; iter < config.maxIterations; ++iter) {
        // Calculate cut values for all tree edges
        calculateCutValues(nodes, edges);

        // Find entering edge (negative cut value)
        SimplexEdge* enteringEdge = findEnteringEdge(edges);

        if (!enteringEdge) {
            // Optimal solution found
            break;
        }

        // Find leaving edge
        SimplexEdge* leavingEdge = findLeavingEdge(nodes, edges, enteringEdge);

        if (!leavingEdge) {
            // Unbounded problem
            break;
        }

        // Update tree
        updateTree(nodes, edges, enteringEdge, leavingEdge);

        // Recalculate positions
        calculatePositions(nodes, edges);
    }

    // Normalize positions to start at 0
    normalizePositions(nodes);
}

void NetworkSimplexPlacer::extractPositions(
    const std::vector<SimplexNode>& nodes,
    std::vector<Layer>& layers) {

    for (const auto& node : nodes) {
        if (node.node) {
            node.node->position.x = node.position;
        }
    }
}

SimplexEdge* NetworkSimplexPlacer::findEnteringEdge(
    std::vector<SimplexEdge>& edges) {

    SimplexEdge* bestEdge = nullptr;
    int minCutValue = 0;

    for (auto& edge : edges) {
        if (!edge.inTree && edge.cutValue < minCutValue) {
            minCutValue = edge.cutValue;
            bestEdge = &edge;
        }
    }

    return bestEdge;
}

SimplexEdge* NetworkSimplexPlacer::findLeavingEdge(
    std::vector<SimplexNode>& nodes,
    std::vector<SimplexEdge>& edges,
    SimplexEdge* enteringEdge) {

    if (!enteringEdge) return nullptr;

    // Find edge in cycle with minimum slack
    SimplexEdge* leavingEdge = nullptr;
    double minSlack = std::numeric_limits<double>::max();

    for (auto& edge : edges) {
        if (edge.inTree) {
            double slack = calculateSlack(edge, nodes);
            if (slack < minSlack) {
                minSlack = slack;
                leavingEdge = &edge;
            }
        }
    }

    return leavingEdge;
}

void NetworkSimplexPlacer::updateTree(
    std::vector<SimplexNode>& nodes,
    std::vector<SimplexEdge>& edges,
    SimplexEdge* enteringEdge,
    SimplexEdge* leavingEdge) {

    if (!enteringEdge || !leavingEdge) return;

    // Add entering edge to tree
    enteringEdge->inTree = true;

    // Remove leaving edge from tree
    leavingEdge->inTree = false;

    // Rebuild tree structure
    SpanningTreeBuilder::build(nodes, edges);
}

void NetworkSimplexPlacer::calculateCutValues(
    std::vector<SimplexNode>& nodes,
    std::vector<SimplexEdge>& edges) {

    for (auto& edge : edges) {
        if (edge.inTree) {
            edge.cutValue = calculateCutValue(&edge, nodes, edges);
        }
    }
}

int NetworkSimplexPlacer::calculateCutValue(
    SimplexEdge* edge,
    const std::vector<SimplexNode>& nodes,
    const std::vector<SimplexEdge>& edges) {

    if (!edge) return 0;

    // Simplified cut value calculation
    // In a full implementation, this would use DFS to find the cut
    return static_cast<int>(edge->weight);
}

void NetworkSimplexPlacer::calculatePositions(
    std::vector<SimplexNode>& nodes,
    const std::vector<SimplexEdge>& edges) {

    // Initialize all positions to 0
    for (auto& node : nodes) {
        node.position = 0.0;
    }

    // Use BFS to propagate positions from root
    if (nodes.empty()) return;

    std::queue<SimplexNode*> queue;
    nodes[0].position = 0.0;
    queue.push(&nodes[0]);

    std::vector<bool> visited(nodes.size(), false);
    visited[0] = true;

    while (!queue.empty()) {
        SimplexNode* current = queue.front();
        queue.pop();

        // Process all tree edges from this node
        for (const auto& edge : edges) {
            if (!edge.inTree) continue;

            if (edge.source == current->id && !visited[edge.target]) {
                nodes[edge.target].position = current->position + edge.minLength;
                queue.push(&nodes[edge.target]);
                visited[edge.target] = true;
            } else if (edge.target == current->id && !visited[edge.source]) {
                nodes[edge.source].position = current->position - edge.minLength;
                queue.push(&nodes[edge.source]);
                visited[edge.source] = true;
            }
        }
    }
}

void NetworkSimplexPlacer::normalizePositions(std::vector<SimplexNode>& nodes) {
    if (nodes.empty()) return;

    double minPos = nodes[0].position;
    for (const auto& node : nodes) {
        minPos = std::min(minPos, node.position);
    }

    for (auto& node : nodes) {
        node.position -= minPos;
    }
}

SimplexNode* NetworkSimplexPlacer::findNode(std::vector<SimplexNode>& nodes, int id) {
    for (auto& node : nodes) {
        if (node.id == id) {
            return &node;
        }
    }
    return nullptr;
}

bool NetworkSimplexPlacer::formsLoop(SimplexNode* source, SimplexNode* target) {
    if (!source || !target) return false;

    // Check if adding edge from source to target would form a loop
    SimplexNode* current = target;
    while (current) {
        if (current == source) {
            return true;
        }
        current = current->parent;
    }

    return false;
}

// ConstraintGraphBuilder implementation

void ConstraintGraphBuilder::build(const std::vector<Layer>& layers,
                                   std::vector<SimplexNode>& nodes,
                                   std::vector<SimplexEdge>& edges,
                                   const NetworkSimplexConfig& config) {
    // Create nodes
    int nodeId = 0;
    for (const auto& layer : layers) {
        for (LNode* lnode : layer.nodes) {
            SimplexNode snode;
            snode.node = lnode;
            snode.id = nodeId++;
            nodes.push_back(snode);
        }
    }

    // Add ordering constraints within layers
    for (const auto& layer : layers) {
        addOrderingConstraints(layer, nodes, edges, config);
    }

    // Add edge straightness constraints
    addEdgeConstraints(layers, nodes, edges, config);

    // Add balance constraints if requested
    if (config.balanceNodes) {
        addBalanceConstraints(nodes, edges, config);
    }
}

void ConstraintGraphBuilder::addOrderingConstraints(
    const Layer& layer,
    std::vector<SimplexNode>& nodes,
    std::vector<SimplexEdge>& edges,
    const NetworkSimplexConfig& config) {

    // Add constraints for adjacent nodes in layer
    for (size_t i = 0; i < layer.nodes.size() - 1; ++i) {
        LNode* node1 = layer.nodes[i];
        LNode* node2 = layer.nodes[i + 1];

        // Find corresponding simplex nodes
        int id1 = -1, id2 = -1;
        for (size_t j = 0; j < nodes.size(); ++j) {
            if (nodes[j].node == node1) id1 = j;
            if (nodes[j].node == node2) id2 = j;
        }

        if (id1 >= 0 && id2 >= 0) {
            SimplexEdge edge;
            edge.source = id1;
            edge.target = id2;
            edge.minLength = config.nodeSpacing;
            edge.weight = 1.0;
            edges.push_back(edge);
        }
    }
}

void ConstraintGraphBuilder::addEdgeConstraints(
    const std::vector<Layer>& layers,
    std::vector<SimplexNode>& nodes,
    std::vector<SimplexEdge>& edges,
    const NetworkSimplexConfig& config) {

    if (!config.straightenEdges) return;

    // Add constraints to straighten edges between layers
    for (size_t i = 0; i < layers.size() - 1; ++i) {
        const Layer& layer1 = layers[i];
        const Layer& layer2 = layers[i + 1];

        for (LNode* node1 : layer1.nodes) {
            for (LPort* port : node1->ports) {
                for (LEdge* ledge : port->outgoingEdges) {
                    LNode* node2 = ledge->target ? ledge->target->node : nullptr;

                    if (node2) {
                        // Find simplex nodes
                        int id1 = -1, id2 = -1;
                        for (size_t j = 0; j < nodes.size(); ++j) {
                            if (nodes[j].node == node1) id1 = j;
                            if (nodes[j].node == node2) id2 = j;
                        }

                        if (id1 >= 0 && id2 >= 0) {
                            SimplexEdge edge;
                            edge.source = id1;
                            edge.target = id2;
                            edge.minLength = 0.0;  // Allow same position
                            edge.weight = config.straightEdgeWeight;
                            edge.originalEdge = ledge;
                            edges.push_back(edge);
                        }
                    }
                }
            }
        }
    }
}

void ConstraintGraphBuilder::addBalanceConstraints(
    std::vector<SimplexNode>& nodes,
    std::vector<SimplexEdge>& edges,
    const NetworkSimplexConfig& config) {

    // Balance constraints help distribute nodes evenly
    // This is a simplified version
}

// SpanningTreeBuilder implementation

void SpanningTreeBuilder::build(std::vector<SimplexNode>& nodes,
                               std::vector<SimplexEdge>& edges) {

    // Create tight tree
    createTightTree(nodes, edges);

    // Make feasible
    makeFeasible(nodes, edges);

    // Build tree structure
    buildTreeStructure(nodes, edges);
}

void SpanningTreeBuilder::createTightTree(std::vector<SimplexNode>& nodes,
                                         std::vector<SimplexEdge>& edges) {

    // Mark all edges as not in tree
    for (auto& edge : edges) {
        edge.inTree = false;
    }

    // Use simple greedy approach: add edges with highest weight
    std::sort(edges.begin(), edges.end(),
        [](const SimplexEdge& a, const SimplexEdge& b) {
            return a.weight > b.weight;
        });

    int treeEdges = 0;
    int targetEdges = nodes.size() - 1;

    for (auto& edge : edges) {
        if (treeEdges >= targetEdges) break;

        // Check if adding this edge would create a loop
        // (simplified check)
        edge.inTree = true;
        ++treeEdges;
    }
}

void SpanningTreeBuilder::makeFeasible(std::vector<SimplexNode>& nodes,
                                      std::vector<SimplexEdge>& edges) {
    // Ensure tree is feasible (satisfies all constraints)
    // Simplified implementation
}

void SpanningTreeBuilder::buildTreeStructure(std::vector<SimplexNode>& nodes,
                                            const std::vector<SimplexEdge>& edges) {

    // Clear existing tree structure
    for (auto& node : nodes) {
        node.parent = nullptr;
        node.children.clear();
        node.depth = 0;
    }

    if (nodes.empty()) return;

    // Use BFS to build tree from first node
    std::queue<SimplexNode*> queue;
    nodes[0].depth = 0;
    queue.push(&nodes[0]);

    std::vector<bool> visited(nodes.size(), false);
    visited[0] = true;

    while (!queue.empty()) {
        SimplexNode* current = queue.front();
        queue.pop();

        // Find tree edges connected to this node
        for (const auto& edge : edges) {
            if (!edge.inTree) continue;

            if (edge.source == current->id && !visited[edge.target]) {
                SimplexNode* child = &nodes[edge.target];
                child->parent = current;
                child->depth = current->depth + 1;
                current->children.push_back(child);
                visited[edge.target] = true;
                queue.push(child);
            } else if (edge.target == current->id && !visited[edge.source]) {
                SimplexNode* child = &nodes[edge.source];
                child->parent = current;
                child->depth = current->depth + 1;
                current->children.push_back(child);
                visited[edge.source] = true;
                queue.push(child);
            }
        }
    }
}

} // namespace placement
} // namespace layered
} // namespace elk
