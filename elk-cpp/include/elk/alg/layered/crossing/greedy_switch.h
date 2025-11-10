// Eclipse Layout Kernel - C++ Port
// Greedy switch heuristic for crossing minimization
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../../core/types.h"
#include "../layered_layout.h"
#include <vector>

namespace elk {
namespace layered {
namespace crossing {

// ============================================================================
// Greedy Switch Configuration
// ============================================================================

enum class GreedySwitchType {
    ONE_SIDED,        // Only consider one layer at a time
    TWO_SIDED         // Consider both adjacent layers
};

struct GreedySwitchConfig {
    GreedySwitchType type = GreedySwitchType::TWO_SIDED;
    int maxIterations = 10;           // Maximum number of passes
    bool improveBackward = true;      // Also try backward passes
    double convergenceThreshold = 0.01; // Stop if improvement is less than this
};

// ============================================================================
// Greedy Switch Heuristic
// ============================================================================

class GreedySwitchHeuristic {
public:
    // Main entry point
    static int minimize(std::vector<Layer>& layers,
                       const GreedySwitchConfig& config);

private:
    // Forward and backward passes
    static int forwardPass(std::vector<Layer>& layers,
                          const GreedySwitchConfig& config);

    static int backwardPass(std::vector<Layer>& layers,
                           const GreedySwitchConfig& config);

    // Single layer processing
    static int processLayer(Layer& currentLayer, Layer* previousLayer,
                           Layer* nextLayer, const GreedySwitchConfig& config);

    // Greedy switching
    static int greedySwitchOneSided(Layer& layer, Layer& fixedLayer, bool forward);
    static int greedySwitchTwoSided(Layer& layer, Layer* leftLayer, Layer* rightLayer);

    // Try swapping adjacent nodes
    static int trySwap(Layer& layer, int i, int j,
                      Layer* leftLayer, Layer* rightLayer);

    // Calculate crossings
    static int countCrossings(const Layer& leftLayer, const Layer& rightLayer);
    static int countCrossingsBetweenNodes(LNode* left1, LNode* left2,
                                         LNode* right1, LNode* right2);

    // Node position utilities
    static double getBarycenter(LNode* node, bool useForwardNeighbors);
    static std::vector<LNode*> getConnectedNodes(LNode* node, bool forward);
};

// ============================================================================
// Crossing Counter
// ============================================================================

class CrossingCounter {
public:
    // Count crossings between two layers
    static int count(const Layer& leftLayer, const Layer& rightLayer);

    // Count crossings for specific edge
    static int countForEdge(const LEdge* edge, const Layer& leftLayer,
                           const Layer& rightLayer);

    // Count crossings between two edges
    static int countBetweenEdges(const LEdge* e1, const LEdge* e2);

private:
    // Check if two edges cross
    static bool edgesCross(const LEdge* e1, const LEdge* e2,
                          const Layer& leftLayer, const Layer& rightLayer);

    // Get node position in layer
    static int getNodePosition(const LNode* node, const Layer& layer);
};

// ============================================================================
// Switch Decision
// ============================================================================

struct SwitchCandidate {
    int index1;
    int index2;
    int crossingReduction;  // How many crossings would be saved
    double score;           // Overall score (may include other factors)
};

class SwitchDecider {
public:
    // Find best switch candidates
    static std::vector<SwitchCandidate> findCandidates(
        const Layer& layer, Layer* leftLayer, Layer* rightLayer);

    // Evaluate a potential switch
    static SwitchCandidate evaluateSwitch(
        const Layer& layer, int i, int j,
        Layer* leftLayer, Layer* rightLayer);

    // Select best candidate
    static SwitchCandidate selectBest(const std::vector<SwitchCandidate>& candidates);

private:
    // Calculate score for a switch
    static double calculateScore(int crossingReduction,
                                 double barycenterDiff,
                                 int portConstraintViolations);
};

// ============================================================================
// Utility Functions
// ============================================================================

// Get all edges between two layers
inline std::vector<LEdge*> getEdgesBetween(const Layer& leftLayer,
                                          const Layer& rightLayer) {
    std::vector<LEdge*> edges;

    for (LNode* node : leftLayer.nodes) {
        for (LPort* port : node->ports) {
            for (LEdge* edge : port->outgoingEdges) {
                if (edge->target && edge->target->node &&
                    std::find(rightLayer.nodes.begin(), rightLayer.nodes.end(),
                             edge->target->node) != rightLayer.nodes.end()) {
                    edges.push_back(edge);
                }
            }
        }
    }

    return edges;
}

// Check if nodes are adjacent in layer
inline bool areAdjacent(const Layer& layer, int i, int j) {
    return std::abs(i - j) == 1;
}

// Swap two nodes in layer
inline void swapNodes(Layer& layer, int i, int j) {
    if (i >= 0 && j >= 0 && i < layer.nodes.size() && j < layer.nodes.size()) {
        std::swap(layer.nodes[i], layer.nodes[j]);
    }
}

// Calculate barycenter value for node
inline double calculateBarycenter(LNode* node, bool useOutgoing) {
    std::vector<double> positions;

    if (useOutgoing) {
        for (LPort* port : node->ports) {
            for (LEdge* edge : port->outgoingEdges) {
                if (edge->target && edge->target->node) {
                    positions.push_back(edge->target->node->layer);
                }
            }
        }
    } else {
        for (LPort* port : node->ports) {
            for (LEdge* edge : port->incomingEdges) {
                if (edge->source && edge->source->node) {
                    positions.push_back(edge->source->node->layer);
                }
            }
        }
    }

    if (positions.empty()) return 0.0;

    double sum = 0.0;
    for (double pos : positions) {
        sum += pos;
    }

    return sum / positions.size();
}

// Count port constraint violations
inline int countPortConstraintViolations(const Layer& layer) {
    int violations = 0;

    for (LNode* node : layer.nodes) {
        // Check if ports are in correct order
        for (size_t i = 0; i < node->ports.size(); ++i) {
            for (size_t j = i + 1; j < node->ports.size(); ++j) {
                LPort* p1 = node->ports[i];
                LPort* p2 = node->ports[j];

                // If ports should be in different order, count as violation
                if (p1->side == p2->side) {
                    // Simple check: compare port indices
                    if (p1->index > p2->index) {
                        ++violations;
                    }
                }
            }
        }
    }

    return violations;
}

} // namespace crossing
} // namespace layered
} // namespace elk
