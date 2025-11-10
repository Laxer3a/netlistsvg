// Eclipse Layout Kernel - C++ Port
// Self-loops processor for layered layout
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../../core/types.h"
#include "../../../graph/graph.h"
#include <vector>
#include <unordered_map>

namespace elk {
namespace layered {

// Forward declarations
struct LNode;
struct LEdge;
struct LPort;

// ============================================================================
// Self-Loop Types
// ============================================================================

enum class SelfLoopPlacement {
    NORTH,       // Place self-loops on north side
    SOUTH,       // Place self-loops on south side
    EAST,        // Place self-loops on east side
    WEST,        // Place self-loops on west side
    STACKED      // Stack self-loops on best side
};

enum class SelfLoopRouting {
    ORTHOGONAL,  // 90-degree corners
    ROUNDED,     // Rounded corners
    SPLINE       // Bezier curves
};

struct SelfLoopEdge {
    LEdge* edge = nullptr;
    LNode* node = nullptr;
    LPort* sourcePort = nullptr;
    LPort* targetPort = nullptr;

    PortSide placementSide = PortSide::NORTH;
    double width = 0.0;          // Width of loop
    double height = 0.0;         // Height of loop
    int stackIndex = 0;          // Index in stack (for multiple loops)

    std::vector<Point> bendPoints;  // Route points
};

struct SelfLoopGroup {
    LNode* node = nullptr;
    std::vector<SelfLoopEdge> loops;
    PortSide preferredSide = PortSide::NORTH;

    double totalWidth = 0.0;
    double totalHeight = 0.0;
};

// ============================================================================
// Self-Loop Configuration
// ============================================================================

struct SelfLoopConfig {
    SelfLoopPlacement placement = SelfLoopPlacement::STACKED;
    SelfLoopRouting routing = SelfLoopRouting::ROUNDED;

    double loopSpacing = 10.0;        // Spacing between stacked loops
    double loopSize = 30.0;           // Default size of single loop
    double minLoopWidth = 20.0;       // Minimum width
    double minLoopHeight = 20.0;      // Minimum height

    bool optimizePlacement = true;    // Try to find best side for loops
    bool avoidOverlaps = true;        // Avoid overlapping other edges
    bool respectPortSides = true;     // Place loops near port sides
};

// ============================================================================
// Self-Loop Processor
// ============================================================================

class SelfLoopProcessor {
public:
    static void process(std::vector<LNode*>& nodes,
                       std::vector<LEdge*>& edges,
                       const SelfLoopConfig& config);

private:
    // Phase 1: Identify self-loops
    static std::vector<SelfLoopGroup> identifySelfLoops(
        const std::vector<LNode*>& nodes,
        const std::vector<LEdge*>& edges);

    // Phase 2: Determine placement side
    static void determinePlacement(std::vector<SelfLoopGroup>& groups,
                                  const SelfLoopConfig& config);

    static PortSide chooseBestSide(const SelfLoopGroup& group,
                                  const SelfLoopConfig& config);

    static int countEdgesOnSide(LNode* node, PortSide side);

    // Phase 3: Calculate loop geometry
    static void calculateGeometry(std::vector<SelfLoopGroup>& groups,
                                  const SelfLoopConfig& config);

    static void calculateSingleLoopSize(SelfLoopEdge& loop,
                                       const SelfLoopConfig& config);

    static void stackLoops(SelfLoopGroup& group, const SelfLoopConfig& config);

    // Phase 4: Route loops
    static void routeLoops(std::vector<SelfLoopGroup>& groups,
                          const SelfLoopConfig& config);

    static void routeOrthogonal(SelfLoopEdge& loop, const SelfLoopConfig& config);
    static void routeRounded(SelfLoopEdge& loop, const SelfLoopConfig& config);
    static void routeSpline(SelfLoopEdge& loop, const SelfLoopConfig& config);

    // Phase 5: Apply to graph
    static void applyToGraph(const std::vector<SelfLoopGroup>& groups);

    // Utilities
    static bool isSelfLoop(const LEdge* edge);
    static Point getPortPosition(LPort* port, LNode* node);
    static Rect getNodeBounds(LNode* node);
};

// ============================================================================
// Self-Loop Helper Functions
// ============================================================================

// Check if edge is a self-loop
inline bool isSelfLoop(LNode* source, LNode* target) {
    return source == target;
}

// Calculate spacing needed for self-loops on a side
inline double calculateSelfLoopSpacing(const SelfLoopGroup& group,
                                      PortSide side,
                                      const SelfLoopConfig& config) {
    if (group.preferredSide != side) return 0.0;

    if (side == PortSide::NORTH || side == PortSide::SOUTH) {
        return group.totalHeight;
    } else {
        return group.totalWidth;
    }
}

// Get corner point for self-loop routing
inline Point getCornerPoint(const Rect& nodeBounds, PortSide side, double offset) {
    switch (side) {
        case PortSide::NORTH:
            return Point{nodeBounds.x + nodeBounds.width / 2, nodeBounds.y - offset};
        case PortSide::SOUTH:
            return Point{nodeBounds.x + nodeBounds.width / 2,
                        nodeBounds.y + nodeBounds.height + offset};
        case PortSide::EAST:
            return Point{nodeBounds.x + nodeBounds.width + offset,
                        nodeBounds.y + nodeBounds.height / 2};
        case PortSide::WEST:
            return Point{nodeBounds.x - offset, nodeBounds.y + nodeBounds.height / 2};
        default:
            return Point{nodeBounds.x, nodeBounds.y};
    }
}

// Create rounded corner points for smooth routing
inline std::vector<Point> createRoundedPath(const Point& start, const Point& end,
                                           PortSide side, double radius) {
    std::vector<Point> points;
    points.push_back(start);

    // Calculate control points based on side
    double dx = end.x - start.x;
    double dy = end.y - start.y;

    if (side == PortSide::NORTH || side == PortSide::SOUTH) {
        double midX = (start.x + end.x) / 2;
        points.push_back(Point{midX, start.y});
        points.push_back(Point{midX, end.y});
    } else {
        double midY = (start.y + end.y) / 2;
        points.push_back(Point{start.x, midY});
        points.push_back(Point{end.x, midY});
    }

    points.push_back(end);
    return points;
}

} // namespace layered
} // namespace elk
