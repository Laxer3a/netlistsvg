// Eclipse Layout Kernel - C++ Port
// Force-directed layout implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/alg/force/force_layout.h"
#include <cmath>
#include <algorithm>
#include <chrono>

namespace elk {
namespace force {

ForceLayoutProvider::ForceLayoutProvider() {
    // Initialize random generator with time-based seed
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng_.seed(static_cast<unsigned int>(seed));
}

void ForceLayoutProvider::layout(Node* graph, ProgressCallback progress) {
    if (!graph) return;

    std::vector<ForceNode> nodes;
    std::vector<ForceEdge> edges;

    // Import graph structure
    importGraph(graph, nodes, edges);

    if (nodes.empty()) return;

    // Initialize random positions if needed
    std::uniform_real_distribution<double> dist(-100.0, 100.0);
    for (auto& node : nodes) {
        if (node.position.x == 0 && node.position.y == 0) {
            node.position.x = dist(rng_);
            node.position.y = dist(rng_);
        }
    }

    // Main iteration loop
    double temp = temperature_;
    double cooling = temperature_ / iterations_;

    for (int iter = 0; iter < iterations_; ++iter) {
        // Calculate forces
        calculateForces(nodes, edges);

        // Update positions
        updatePositions(nodes, temp);

        // Cool down
        temp -= cooling;

        // Report progress
        if (progress) {
            progress("Force Layout", static_cast<double>(iter) / iterations_);
        }
    }

    // Center and normalize
    centerGraph(nodes);
    normalizePositions(nodes);

    // Apply results back to original graph
    applyLayout(nodes);

    if (progress) {
        progress("Force Layout", 1.0);
    }
}

void ForceLayoutProvider::importGraph(Node* graph, std::vector<ForceNode>& nodes, std::vector<ForceEdge>& edges) {
    // Create ForceNode for each child node
    for (auto& child : graph->children) {
        nodes.emplace_back(child.get());
    }

    // Create mapping from Node* to ForceNode index
    std::unordered_map<Node*, size_t> nodeMap;
    for (size_t i = 0; i < nodes.size(); ++i) {
        nodeMap[nodes[i].node] = i;
    }

    // Create ForceEdge for each edge
    for (auto& edge : graph->edges) {
        if (edge->sourceNodes.empty() || edge->targetNodes.empty()) continue;

        Node* src = edge->sourceNodes[0];
        Node* tgt = edge->targetNodes[0];

        auto srcIt = nodeMap.find(src);
        auto tgtIt = nodeMap.find(tgt);

        if (srcIt != nodeMap.end() && tgtIt != nodeMap.end()) {
            edges.emplace_back(&nodes[srcIt->second], &nodes[tgtIt->second]);
        }
    }
}

void ForceLayoutProvider::calculateForces(std::vector<ForceNode>& nodes, const std::vector<ForceEdge>& edges) {
    // Reset forces
    for (auto& node : nodes) {
        node.force = {0, 0};
    }

    // Calculate repulsive forces between all node pairs
    calculateRepulsiveForces(nodes);

    // Calculate attractive forces along edges
    calculateAttractiveForces(edges);
}

void ForceLayoutProvider::calculateRepulsiveForces(std::vector<ForceNode>& nodes) {
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (size_t j = i + 1; j < nodes.size(); ++j) {
            Point force;

            if (model_ == ForceModel::EADES) {
                force = calculateEadesRepulsion(nodes[i].position, nodes[j].position, repulsion_);
            } else {
                force = calculateFRRepulsion(nodes[i].position, nodes[j].position, springLength_);
            }

            nodes[i].force += force;
            nodes[j].force -= force;
        }
    }
}

void ForceLayoutProvider::calculateAttractiveForces(const std::vector<ForceEdge>& edges) {
    for (const auto& edge : edges) {
        Point force;

        if (model_ == ForceModel::EADES) {
            force = calculateEadesAttraction(edge.source->position, edge.target->position,
                                            1.0, edge.desiredLength);
        } else {
            force = calculateFRAttraction(edge.source->position, edge.target->position, springLength_);
        }

        edge.source->force += force;
        edge.target->force -= force;
    }
}

void ForceLayoutProvider::updatePositions(std::vector<ForceNode>& nodes, double temperature) {
    for (auto& node : nodes) {
        // Limit displacement by temperature
        double forceLen = node.force.length();
        if (forceLen > EPSILON) {
            Point displacement = node.force.normalized() * std::min(forceLen, temperature);
            node.position += displacement;
        }
    }
}

void ForceLayoutProvider::centerGraph(std::vector<ForceNode>& nodes) {
    if (nodes.empty()) return;

    // Calculate center of mass
    Point center(0, 0);
    for (const auto& node : nodes) {
        center += node.position;
    }
    center = center / static_cast<double>(nodes.size());

    // Shift all nodes to center around origin
    for (auto& node : nodes) {
        node.position -= center;
    }
}

void ForceLayoutProvider::normalizePositions(std::vector<ForceNode>& nodes) {
    if (nodes.empty()) return;

    // Find bounds
    double minX = nodes[0].position.x;
    double minY = nodes[0].position.y;
    double maxX = nodes[0].position.x;
    double maxY = nodes[0].position.y;

    for (const auto& node : nodes) {
        minX = std::min(minX, node.position.x);
        minY = std::min(minY, node.position.y);
        maxX = std::max(maxX, node.position.x);
        maxY = std::max(maxY, node.position.y);
    }

    // Shift to positive coordinates with margin
    double margin = 20.0;
    for (auto& node : nodes) {
        node.position.x = node.position.x - minX + margin;
        node.position.y = node.position.y - minY + margin;
    }
}

void ForceLayoutProvider::applyLayout(const std::vector<ForceNode>& nodes) {
    for (const auto& fnode : nodes) {
        fnode.node->position = fnode.position;
    }
}

} // namespace force
} // namespace elk
