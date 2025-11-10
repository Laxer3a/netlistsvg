// Eclipse Layout Kernel - C++ Port
// Label placement implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/alg/common/label_placement.h"
#include <algorithm>
#include <cmath>
#include <random>

namespace elk {
namespace labels {

// ============================================================================
// LabelPlacer Implementation
// ============================================================================

void LabelPlacer::placeAllLabels(Node* graph, const LabelPlacementConfig& config) {
    if (!graph) return;

    // Collect all labels
    std::vector<Label*> allLabels;

    // Place node labels
    for (auto& node : graph->children) {
        placeNodeLabels(node.get(), config);
        for (auto& label : node->labels) {
            allLabels.push_back(&label);
        }
    }

    // Place edge labels
    for (auto& edge : graph->edges) {
        placeEdgeLabels(edge.get(), config);
        for (auto& label : edge->labels) {
            allLabels.push_back(&label);
        }
    }

    // Remove overlaps if needed
    if (config.avoidOverlaps && !allLabels.empty()) {
        if (config.strategy == LabelPlacementStrategy::GREEDY) {
            AdvancedLabelPlacer::greedyPlacement(graph, allLabels, config);
        } else if (config.strategy == LabelPlacementStrategy::SIMULATED_ANNEALING) {
            AdvancedLabelPlacer::simulatedAnnealingPlacement(graph, allLabels, config);
        } else {
            removeOverlaps(allLabels, config.labelLabelSpacing);
        }
    }
}

void LabelPlacer::placeNodeLabels(Node* node, const LabelPlacementConfig& config) {
    if (!node) return;

    for (auto& label : node->labels) {
        label.position = calculateNodeLabelPosition(
            node, label, config.nodePlacement, config.nodeLabelSpacing);
    }
}

void LabelPlacer::placeEdgeLabels(Edge* edge, const LabelPlacementConfig& config) {
    if (!edge) return;

    for (auto& label : edge->labels) {
        label.position = calculateEdgeLabelPosition(edge, label, config.edgePlacement);
    }
}

void LabelPlacer::placePortLabels(Port* port, const LabelPlacementConfig& config) {
    if (!port) return;

    for (auto& label : port->labels) {
        // Place port label based on port side
        Point pos = port->position;

        switch (port->side) {
            case PortSide::NORTH:
                pos.y -= label.size.height + config.nodeLabelSpacing;
                break;
            case PortSide::SOUTH:
                pos.y += port->size.height + config.nodeLabelSpacing;
                break;
            case PortSide::EAST:
                pos.x += port->size.width + config.nodeLabelSpacing;
                break;
            case PortSide::WEST:
                pos.x -= label.size.width + config.nodeLabelSpacing;
                break;
            default:
                pos.x += port->size.width + config.nodeLabelSpacing;
                break;
        }

        label.position = pos;
    }
}

void LabelPlacer::removeOverlaps(std::vector<Label*>& labels, double spacing) {
    if (labels.size() < 2) return;

    // Simple greedy overlap removal
    for (size_t i = 0; i < labels.size(); ++i) {
        for (size_t j = i + 1; j < labels.size(); ++j) {
            if (labelsOverlap(*labels[i], *labels[j], spacing)) {
                resolveOverlap(*labels[j], *labels[i], spacing);
            }
        }
    }
}

Point LabelPlacer::calculateNodeLabelPosition(
    const Node* node,
    const Label& label,
    NodeLabelPlacement placement,
    double spacing) {

    if (!node) return Point(0, 0);

    Rect nodeRect = node->getBounds();
    return placeLabelAt(label, nodeRect, placement, spacing);
}

Point LabelPlacer::calculateEdgeLabelPosition(
    const Edge* edge,
    const Label& label,
    EdgeLabelPlacement placement) {

    if (!edge) return Point(0, 0);

    double ratio = 0.5;  // Default to center
    if (placement == EdgeLabelPlacement::HEAD) {
        ratio = 0.9;
    } else if (placement == EdgeLabelPlacement::TAIL) {
        ratio = 0.1;
    }

    Point edgePoint = getPointOnEdge(edge, ratio);

    // Center label on edge point
    return Point(
        edgePoint.x - label.size.width / 2,
        edgePoint.y - label.size.height / 2
    );
}

Point LabelPlacer::getPointOnEdge(const Edge* edge, double ratio) {
    if (!edge || edge->sections.empty()) return Point(0, 0);

    const auto& section = edge->sections[0];

    // Build complete path
    std::vector<Point> path;
    path.push_back(section.startPoint);
    path.insert(path.end(), section.bendPoints.begin(), section.bendPoints.end());
    path.push_back(section.endPoint);

    // Calculate total length
    double totalLength = 0.0;
    std::vector<double> segmentLengths;

    for (size_t i = 0; i < path.size() - 1; ++i) {
        double len = (path[i + 1] - path[i]).length();
        segmentLengths.push_back(len);
        totalLength += len;
    }

    // Find point at ratio
    double targetDist = totalLength * ratio;
    double currentDist = 0.0;

    for (size_t i = 0; i < segmentLengths.size(); ++i) {
        if (currentDist + segmentLengths[i] >= targetDist) {
            // Point is in this segment
            double segmentRatio = (targetDist - currentDist) / segmentLengths[i];
            return path[i] * (1 - segmentRatio) + path[i + 1] * segmentRatio;
        }
        currentDist += segmentLengths[i];
    }

    return section.endPoint;
}

bool LabelPlacer::labelsOverlap(const Label& a, const Label& b, double spacing) {
    Rect ra = getLabelBounds(a);
    Rect rb = getLabelBounds(b);

    ra.x -= spacing;
    ra.y -= spacing;
    ra.width += 2 * spacing;
    ra.height += 2 * spacing;

    return ra.intersects(rb);
}

void LabelPlacer::resolveOverlap(Label& a, const Label& b, double spacing) {
    Rect ra = getLabelBounds(a);
    Rect rb = getLabelBounds(b);

    // Calculate overlap
    double overlapX = 0, overlapY = 0;

    if (ra.right() > rb.left() && ra.left() < rb.right()) {
        overlapX = std::min(ra.right() - rb.left(), rb.right() - ra.left());
    }

    if (ra.bottom() > rb.top() && ra.top() < rb.bottom()) {
        overlapY = std::min(ra.bottom() - rb.top(), rb.bottom() - ra.top());
    }

    // Move in direction of smaller overlap
    if (overlapX > 0 && overlapY > 0) {
        if (overlapX < overlapY) {
            // Move horizontally
            a.position.x += (ra.center().x < rb.center().x) ? -(overlapX + spacing) : (overlapX + spacing);
        } else {
            // Move vertically
            a.position.y += (ra.center().y < rb.center().y) ? -(overlapY + spacing) : (overlapY + spacing);
        }
    }
}

// ============================================================================
// AdvancedLabelPlacer Implementation
// ============================================================================

void AdvancedLabelPlacer::greedyPlacement(
    Node* graph,
    std::vector<Label*>& labels,
    const LabelPlacementConfig& config) {

    // Sort labels by size (largest first) for better packing
    std::sort(labels.begin(), labels.end(), [](Label* a, Label* b) {
        return (a->size.width * a->size.height) > (b->size.width * b->size.height);
    });

    // Place each label, avoiding overlaps with already placed labels
    for (size_t i = 0; i < labels.size(); ++i) {
        Label* current = labels[i];
        Point bestPos = current->position;
        double bestScore = calculateQualityScore({current}, graph);

        // Try alternative positions
        std::vector<Point> candidates = {
            current->position,
            Point(current->position.x + 10, current->position.y),
            Point(current->position.x - 10, current->position.y),
            Point(current->position.x, current->position.y + 10),
            Point(current->position.x, current->position.y - 10)
        };

        for (const Point& candidate : candidates) {
            Point originalPos = current->position;
            current->position = candidate;

            // Check for overlaps with previously placed labels
            bool hasOverlap = false;
            for (size_t j = 0; j < i; ++j) {
                if (LabelPlacer::labelsOverlap(*current, *labels[j], config.labelLabelSpacing)) {
                    hasOverlap = true;
                    break;
                }
            }

            if (!hasOverlap) {
                double score = calculateQualityScore({current}, graph);
                if (score > bestScore) {
                    bestScore = score;
                    bestPos = candidate;
                }
            }

            current->position = originalPos;
        }

        current->position = bestPos;
    }
}

void AdvancedLabelPlacer::simulatedAnnealingPlacement(
    Node* graph,
    std::vector<Label*>& labels,
    const LabelPlacementConfig& config) {

    if (labels.empty()) return;

    double temperature = 100.0;
    double cooling = 0.95;
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(-10.0, 10.0);
    std::uniform_real_distribution<double> prob(0.0, 1.0);

    double currentScore = calculateQualityScore(labels, graph);

    for (int iter = 0; iter < config.maxIterations; ++iter) {
        // Random label
        Label* label = labels[rng() % labels.size()];
        Point oldPos = label->position;

        // Random perturbation
        label->position.x += dist(rng);
        label->position.y += dist(rng);

        double newScore = calculateQualityScore(labels, graph);
        double delta = newScore - currentScore;

        // Accept or reject
        if (delta > 0 || prob(rng) < std::exp(delta / temperature)) {
            currentScore = newScore;
        } else {
            label->position = oldPos;
        }

        temperature *= cooling;
    }
}

void AdvancedLabelPlacer::forceBasedPlacement(
    Node* graph,
    std::vector<Label*>& labels,
    const LabelPlacementConfig& config) {

    // Simple force-based label adjustment
    for (int iter = 0; iter < config.maxIterations; ++iter) {
        for (size_t i = 0; i < labels.size(); ++i) {
            Point force(0, 0);

            // Repulsion from other labels
            for (size_t j = 0; j < labels.size(); ++j) {
                if (i == j) continue;

                Point delta = labels[i]->position - labels[j]->position;
                double dist = delta.length();
                if (dist < EPSILON) dist = EPSILON;

                if (dist < 50.0) {  // Repulsion radius
                    force += delta.normalized() * (50.0 - dist) * 0.1;
                }
            }

            // Apply force
            labels[i]->position += force;
        }
    }
}

double AdvancedLabelPlacer::calculateQualityScore(
    const std::vector<Label*>& labels,
    const Node* graph) {

    double score = 1000.0;

    // Penalize overlaps
    score -= countOverlaps(labels) * 100.0;

    // Penalize edge occlusion
    score -= calculateEdgeOcclusion(labels, graph) * 10.0;

    return score;
}

int AdvancedLabelPlacer::countOverlaps(const std::vector<Label*>& labels) {
    int count = 0;
    for (size_t i = 0; i < labels.size(); ++i) {
        for (size_t j = i + 1; j < labels.size(); ++j) {
            if (LabelPlacer::labelsOverlap(*labels[i], *labels[j])) {
                count++;
            }
        }
    }
    return count;
}

double AdvancedLabelPlacer::calculateEdgeOcclusion(
    const std::vector<Label*>& labels,
    const Node* graph) {

    if (!graph) return 0.0;

    double occlusion = 0.0;
    for (const auto& label : labels) {
        for (const auto& edge : graph->edges) {
            if (labelIntersectsEdge(*label, edge.get())) {
                occlusion += 1.0;
            }
        }
    }
    return occlusion;
}

// ============================================================================
// LabelSizeEstimator Implementation
// ============================================================================

Size LabelSizeEstimator::estimateSize(const std::string& text, const FontMetrics& font) {
    if (text.empty()) return Size(0, font.characterHeight);

    return Size(
        text.length() * font.characterWidth,
        font.characterHeight
    );
}

Size LabelSizeEstimator::estimateMultiLineSize(
    const std::vector<std::string>& lines,
    const FontMetrics& font) {

    if (lines.empty()) return Size(0, 0);

    double maxWidth = 0.0;
    for (const auto& line : lines) {
        maxWidth = std::max(maxWidth, line.length() * font.characterWidth);
    }

    double height = lines.size() * font.characterHeight +
                   (lines.size() - 1) * font.lineSpacing;

    return Size(maxWidth, height);
}

std::vector<std::string> LabelSizeEstimator::wordWrap(
    const std::string& text,
    double maxWidth,
    const FontMetrics& font) {

    std::vector<std::string> lines;
    std::string currentLine;
    size_t maxChars = static_cast<size_t>(maxWidth / font.characterWidth);

    for (char c : text) {
        if (c == '\n' || currentLine.length() >= maxChars) {
            lines.push_back(currentLine);
            currentLine.clear();
        }
        if (c != '\n') {
            currentLine += c;
        }
    }

    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

    return lines;
}

} // namespace labels
} // namespace elk
