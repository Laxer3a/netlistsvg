// Eclipse Layout Kernel - C++ Port
// Median heuristic implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/alg/layered/crossing/median_heuristic.h"
#include <algorithm>
#include <random>
#include <cmath>

namespace elk {
namespace layered {
namespace crossing {

void MedianHeuristic::minimize(std::vector<Layer>& layers,
                              const MedianHeuristicConfig& config) {
    if (layers.size() < 2) return;

    // Randomize initial order if requested
    if (config.randomize) {
        std::random_device rd;
        std::mt19937 gen(rd());
        for (auto& layer : layers) {
            std::shuffle(layer.nodes.begin(), layer.nodes.end(), gen);
        }
    }

    // Perform multiple iterations
    for (int iter = 0; iter < config.maxIterations; ++iter) {
        bool forward = (iter % 2 == 0) || !config.balanceForward;

        if (forward) {
            forwardSweep(layers, config);
        } else {
            backwardSweep(layers, config);
        }

        // Optional transpose pass
        bool improved = false;
        for (size_t i = 0; i < layers.size(); ++i) {
            const Layer* prev = i > 0 ? &layers[i - 1] : nullptr;
            const Layer* next = i < layers.size() - 1 ? &layers[i + 1] : nullptr;

            int reduction = transpose(layers[i], prev, next);
            if (reduction > 0) {
                improved = true;
            }
        }

        // Early exit if no improvement
        if (!improved && iter > 2) {
            break;
        }
    }
}

void MedianHeuristic::forwardSweep(std::vector<Layer>& layers,
                                   const MedianHeuristicConfig& config) {
    for (size_t i = 1; i < layers.size(); ++i) {
        processLayer(layers[i], &layers[i - 1], true, config);
    }
}

void MedianHeuristic::backwardSweep(std::vector<Layer>& layers,
                                    const MedianHeuristicConfig& config) {
    for (int i = layers.size() - 2; i >= 0; --i) {
        processLayer(layers[i], &layers[i + 1], false, config);
    }
}

void MedianHeuristic::processLayer(Layer& layer, const Layer* fixedLayer,
                                   bool useIncoming,
                                   const MedianHeuristicConfig& config) {
    if (!fixedLayer || layer.nodes.empty()) return;

    // Calculate medians for all nodes
    std::vector<NodeWithMedian> medians;
    medians.reserve(layer.nodes.size());

    for (size_t i = 0; i < layer.nodes.size(); ++i) {
        LNode* node = layer.nodes[i];
        NodeWithMedian nm;
        nm.node = node;
        nm.originalPosition = i;

        if (config.type == MedianType::MEDIAN) {
            nm.median = calculateMedian(node, useIncoming);
        } else {
            nm.median = calculateBarycenter(node, useIncoming);
        }

        medians.push_back(nm);
    }

    // Sort by median values
    sortByMedian(layer, medians);
}

double MedianHeuristic::calculateMedian(LNode* node, bool useIncoming) {
    auto positions = getConnectedPositions(node, useIncoming);
    return medianValue(positions);
}

double MedianHeuristic::calculateBarycenter(LNode* node, bool useIncoming) {
    auto positions = getConnectedPositions(node, useIncoming);
    return averageValue(positions);
}

void MedianHeuristic::sortByMedian(Layer& layer,
                                   const std::vector<NodeWithMedian>& medians) {
    // Create sorted copy
    std::vector<NodeWithMedian> sorted = medians;
    std::sort(sorted.begin(), sorted.end());

    // Apply new order
    for (size_t i = 0; i < sorted.size(); ++i) {
        layer.nodes[i] = sorted[i].node;
    }
}

std::vector<double> MedianHeuristic::getConnectedPositions(LNode* node,
                                                           bool useIncoming) {
    std::vector<double> positions;

    if (useIncoming) {
        for (LPort* port : node->ports) {
            for (LEdge* edge : port->incomingEdges) {
                if (edge->source && edge->source->node) {
                    positions.push_back(edge->source->node->layer);
                }
            }
        }
    } else {
        for (LPort* port : node->ports) {
            for (LEdge* edge : port->outgoingEdges) {
                if (edge->target && edge->target->node) {
                    positions.push_back(edge->target->node->layer);
                }
            }
        }
    }

    return positions;
}

int MedianHeuristic::transpose(Layer& currentLayer,
                              const Layer* previousLayer,
                              const Layer* nextLayer) {
    int improvements = 0;
    bool improved = true;

    while (improved) {
        improved = false;

        for (size_t i = 0; i < currentLayer.nodes.size() - 1; ++i) {
            // Count crossings before swap
            int crossingsBefore = 0;
            if (previousLayer) {
                // Count crossings with previous layer
                for (const auto& edge1 : getIncomingEdges(currentLayer.nodes[i])) {
                    for (const auto& edge2 : getIncomingEdges(currentLayer.nodes[i + 1])) {
                        LNode* s1 = edge1->source ? edge1->source->node : nullptr;
                        LNode* s2 = edge2->source ? edge2->source->node : nullptr;

                        if (s1 && s2) {
                            int pos1 = getNodePositionInLayer(*previousLayer, s1);
                            int pos2 = getNodePositionInLayer(*previousLayer, s2);

                            if (pos1 > pos2) {
                                ++crossingsBefore;
                            }
                        }
                    }
                }
            }

            if (nextLayer) {
                // Count crossings with next layer
                for (const auto& edge1 : getOutgoingEdges(currentLayer.nodes[i])) {
                    for (const auto& edge2 : getOutgoingEdges(currentLayer.nodes[i + 1])) {
                        LNode* t1 = edge1->target ? edge1->target->node : nullptr;
                        LNode* t2 = edge2->target ? edge2->target->node : nullptr;

                        if (t1 && t2) {
                            int pos1 = getNodePositionInLayer(*nextLayer, t1);
                            int pos2 = getNodePositionInLayer(*nextLayer, t2);

                            if (pos1 > pos2) {
                                ++crossingsBefore;
                            }
                        }
                    }
                }
            }

            // Try swap
            std::swap(currentLayer.nodes[i], currentLayer.nodes[i + 1]);

            // Count crossings after swap
            int crossingsAfter = 0;
            if (previousLayer) {
                for (const auto& edge1 : getIncomingEdges(currentLayer.nodes[i])) {
                    for (const auto& edge2 : getIncomingEdges(currentLayer.nodes[i + 1])) {
                        LNode* s1 = edge1->source ? edge1->source->node : nullptr;
                        LNode* s2 = edge2->source ? edge2->source->node : nullptr;

                        if (s1 && s2) {
                            int pos1 = getNodePositionInLayer(*previousLayer, s1);
                            int pos2 = getNodePositionInLayer(*previousLayer, s2);

                            if (pos1 > pos2) {
                                ++crossingsAfter;
                            }
                        }
                    }
                }
            }

            if (nextLayer) {
                for (const auto& edge1 : getOutgoingEdges(currentLayer.nodes[i])) {
                    for (const auto& edge2 : getOutgoingEdges(currentLayer.nodes[i + 1])) {
                        LNode* t1 = edge1->target ? edge1->target->node : nullptr;
                        LNode* t2 = edge2->target ? edge2->target->node : nullptr;

                        if (t1 && t2) {
                            int pos1 = getNodePositionInLayer(*nextLayer, t1);
                            int pos2 = getNodePositionInLayer(*nextLayer, t2);

                            if (pos1 > pos2) {
                                ++crossingsAfter;
                            }
                        }
                    }
                }
            }

            // Keep swap if it improves
            if (crossingsAfter < crossingsBefore) {
                improvements += crossingsBefore - crossingsAfter;
                improved = true;
            } else {
                // Undo swap
                std::swap(currentLayer.nodes[i], currentLayer.nodes[i + 1]);
            }
        }
    }

    return improvements;
}

// PositionCalculator implementation

double PositionCalculator::calculate(LNode* node, bool useIncoming, MedianType type) {
    auto positions = getNeighborPositions(node, useIncoming);

    if (type == MedianType::MEDIAN) {
        return median(positions);
    } else {
        return barycenter(positions);
    }
}

std::vector<double> PositionCalculator::getNeighborPositions(LNode* node,
                                                             bool useIncoming) {
    std::vector<double> positions;

    auto connectedNodes = getConnectedNodes(node, useIncoming);

    for (LNode* connected : connectedNodes) {
        positions.push_back(static_cast<double>(connected->layer));
    }

    return positions;
}

double PositionCalculator::median(std::vector<double>& values) {
    return medianValue(values);
}

double PositionCalculator::barycenter(const std::vector<double>& values) {
    return averageValue(values);
}

// LayerOrderer implementation

void LayerOrderer::order(Layer& layer, const Layer& fixedLayer,
                        bool useIncoming, MedianType type) {
    auto medians = createNodeMedians(layer, fixedLayer, useIncoming, type);
    resolveTies(medians);
    applyOrdering(layer, medians);
}

std::vector<NodeWithMedian> LayerOrderer::createNodeMedians(
    const Layer& layer, const Layer& fixedLayer,
    bool useIncoming, MedianType type) {

    std::vector<NodeWithMedian> medians;
    medians.reserve(layer.nodes.size());

    for (size_t i = 0; i < layer.nodes.size(); ++i) {
        NodeWithMedian nm;
        nm.node = layer.nodes[i];
        nm.originalPosition = i;
        nm.median = PositionCalculator::calculate(nm.node, useIncoming, type);
        medians.push_back(nm);
    }

    return medians;
}

void LayerOrderer::applyOrdering(Layer& layer,
                                const std::vector<NodeWithMedian>& medians) {
    std::vector<NodeWithMedian> sorted = medians;
    std::sort(sorted.begin(), sorted.end());

    for (size_t i = 0; i < sorted.size(); ++i) {
        layer.nodes[i] = sorted[i].node;
    }
}

void LayerOrderer::resolveTies(std::vector<NodeWithMedian>& medians) {
    // Nodes with same median keep their relative order
    // This is already handled by the stable sort and originalPosition field
}

} // namespace crossing
} // namespace layered
} // namespace elk
