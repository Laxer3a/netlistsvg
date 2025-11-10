// Eclipse Layout Kernel - C++ Port
// Node spacing implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/alg/common/node_spacing.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace elk {
namespace spacing {

void NodeSpacingCalculator::calculate(Node* graph, const NodeSpacingConfig& config) {
    if (!graph || graph->children.empty()) return;

    auto cells = buildGrid(graph, config);
    if (cells.empty()) return;

    calculateDensity(cells, config);
    determineSpacing(cells, config);
    applySpacing(graph, cells, config);
}

std::vector<SpacingCell> NodeSpacingCalculator::buildGrid(
    Node* graph, const NodeSpacingConfig& config) {

    std::vector<SpacingCell> cells;

    // Calculate bounding box of all children
    if (graph->children.empty()) return cells;

    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    for (const auto& child : graph->children) {
        minX = std::min(minX, child->position.x);
        maxX = std::max(maxX, child->position.x + child->size.width);
        minY = std::min(minY, child->position.y);
        maxY = std::max(maxY, child->position.y + child->size.height);
    }

    double width = maxX - minX;
    double height = maxY - minY;

    if (width <= 0 || height <= 0) return cells;

    // Create grid
    double cellWidth = width / config.cellGridSize;
    double cellHeight = height / config.cellGridSize;

    for (int i = 0; i < config.cellGridSize; ++i) {
        for (int j = 0; j < config.cellGridSize; ++j) {
            SpacingCell cell;
            cell.bounds = Rect{
                minX + i * cellWidth,
                minY + j * cellHeight,
                cellWidth,
                cellHeight
            };
            cells.push_back(cell);
        }
    }

    // Assign nodes to cells
    for (auto& child : graph->children) {
        double cx = child->position.x + child->size.width / 2;
        double cy = child->position.y + child->size.height / 2;

        // Find containing cell
        int cellX = static_cast<int>((cx - minX) / cellWidth);
        int cellY = static_cast<int>((cy - minY) / cellHeight);

        cellX = std::clamp(cellX, 0, config.cellGridSize - 1);
        cellY = std::clamp(cellY, 0, config.cellGridSize - 1);

        int cellIdx = cellY * config.cellGridSize + cellX;
        if (cellIdx >= 0 && cellIdx < cells.size()) {
            cells[cellIdx].nodes.push_back(child.get());
        }
    }

    return cells;
}

void NodeSpacingCalculator::calculateDensity(
    std::vector<SpacingCell>& cells, const NodeSpacingConfig& config) {

    for (auto& cell : cells) {
        if (cell.nodes.empty()) {
            cell.density = 0.0;
            continue;
        }

        // Calculate density as nodes per unit area
        double cellArea = cell.bounds.width * cell.bounds.height;
        if (cellArea > 0) {
            // Calculate total node area in cell
            double nodeArea = 0;
            for (Node* node : cell.nodes) {
                nodeArea += node->size.width * node->size.height;
            }

            // Density is ratio of node area to cell area
            cell.density = nodeArea / cellArea;
        }
    }
}

void NodeSpacingCalculator::determineSpacing(
    std::vector<SpacingCell>& cells, const NodeSpacingConfig& config) {

    for (auto& cell : cells) {
        // Base spacing
        double spacing = config.baseNodeSpacing;

        // Adjust based on density
        if (cell.density > 0.7) {
            // High density - reduce spacing
            spacing *= 0.7;
        } else if (cell.density < 0.3) {
            // Low density - increase spacing
            spacing *= 1.3;
        }

        // Consider labels if enabled
        if (config.considerLabels) {
            double maxLabelSize = 0;
            for (Node* node : cell.nodes) {
                for (const auto& label : node->labels) {
                    maxLabelSize = std::max(maxLabelSize,
                                          std::max(label.size.width, label.size.height));
                }
            }
            spacing = std::max(spacing, maxLabelSize + config.baseLabelSpacing);
        }

        // Consider ports if enabled
        if (config.considerPorts) {
            int maxPorts = 0;
            for (Node* node : cell.nodes) {
                maxPorts = std::max(maxPorts, static_cast<int>(node->ports.size()));
            }
            if (maxPorts > 4) {
                spacing *= 1.2; // More spacing for nodes with many ports
            }
        }

        // Clamp to min/max
        cell.requiredSpacing = std::clamp(spacing, config.minSpacing, config.maxSpacing);
    }
}

void NodeSpacingCalculator::applySpacing(
    Node* graph, const std::vector<SpacingCell>& cells,
    const NodeSpacingConfig& config) {

    // This is informational - actual spacing is applied during layout
    // Store spacing requirements as node properties for layout algorithms to use

    for (const auto& cell : cells) {
        for (Node* node : cell.nodes) {
            // Store the required spacing as a property
            node->setProperty("spacing.required", cell.requiredSpacing);
        }
    }
}

double AdaptiveSpacing::calculate(Node* node, const std::vector<Node*>& neighbors,
                                  const NodeSpacingConfig& config) {
    double spacing = config.baseNodeSpacing;

    if (config.considerLabels) {
        spacing = std::max(spacing, calculateLabelSpacing(node, config));
    }

    if (config.considerPorts) {
        spacing = std::max(spacing, calculatePortSpacing(node, config));
    }

    if (config.considerEdgeDensity) {
        spacing = std::max(spacing, calculateEdgeSpacing(node, config));
    }

    // Consider neighbor density
    if (!neighbors.empty()) {
        // More neighbors = potentially more spacing needed
        double neighborFactor = 1.0 + (neighbors.size() * 0.05);
        spacing *= neighborFactor;
    }

    return std::clamp(spacing, config.minSpacing, config.maxSpacing);
}

double AdaptiveSpacing::calculateLabelSpacing(Node* node,
                                             const NodeSpacingConfig& config) {
    if (!node || node->labels.empty()) {
        return config.baseLabelSpacing;
    }

    // Find maximum label dimension
    double maxLabelSize = 0;
    for (const auto& label : node->labels) {
        maxLabelSize = std::max(maxLabelSize,
                              std::max(label.size.width, label.size.height));
    }

    // Add base spacing
    return maxLabelSize + config.baseLabelSpacing;
}

double AdaptiveSpacing::calculatePortSpacing(Node* node,
                                            const NodeSpacingConfig& config) {
    if (!node) return config.baseNodeSpacing;

    // More ports = more spacing needed for edge routing
    int portCount = node->ports.size();

    if (portCount <= 2) {
        return config.baseNodeSpacing;
    } else if (portCount <= 4) {
        return config.baseNodeSpacing * 1.2;
    } else if (portCount <= 8) {
        return config.baseNodeSpacing * 1.5;
    } else {
        return config.baseNodeSpacing * 2.0;
    }
}

double AdaptiveSpacing::calculateEdgeSpacing(Node* node,
                                            const NodeSpacingConfig& config) {
    if (!node) return config.baseEdgeSpacing;

    // Count total edges
    int edgeCount = 0;
    for (const auto& port : node->ports) {
        edgeCount += port->incomingEdges.size() + port->outgoingEdges.size();
    }

    // More edges = more spacing needed
    if (edgeCount <= 2) {
        return config.baseEdgeSpacing;
    } else if (edgeCount <= 5) {
        return config.baseEdgeSpacing * 1.3;
    } else if (edgeCount <= 10) {
        return config.baseEdgeSpacing * 1.6;
    } else {
        return config.baseEdgeSpacing * 2.0;
    }
}

} // namespace spacing
} // namespace elk
