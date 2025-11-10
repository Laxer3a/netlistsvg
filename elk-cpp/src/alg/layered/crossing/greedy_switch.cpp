// Eclipse Layout Kernel - C++ Port
// Greedy switch heuristic implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/alg/layered/crossing/greedy_switch.h"
#include <algorithm>
#include <limits>
#include <cmath>

namespace elk {
namespace layered {
namespace crossing {

int GreedySwitchHeuristic::minimize(std::vector<Layer>& layers,
                                   const GreedySwitchConfig& config) {
    if (layers.size() < 2) return 0;

    int totalReduction = 0;
    int iteration = 0;
    int lastCrossings = std::numeric_limits<int>::max();

    while (iteration < config.maxIterations) {
        int reduction = forwardPass(layers, config);

        if (config.improveBackward) {
            reduction += backwardPass(layers, config);
        }

        totalReduction += reduction;

        // Check convergence
        int currentCrossings = 0;
        for (size_t i = 0; i < layers.size() - 1; ++i) {
            currentCrossings += countCrossings(layers[i], layers[i + 1]);
        }

        double improvement = static_cast<double>(lastCrossings - currentCrossings) /
                           lastCrossings;

        if (improvement < config.convergenceThreshold) {
            break;
        }

        lastCrossings = currentCrossings;
        ++iteration;
    }

    return totalReduction;
}

int GreedySwitchHeuristic::forwardPass(std::vector<Layer>& layers,
                                       const GreedySwitchConfig& config) {
    int totalReduction = 0;

    for (size_t i = 0; i < layers.size(); ++i) {
        Layer* prevLayer = i > 0 ? &layers[i - 1] : nullptr;
        Layer* nextLayer = i < layers.size() - 1 ? &layers[i + 1] : nullptr;

        int reduction = processLayer(layers[i], prevLayer, nextLayer, config);
        totalReduction += reduction;
    }

    return totalReduction;
}

int GreedySwitchHeuristic::backwardPass(std::vector<Layer>& layers,
                                        const GreedySwitchConfig& config) {
    int totalReduction = 0;

    for (int i = layers.size() - 1; i >= 0; --i) {
        Layer* prevLayer = i > 0 ? &layers[i - 1] : nullptr;
        Layer* nextLayer = i < layers.size() - 1 ? &layers[i + 1] : nullptr;

        int reduction = processLayer(layers[i], prevLayer, nextLayer, config);
        totalReduction += reduction;
    }

    return totalReduction;
}

int GreedySwitchHeuristic::processLayer(Layer& currentLayer,
                                        Layer* previousLayer,
                                        Layer* nextLayer,
                                        const GreedySwitchConfig& config) {
    int totalReduction = 0;

    if (config.type == GreedySwitchType::ONE_SIDED) {
        // Use one-sided approach
        if (previousLayer) {
            totalReduction += greedySwitchOneSided(currentLayer, *previousLayer, false);
        }
        if (nextLayer) {
            totalReduction += greedySwitchOneSided(currentLayer, *nextLayer, true);
        }
    } else {
        // Use two-sided approach
        totalReduction += greedySwitchTwoSided(currentLayer, previousLayer, nextLayer);
    }

    return totalReduction;
}

int GreedySwitchHeuristic::greedySwitchOneSided(Layer& layer, Layer& fixedLayer,
                                                bool forward) {
    int totalReduction = 0;
    bool improved = true;

    while (improved) {
        improved = false;

        for (size_t i = 0; i < layer.nodes.size() - 1; ++i) {
            int reduction = trySwap(layer, i, i + 1,
                                   forward ? nullptr : &fixedLayer,
                                   forward ? &fixedLayer : nullptr);

            if (reduction > 0) {
                swapNodes(layer, i, i + 1);
                totalReduction += reduction;
                improved = true;
            }
        }
    }

    return totalReduction;
}

int GreedySwitchHeuristic::greedySwitchTwoSided(Layer& layer,
                                                Layer* leftLayer,
                                                Layer* rightLayer) {
    int totalReduction = 0;
    bool improved = true;

    while (improved) {
        improved = false;

        // Find all switch candidates
        auto candidates = SwitchDecider::findCandidates(layer, leftLayer, rightLayer);

        if (!candidates.empty()) {
            // Select best candidate
            SwitchCandidate best = SwitchDecider::selectBest(candidates);

            if (best.crossingReduction > 0) {
                swapNodes(layer, best.index1, best.index2);
                totalReduction += best.crossingReduction;
                improved = true;
            }
        }
    }

    return totalReduction;
}

int GreedySwitchHeuristic::trySwap(Layer& layer, int i, int j,
                                   Layer* leftLayer, Layer* rightLayer) {
    // Calculate current crossings
    int currentCrossings = 0;

    if (leftLayer) {
        currentCrossings += countCrossings(*leftLayer, layer);
    }
    if (rightLayer) {
        currentCrossings += countCrossings(layer, *rightLayer);
    }

    // Try swap
    swapNodes(layer, i, j);

    // Calculate new crossings
    int newCrossings = 0;

    if (leftLayer) {
        newCrossings += countCrossings(*leftLayer, layer);
    }
    if (rightLayer) {
        newCrossings += countCrossings(layer, *rightLayer);
    }

    // Undo swap
    swapNodes(layer, i, j);

    return currentCrossings - newCrossings;
}

int GreedySwitchHeuristic::countCrossings(const Layer& leftLayer,
                                         const Layer& rightLayer) {
    return CrossingCounter::count(leftLayer, rightLayer);
}

int GreedySwitchHeuristic::countCrossingsBetweenNodes(LNode* left1, LNode* left2,
                                                      LNode* right1, LNode* right2) {
    int crossings = 0;

    // Get all edges from left nodes to right nodes
    std::vector<std::pair<int, int>> connections1;  // (left port, right port)
    std::vector<std::pair<int, int>> connections2;

    // Collect edges from left1 to right nodes
    for (LPort* port : left1->ports) {
        for (LEdge* edge : port->outgoingEdges) {
            if (edge->target && edge->target->node == right1) {
                connections1.push_back({port->index, edge->target->index});
            } else if (edge->target && edge->target->node == right2) {
                connections1.push_back({port->index, edge->target->index});
            }
        }
    }

    // Collect edges from left2 to right nodes
    for (LPort* port : left2->ports) {
        for (LEdge* edge : port->outgoingEdges) {
            if (edge->target && edge->target->node == right1) {
                connections2.push_back({port->index, edge->target->index});
            } else if (edge->target && edge->target->node == right2) {
                connections2.push_back({port->index, edge->target->index});
            }
        }
    }

    // Count crossings between connection pairs
    for (const auto& c1 : connections1) {
        for (const auto& c2 : connections2) {
            // Edges cross if one goes "over" the other
            if ((c1.second < c2.second && c1.first > c2.first) ||
                (c1.second > c2.second && c1.first < c2.first)) {
                ++crossings;
            }
        }
    }

    return crossings;
}

double GreedySwitchHeuristic::getBarycenter(LNode* node, bool useForwardNeighbors) {
    return calculateBarycenter(node, useForwardNeighbors);
}

std::vector<LNode*> GreedySwitchHeuristic::getConnectedNodes(LNode* node, bool forward) {
    std::vector<LNode*> connected;

    for (LPort* port : node->ports) {
        if (forward) {
            for (LEdge* edge : port->outgoingEdges) {
                if (edge->target && edge->target->node) {
                    connected.push_back(edge->target->node);
                }
            }
        } else {
            for (LEdge* edge : port->incomingEdges) {
                if (edge->source && edge->source->node) {
                    connected.push_back(edge->source->node);
                }
            }
        }
    }

    return connected;
}

// CrossingCounter implementation

int CrossingCounter::count(const Layer& leftLayer, const Layer& rightLayer) {
    int crossings = 0;

    auto edges = getEdgesBetween(leftLayer, rightLayer);

    // Count crossings between all pairs of edges
    for (size_t i = 0; i < edges.size(); ++i) {
        for (size_t j = i + 1; j < edges.size(); ++j) {
            if (edgesCross(edges[i], edges[j], leftLayer, rightLayer)) {
                ++crossings;
            }
        }
    }

    return crossings;
}

int CrossingCounter::countForEdge(const LEdge* edge, const Layer& leftLayer,
                                 const Layer& rightLayer) {
    int crossings = 0;

    auto edges = getEdgesBetween(leftLayer, rightLayer);

    for (LEdge* other : edges) {
        if (other != edge && edgesCross(edge, other, leftLayer, rightLayer)) {
            ++crossings;
        }
    }

    return crossings;
}

int CrossingCounter::countBetweenEdges(const LEdge* e1, const LEdge* e2) {
    // Simple check: edges cross if their endpoints are in opposite order
    if (!e1 || !e2 || !e1->source || !e2->source ||
        !e1->target || !e2->target) {
        return 0;
    }

    LNode* s1 = e1->source->node;
    LNode* t1 = e1->target->node;
    LNode* s2 = e2->source->node;
    LNode* t2 = e2->target->node;

    if (!s1 || !t1 || !s2 || !t2) return 0;

    // Check if edges cross
    int sourceOrder = s1->layer - s2->layer;
    int targetOrder = t1->layer - t2->layer;

    return (sourceOrder * targetOrder < 0) ? 1 : 0;
}

bool CrossingCounter::edgesCross(const LEdge* e1, const LEdge* e2,
                                const Layer& leftLayer, const Layer& rightLayer) {
    if (!e1 || !e2) return false;

    LNode* s1 = e1->source ? e1->source->node : nullptr;
    LNode* t1 = e1->target ? e1->target->node : nullptr;
    LNode* s2 = e2->source ? e2->source->node : nullptr;
    LNode* t2 = e2->target ? e2->target->node : nullptr;

    if (!s1 || !t1 || !s2 || !t2) return false;

    int s1Pos = getNodePosition(s1, leftLayer);
    int t1Pos = getNodePosition(t1, rightLayer);
    int s2Pos = getNodePosition(s2, leftLayer);
    int t2Pos = getNodePosition(t2, rightLayer);

    // Edges cross if they have opposite order
    return ((s1Pos < s2Pos && t1Pos > t2Pos) ||
            (s1Pos > s2Pos && t1Pos < t2Pos));
}

int CrossingCounter::getNodePosition(const LNode* node, const Layer& layer) {
    for (size_t i = 0; i < layer.nodes.size(); ++i) {
        if (layer.nodes[i] == node) {
            return i;
        }
    }
    return -1;
}

// SwitchDecider implementation

std::vector<SwitchCandidate> SwitchDecider::findCandidates(
    const Layer& layer, Layer* leftLayer, Layer* rightLayer) {

    std::vector<SwitchCandidate> candidates;

    for (size_t i = 0; i < layer.nodes.size() - 1; ++i) {
        SwitchCandidate candidate = evaluateSwitch(layer, i, i + 1,
                                                   leftLayer, rightLayer);
        if (candidate.crossingReduction > 0) {
            candidates.push_back(candidate);
        }
    }

    return candidates;
}

SwitchCandidate SwitchDecider::evaluateSwitch(
    const Layer& layer, int i, int j,
    Layer* leftLayer, Layer* rightLayer) {

    SwitchCandidate candidate;
    candidate.index1 = i;
    candidate.index2 = j;
    candidate.crossingReduction = 0;

    // This would require modifying the layer temporarily
    // For now, return a simple estimate
    candidate.score = 0.0;

    return candidate;
}

SwitchCandidate SwitchDecider::selectBest(
    const std::vector<SwitchCandidate>& candidates) {

    if (candidates.empty()) {
        return SwitchCandidate{-1, -1, 0, 0.0};
    }

    return *std::max_element(candidates.begin(), candidates.end(),
        [](const SwitchCandidate& a, const SwitchCandidate& b) {
            if (a.crossingReduction != b.crossingReduction) {
                return a.crossingReduction < b.crossingReduction;
            }
            return a.score < b.score;
        });
}

double SwitchDecider::calculateScore(int crossingReduction,
                                    double barycenterDiff,
                                    int portConstraintViolations) {
    // Weight the different factors
    return crossingReduction * 10.0 -
           std::abs(barycenterDiff) * 2.0 -
           portConstraintViolations * 5.0;
}

} // namespace crossing
} // namespace layered
} // namespace elk
