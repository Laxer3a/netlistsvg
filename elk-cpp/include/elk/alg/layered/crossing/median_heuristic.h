// Eclipse Layout Kernel - C++ Port
// Median heuristic for crossing minimization
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../../core/types.h"
#include "../layered_layout.h"
#include <vector>

namespace elk {
namespace layered {
namespace crossing {

// ============================================================================
// Median Heuristic Configuration
// ============================================================================

enum class MedianType {
    MEDIAN,      // Use true median
    BARYCENTER   // Use barycenter (average)
};

struct MedianHeuristicConfig {
    MedianType type = MedianType::MEDIAN;
    int maxIterations = 24;           // Maximum number of passes
    bool randomize = false;           // Randomize initial order
    bool balanceForward = true;       // Balance forward and backward passes
};

// ============================================================================
// Node with Median Value
// ============================================================================

struct NodeWithMedian {
    LNode* node = nullptr;
    double median = 0.0;
    int originalPosition = 0;

    bool operator<(const NodeWithMedian& other) const {
        if (std::abs(median - other.median) < 1e-6) {
            return originalPosition < other.originalPosition;
        }
        return median < other.median;
    }
};

// ============================================================================
// Median Heuristic
// ============================================================================

class MedianHeuristic {
public:
    // Main entry point
    static void minimize(std::vector<Layer>& layers,
                        const MedianHeuristicConfig& config);

private:
    // Forward and backward sweeps
    static void forwardSweep(std::vector<Layer>& layers,
                            const MedianHeuristicConfig& config);

    static void backwardSweep(std::vector<Layer>& layers,
                             const MedianHeuristicConfig& config);

    // Process single layer
    static void processLayer(Layer& layer, const Layer* fixedLayer,
                            bool useIncoming, const MedianHeuristicConfig& config);

    // Calculate median/barycenter
    static double calculateMedian(LNode* node, bool useIncoming);
    static double calculateBarycenter(LNode* node, bool useIncoming);

    // Sort layer by median values
    static void sortByMedian(Layer& layer, const std::vector<NodeWithMedian>& medians);

    // Get connected port positions
    static std::vector<double> getConnectedPositions(LNode* node, bool useIncoming);

    // Transpose pass to reduce crossings
    static int transpose(Layer& currentLayer, const Layer* previousLayer,
                        const Layer* nextLayer);
};

// ============================================================================
// Position Calculator
// ============================================================================

class PositionCalculator {
public:
    // Calculate position for node based on neighbors
    static double calculate(LNode* node, bool useIncoming, MedianType type);

private:
    // Get neighbor positions
    static std::vector<double> getNeighborPositions(LNode* node, bool useIncoming);

    // Calculate median from values
    static double median(std::vector<double>& values);

    // Calculate barycenter from values
    static double barycenter(const std::vector<double>& values);
};

// ============================================================================
// Layer Ordering
// ============================================================================

class LayerOrderer {
public:
    // Order layer based on median values
    static void order(Layer& layer, const Layer& fixedLayer,
                     bool useIncoming, MedianType type);

private:
    // Create node-median pairs
    static std::vector<NodeWithMedian> createNodeMedians(
        const Layer& layer, const Layer& fixedLayer,
        bool useIncoming, MedianType type);

    // Apply ordering to layer
    static void applyOrdering(Layer& layer, const std::vector<NodeWithMedian>& medians);

    // Handle nodes with same median
    static void resolveTies(std::vector<NodeWithMedian>& medians);
};

// ============================================================================
// Utility Functions
// ============================================================================

// Get position of node in layer
inline int getNodePositionInLayer(const Layer& layer, const LNode* node) {
    for (size_t i = 0; i < layer.nodes.size(); ++i) {
        if (layer.nodes[i] == node) {
            return i;
        }
    }
    return -1;
}

// Get all incoming edges for node
inline std::vector<LEdge*> getIncomingEdges(LNode* node) {
    std::vector<LEdge*> edges;
    for (LPort* port : node->ports) {
        for (LEdge* edge : port->incomingEdges) {
            edges.push_back(edge);
        }
    }
    return edges;
}

// Get all outgoing edges for node
inline std::vector<LEdge*> getOutgoingEdges(LNode* node) {
    std::vector<LEdge*> edges;
    for (LPort* port : node->ports) {
        for (LEdge* edge : port->outgoingEdges) {
            edges.push_back(edge);
        }
    }
    return edges;
}

// Get connected nodes (incoming or outgoing)
inline std::vector<LNode*> getConnectedNodes(LNode* node, bool incoming) {
    std::vector<LNode*> nodes;

    auto edges = incoming ? getIncomingEdges(node) : getOutgoingEdges(node);

    for (LEdge* edge : edges) {
        LNode* connected = incoming ?
            (edge->source ? edge->source->node : nullptr) :
            (edge->target ? edge->target->node : nullptr);

        if (connected) {
            nodes.push_back(connected);
        }
    }

    return nodes;
}

// Calculate median from a vector of values
inline double medianValue(std::vector<double> values) {
    if (values.empty()) return 0.0;

    std::sort(values.begin(), values.end());

    size_t n = values.size();
    if (n % 2 == 0) {
        return (values[n / 2 - 1] + values[n / 2]) / 2.0;
    } else {
        return values[n / 2];
    }
}

// Calculate average from a vector of values
inline double averageValue(const std::vector<double>& values) {
    if (values.empty()) return 0.0;

    double sum = 0.0;
    for (double v : values) {
        sum += v;
    }

    return sum / values.size();
}

// Check if layer order changed
inline bool orderChanged(const Layer& layer,
                        const std::vector<LNode*>& previousOrder) {
    if (layer.nodes.size() != previousOrder.size()) {
        return true;
    }

    for (size_t i = 0; i < layer.nodes.size(); ++i) {
        if (layer.nodes[i] != previousOrder[i]) {
            return true;
        }
    }

    return false;
}

// Save layer order
inline std::vector<LNode*> saveLayerOrder(const Layer& layer) {
    return layer.nodes;
}

} // namespace crossing
} // namespace layered
} // namespace elk
