// Eclipse Layout Kernel - C++ Port
// Node spacing cell system for adaptive spacing
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../core/types.h"
#include "../../graph/graph.h"
#include <vector>
#include <unordered_map>

namespace elk {
namespace spacing {

// ============================================================================
// Spacing Cell System
// ============================================================================

enum class SpacingStrategy {
    UNIFORM,          // Same spacing everywhere
    ADAPTIVE,         // Adapt to node density
    EDGE_BASED,       // Based on edge routing needs
    LABEL_BASED       // Based on label sizes
};

struct SpacingCell {
    Rect bounds;
    std::vector<Node*> nodes;
    double density = 0.0;
    double requiredSpacing = 0.0;
};

struct NodeSpacingConfig {
    SpacingStrategy strategy = SpacingStrategy::ADAPTIVE;

    double baseNodeSpacing = 20.0;
    double baseLabelSpacing = 10.0;
    double baseEdgeSpacing = 15.0;

    double minSpacing = 5.0;
    double maxSpacing = 100.0;

    bool considerLabels = true;
    bool considerPorts = true;
    bool considerEdgeDensity = true;

    int cellGridSize = 10;  // Number of cells per dimension
};

// ============================================================================
// Node Spacing Calculator
// ============================================================================

class NodeSpacingCalculator {
public:
    static void calculate(Node* graph, const NodeSpacingConfig& config);

private:
    // Build spacing grid
    static std::vector<SpacingCell> buildGrid(Node* graph,
                                              const NodeSpacingConfig& config);

    // Calculate density for each cell
    static void calculateDensity(std::vector<SpacingCell>& cells,
                                 const NodeSpacingConfig& config);

    // Determine required spacing
    static void determineSpacing(std::vector<SpacingCell>& cells,
                                const NodeSpacingConfig& config);

    // Apply spacing to nodes
    static void applySpacing(Node* graph,
                           const std::vector<SpacingCell>& cells,
                           const NodeSpacingConfig& config);
};

// ============================================================================
// Adaptive Spacing
// ============================================================================

class AdaptiveSpacing {
public:
    static double calculate(Node* node, const std::vector<Node*>& neighbors,
                           const NodeSpacingConfig& config);

private:
    static double calculateLabelSpacing(Node* node, const NodeSpacingConfig& config);
    static double calculatePortSpacing(Node* node, const NodeSpacingConfig& config);
    static double calculateEdgeSpacing(Node* node, const NodeSpacingConfig& config);
};

} // namespace spacing
} // namespace elk
