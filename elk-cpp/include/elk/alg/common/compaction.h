// Eclipse Layout Kernel - C++ Port
// Graph compaction algorithms
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../core/types.h"
#include "../../graph/graph.h"
#include <vector>
#include <unordered_map>

namespace elk {
namespace compaction {

// ============================================================================
// Compaction Direction
// ============================================================================

enum class CompactionDirection {
    HORIZONTAL,     // Compact horizontally (reduce width)
    VERTICAL,       // Compact vertically (reduce height)
    BOTH            // Compact in both dimensions
};

// ============================================================================
// Compaction Strategy
// ============================================================================

enum class CompactionStrategy {
    SIMPLE,         // Simple gap removal
    QUADRATIC,      // Quadratic programming
    SCANLINE,       // Scanline-based compaction
    LONGEST_PATH    // Longest path layering
};

// ============================================================================
// Compaction Configuration
// ============================================================================

struct CompactionConfig {
    CompactionStrategy strategy = CompactionStrategy::SCANLINE;
    CompactionDirection direction = CompactionDirection::BOTH;

    double minNodeSpacing = 10.0;      // Minimum space between nodes
    double minLayerSpacing = 20.0;     // Minimum space between layers
    double compactionRatio = 0.8;      // How aggressively to compact (0-1)

    bool preserveTopology = true;      // Maintain relative positions
    bool avoidOverlaps = true;         // Ensure no node overlaps
    int maxIterations = 20;            // For iterative algorithms
};

// ============================================================================
// Graph Compactor
// ============================================================================

class GraphCompactor {
public:
    // Compact an entire graph
    static void compact(Node* graph, const CompactionConfig& config);

    // Compact horizontally only
    static void compactHorizontal(Node* graph, const CompactionConfig& config);

    // Compact vertically only
    static void compactVertical(Node* graph, const CompactionConfig& config);

    // Remove whitespace gaps
    static void removeGaps(Node* graph, double minSpacing);

private:
    // Sort nodes by position
    static std::vector<Node*> sortNodesByPosition(
        Node* graph,
        bool horizontal);

    // Check if two nodes can be moved closer
    static bool canMoveCloser(
        const Node* a,
        const Node* b,
        double minSpacing,
        bool horizontal);

    // Calculate safe position to move node to
    static double calculateSafePosition(
        Node* node,
        const std::vector<Node*>& others,
        double targetPos,
        double minSpacing,
        bool horizontal);
};

// ============================================================================
// Scanline Compactor
// ============================================================================

class ScanlineCompactor {
public:
    struct Event {
        enum Type { START, END };
        Type type;
        double position;    // X or Y coordinate
        Node* node;

        bool operator<(const Event& other) const {
            return position < other.position;
        }
    };

    // Compact using scanline algorithm
    static void compact(
        Node* graph,
        CompactionDirection direction,
        const CompactionConfig& config);

private:
    // Generate events from nodes
    static std::vector<Event> generateEvents(
        Node* graph,
        bool horizontal);

    // Process scanline events
    static void processScanline(
        const std::vector<Event>& events,
        const CompactionConfig& config,
        bool horizontal);

    // Find overlapping nodes at scanline position
    static std::vector<Node*> findActiveNodes(
        const std::vector<Event>& events,
        size_t currentIndex);
};

// ============================================================================
// Layered Compactor (for layered layouts)
// ============================================================================

class LayeredCompactor {
public:
    struct Layer {
        std::vector<Node*> nodes;
        double position;    // X or Y coordinate of layer
        double thickness;   // Width or height of layer
    };

    // Compact layers together
    static void compactLayers(
        std::vector<Layer>& layers,
        const CompactionConfig& config);

    // Calculate minimum distance between two layers
    static double calculateMinDistance(
        const Layer& layer1,
        const Layer& layer2,
        double minSpacing);

    // Move layer to new position
    static void moveLayer(Layer& layer, double newPosition, bool horizontal);

private:
    // Build constraint graph between layers
    static std::unordered_map<int, std::vector<int>> buildConstraints(
        const std::vector<Layer>& layers,
        double minSpacing);

    // Calculate longest path to determine positions
    static std::vector<double> longestPath(
        const std::vector<Layer>& layers,
        const std::unordered_map<int, std::vector<int>>& constraints);
};

// ============================================================================
// Quadratic Compactor
// ============================================================================

class QuadraticCompactor {
public:
    struct Constraint {
        Node* node1;
        Node* node2;
        double minDistance;
        bool horizontal;
    };

    // Compact using quadratic programming
    static void compact(
        Node* graph,
        const CompactionConfig& config);

private:
    // Generate constraints from node positions
    static std::vector<Constraint> generateConstraints(
        Node* graph,
        double minSpacing);

    // Solve quadratic program to minimize total size
    static void solveQuadraticProgram(
        const std::vector<Node*>& nodes,
        const std::vector<Constraint>& constraints,
        bool horizontal);

    // Simple iterative solver
    static void iterativeSolve(
        const std::vector<Node*>& nodes,
        const std::vector<Constraint>& constraints,
        int maxIterations,
        bool horizontal);
};

// ============================================================================
// Rectangle Packer (for node placement)
// ============================================================================

class RectanglePacker {
public:
    struct Rectangle {
        Node* node;
        Rect bounds;
        int layer = -1;     // Optional layer assignment
    };

    // Pack rectangles tightly
    static void pack(
        std::vector<Rectangle>& rectangles,
        double containerWidth,
        double spacing);

    // Pack using shelf algorithm
    static void shelfPack(
        std::vector<Rectangle>& rectangles,
        double containerWidth,
        double spacing);

    // Pack using guillotine algorithm
    static void guillotinePack(
        std::vector<Rectangle>& rectangles,
        double containerWidth,
        double spacing);

private:
    // Find best position for rectangle
    static Point findBestPosition(
        const Rectangle& rect,
        const std::vector<Rectangle>& placed,
        double containerWidth,
        double spacing);

    // Check if position is valid (no overlaps)
    static bool isValidPosition(
        const Rect& bounds,
        const std::vector<Rectangle>& placed,
        double spacing);
};

// ============================================================================
// Helper Functions
// ============================================================================

// Calculate bounding box of all nodes
inline Rect calculateBoundingBox(const std::vector<Node*>& nodes) {
    if (nodes.empty()) return Rect();

    double minX = nodes[0]->position.x;
    double minY = nodes[0]->position.y;
    double maxX = nodes[0]->position.x + nodes[0]->size.width;
    double maxY = nodes[0]->position.y + nodes[0]->size.height;

    for (const Node* node : nodes) {
        minX = std::min(minX, node->position.x);
        minY = std::min(minY, node->position.y);
        maxX = std::max(maxX, node->position.x + node->size.width);
        maxY = std::max(maxY, node->position.y + node->size.height);
    }

    return Rect(minX, minY, maxX - minX, maxY - minY);
}

// Calculate compaction ratio achieved
inline double calculateCompactionRatio(const Rect& before, const Rect& after) {
    double beforeArea = before.width * before.height;
    double afterArea = after.width * after.height;
    return (beforeArea > 0) ? afterArea / beforeArea : 1.0;
}

// Check if two rectangles overlap
inline bool rectanglesOverlap(const Rect& a, const Rect& b, double spacing = 0) {
    Rect expanded = a;
    expanded.x -= spacing;
    expanded.y -= spacing;
    expanded.width += 2 * spacing;
    expanded.height += 2 * spacing;
    return expanded.intersects(b);
}

// Get distance between two rectangles
inline double rectangleDistance(const Rect& a, const Rect& b, bool horizontal) {
    if (horizontal) {
        if (a.right() < b.left()) return b.left() - a.right();
        if (b.right() < a.left()) return a.left() - b.right();
        return 0.0;  // Overlapping
    } else {
        if (a.bottom() < b.top()) return b.top() - a.bottom();
        if (b.bottom() < a.top()) return a.top() - b.bottom();
        return 0.0;  // Overlapping
    }
}

} // namespace compaction
} // namespace elk
