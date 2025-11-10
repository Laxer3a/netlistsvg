// Eclipse Layout Kernel - C++ Port
// Label placement optimization
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../core/types.h"
#include "../../graph/graph.h"
#include <vector>

namespace elk {
namespace labels {

// ============================================================================
// Label Placement Strategy
// ============================================================================

enum class LabelPlacementStrategy {
    SIMPLE,             // Basic centered placement
    GREEDY,             // Greedy non-overlapping placement
    SIMULATED_ANNEALING,// Optimal placement using simulated annealing
    FORCE_BASED         // Force-directed label placement
};

// ============================================================================
// Node Label Placement
// ============================================================================

enum class NodeLabelPlacement {
    INSIDE_CENTER,      // Center of node
    INSIDE_TOP,         // Top inside
    INSIDE_BOTTOM,      // Bottom inside
    OUTSIDE_TOP,        // Above node
    OUTSIDE_BOTTOM,     // Below node
    OUTSIDE_LEFT,       // Left of node
    OUTSIDE_RIGHT       // Right of node
};

// ============================================================================
// Edge Label Placement
// ============================================================================

enum class EdgeLabelPlacement {
    CENTER,             // Middle of edge
    HEAD,               // Near target
    TAIL,               // Near source
    INLINE              // Along edge path
};

// ============================================================================
// Label Placement Configuration
// ============================================================================

struct LabelPlacementConfig {
    LabelPlacementStrategy strategy = LabelPlacementStrategy::GREEDY;
    NodeLabelPlacement nodePlacement = NodeLabelPlacement::INSIDE_CENTER;
    EdgeLabelPlacement edgePlacement = EdgeLabelPlacement::CENTER;

    double nodeLabelSpacing = 2.0;     // Spacing between label and node border
    double edgeLabelSpacing = 2.0;     // Spacing between label and edge
    double labelLabelSpacing = 5.0;    // Spacing between labels

    bool avoidOverlaps = true;         // Try to avoid label overlaps
    bool preferStraightEdges = true;   // Prefer labels not to obscure edges
    int maxIterations = 100;           // For iterative algorithms
};

// ============================================================================
// Label Placer
// ============================================================================

class LabelPlacer {
public:
    // Place all labels in a graph
    static void placeAllLabels(Node* graph, const LabelPlacementConfig& config);

    // Place node labels
    static void placeNodeLabels(Node* node, const LabelPlacementConfig& config);

    // Place edge labels
    static void placeEdgeLabels(Edge* edge, const LabelPlacementConfig& config);

    // Place port labels
    static void placePortLabels(Port* port, const LabelPlacementConfig& config);

    // Remove overlaps between labels
    static void removeOverlaps(std::vector<Label*>& labels, double spacing);

    // Check if two labels overlap (public for use by AdvancedLabelPlacer)
    static bool labelsOverlap(const Label& a, const Label& b, double spacing = 0);

private:
    // Calculate position for node label
    static Point calculateNodeLabelPosition(
        const Node* node,
        const Label& label,
        NodeLabelPlacement placement,
        double spacing);

    // Calculate position for edge label
    static Point calculateEdgeLabelPosition(
        const Edge* edge,
        const Label& label,
        EdgeLabelPlacement placement);

    // Find point along edge path at given ratio (0.0 to 1.0)
    static Point getPointOnEdge(const Edge* edge, double ratio);

    // Move label to avoid overlap
    static void resolveOverlap(Label& a, const Label& b, double spacing);
};

// ============================================================================
// Advanced Label Placement Algorithms
// ============================================================================

class AdvancedLabelPlacer {
public:
    // Greedy label placement (fast, good quality)
    static void greedyPlacement(
        Node* graph,
        std::vector<Label*>& labels,
        const LabelPlacementConfig& config);

    // Simulated annealing (slow, high quality)
    static void simulatedAnnealingPlacement(
        Node* graph,
        std::vector<Label*>& labels,
        const LabelPlacementConfig& config);

    // Force-based placement
    static void forceBasedPlacement(
        Node* graph,
        std::vector<Label*>& labels,
        const LabelPlacementConfig& config);

private:
    // Calculate placement quality score
    static double calculateQualityScore(
        const std::vector<Label*>& labels,
        const Node* graph);

    // Count label-label overlaps
    static int countOverlaps(const std::vector<Label*>& labels);

    // Calculate total edge occlusion
    static double calculateEdgeOcclusion(
        const std::vector<Label*>& labels,
        const Node* graph);
};

// ============================================================================
// Label Size Estimation
// ============================================================================

class LabelSizeEstimator {
public:
    struct FontMetrics {
        double characterWidth = 8.0;   // Average character width
        double characterHeight = 14.0;  // Character height
        double lineSpacing = 2.0;       // Extra space between lines
    };

    // Estimate label size from text
    static Size estimateSize(const std::string& text, const FontMetrics& font);

    // Estimate multi-line label size
    static Size estimateMultiLineSize(
        const std::vector<std::string>& lines,
        const FontMetrics& font);

    // Word wrap text to fit width
    static std::vector<std::string> wordWrap(
        const std::string& text,
        double maxWidth,
        const FontMetrics& font);
};

// ============================================================================
// Helper Functions
// ============================================================================

// Get bounding box of label
inline Rect getLabelBounds(const Label& label) {
    return Rect(label.position.x, label.position.y,
                label.size.width, label.size.height);
}

// Center label within rectangle
inline Point centerLabelInRect(const Label& label, const Rect& rect) {
    return Point(
        rect.x + (rect.width - label.size.width) / 2,
        rect.y + (rect.height - label.size.height) / 2
    );
}

// Place label at cardinal position relative to rectangle
inline Point placeLabelAt(const Label& label, const Rect& rect,
                         NodeLabelPlacement placement, double spacing) {
    Point pos;

    switch (placement) {
        case NodeLabelPlacement::INSIDE_CENTER:
            return centerLabelInRect(label, rect);

        case NodeLabelPlacement::INSIDE_TOP:
            pos.x = rect.x + (rect.width - label.size.width) / 2;
            pos.y = rect.y + spacing;
            break;

        case NodeLabelPlacement::INSIDE_BOTTOM:
            pos.x = rect.x + (rect.width - label.size.width) / 2;
            pos.y = rect.y + rect.height - label.size.height - spacing;
            break;

        case NodeLabelPlacement::OUTSIDE_TOP:
            pos.x = rect.x + (rect.width - label.size.width) / 2;
            pos.y = rect.y - label.size.height - spacing;
            break;

        case NodeLabelPlacement::OUTSIDE_BOTTOM:
            pos.x = rect.x + (rect.width - label.size.width) / 2;
            pos.y = rect.y + rect.height + spacing;
            break;

        case NodeLabelPlacement::OUTSIDE_LEFT:
            pos.x = rect.x - label.size.width - spacing;
            pos.y = rect.y + (rect.height - label.size.height) / 2;
            break;

        case NodeLabelPlacement::OUTSIDE_RIGHT:
            pos.x = rect.x + rect.width + spacing;
            pos.y = rect.y + (rect.height - label.size.height) / 2;
            break;

        default:
            return centerLabelInRect(label, rect);
    }

    return pos;
}

// Check if line segment intersects rectangle
inline bool intersectsRect(const Point& p1, const Point& p2, const Rect& rect) {
    // Check if either endpoint is inside rectangle
    if (rect.contains(p1) || rect.contains(p2)) {
        return true;
    }

    // Check line-rectangle intersection (simplified)
    // Check intersection with each edge of the rectangle
    double minX = std::min(p1.x, p2.x);
    double maxX = std::max(p1.x, p2.x);
    double minY = std::min(p1.y, p2.y);
    double maxY = std::max(p1.y, p2.y);

    // Bounding box check
    if (maxX < rect.x || minX > rect.x + rect.width ||
        maxY < rect.y || minY > rect.y + rect.height) {
        return false;
    }

    return true;  // Simplified - any bounding box overlap counts as intersection
}

// Check if label intersects edge
inline bool labelIntersectsEdge(const Label& label, const Edge* edge) {
    if (!edge || edge->sections.empty()) return false;

    Rect labelRect = getLabelBounds(label);

    for (const auto& section : edge->sections) {
        Point prev = section.startPoint;

        for (const auto& bp : section.bendPoints) {
            if (intersectsRect(prev, bp, labelRect)) {
                return true;
            }
            prev = bp;
        }

        if (intersectsRect(prev, section.endPoint, labelRect)) {
            return true;
        }
    }

    return false;
}

} // namespace labels
} // namespace elk
