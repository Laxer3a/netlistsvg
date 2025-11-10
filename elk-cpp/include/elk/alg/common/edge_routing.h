// Eclipse Layout Kernel - C++ Port
// Advanced edge routing algorithms
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../core/types.h"
#include "../../graph/graph.h"
#include <vector>
#include <cmath>

namespace elk {
namespace routing {

// ============================================================================
// Orthogonal Edge Router
// ============================================================================

class OrthogonalEdgeRouter {
public:
    struct RoutingContext {
        Node* graph;
        double edgeSpacing = 5.0;
        double minSegmentLength = 10.0;
        bool simplifyRoutes = true;
    };

    // Route a single edge orthogonally (Manhattan style)
    static void routeEdge(Edge* edge, const RoutingContext& ctx);

    // Route all edges in a graph
    static void routeAllEdges(Node* graph, const RoutingContext& ctx);

private:
    // Create orthogonal path between two points
    static std::vector<Point> createOrthogonalPath(
        const Point& start, const Point& end,
        Direction preferredDirection = Direction::RIGHT);

    // Add intermediate waypoints to avoid node overlaps
    static std::vector<Point> avoidObstacles(
        const std::vector<Point>& path,
        const std::vector<Rect>& obstacles,
        double spacing);

    // Simplify path by removing unnecessary waypoints
    static std::vector<Point> simplifyPath(const std::vector<Point>& path);
};

// ============================================================================
// Spline Edge Router
// ============================================================================

class SplineEdgeRouter {
public:
    struct SplineConfig {
        int subdivisions = 10;      // Points per curve segment
        double tension = 0.5;       // 0 = sharp corners, 1 = smooth curves
        bool avoidNodes = true;
    };

    // Route edge as smooth spline
    static void routeEdge(Edge* edge, const SplineConfig& cfg);

    // Route all edges with splines
    static void routeAllEdges(Node* graph, const SplineConfig& cfg);

private:
    // Catmull-Rom spline interpolation
    static std::vector<Point> catmullRomSpline(
        const std::vector<Point>& controlPoints,
        int subdivisions,
        double tension);

    // Bezier curve interpolation
    static std::vector<Point> bezierCurve(
        const Point& p0, const Point& p1,
        const Point& p2, const Point& p3,
        int subdivisions);

    // Calculate control points for smooth curves
    static std::vector<Point> calculateControlPoints(
        const std::vector<Point>& waypoints,
        double tension);
};

// ============================================================================
// Polyline Router (improved)
// ============================================================================

class PolylineRouter {
public:
    struct PolylineConfig {
        double bendRadius = 0.0;    // Rounded corners (0 = sharp)
        bool straightenEdges = true;
        double minAngle = 15.0;     // Minimum angle between segments (degrees)
    };

    // Route edge as polyline with optional improvements
    static void routeEdge(Edge* edge, const PolylineConfig& cfg);

    // Optimize polyline by removing unnecessary bends
    static std::vector<Point> optimizePolyline(const std::vector<Point>& points);

    // Create rounded corners at bend points
    static std::vector<Point> roundCorners(const std::vector<Point>& points, double radius);
};

// ============================================================================
// Channel Router (for layered graphs)
// ============================================================================

class ChannelRouter {
public:
    struct Channel {
        int layer;
        double y;           // Y position of channel
        double height;      // Channel height
    };

    struct ChannelConfig {
        std::vector<Channel> channels;
        double channelSpacing = 5.0;
        bool minimizeBends = true;
    };

    // Route edges through horizontal channels (for layered layout)
    static void routeInChannels(
        const std::vector<Edge*>& edges,
        const ChannelConfig& cfg);

private:
    // Assign edges to channels to minimize conflicts
    static int assignChannel(Edge* edge, const ChannelConfig& cfg);

    // Create routing path through assigned channel
    static std::vector<Point> routeThroughChannel(
        const Point& start, const Point& end,
        int channelIndex, const ChannelConfig& cfg);
};

// ============================================================================
// Helper functions
// ============================================================================

// Check if line segment intersects rectangle
inline bool intersectsRect(const Point& p1, const Point& p2, const Rect& rect) {
    // Simple AABB line intersection test
    double minX = std::min(p1.x, p2.x);
    double maxX = std::max(p1.x, p2.x);
    double minY = std::min(p1.y, p2.y);
    double maxY = std::max(p1.y, p2.y);

    return !(maxX < rect.x || minX > rect.x + rect.width ||
             maxY < rect.y || minY > rect.y + rect.height);
}

// Calculate bend point for orthogonal routing
inline Point calculateBendPoint(const Point& from, const Point& to, Direction dir) {
    switch (dir) {
        case Direction::RIGHT:
        case Direction::LEFT:
            return Point(to.x, from.y);
        case Direction::DOWN:
        case Direction::UP:
            return Point(from.x, to.y);
        default:
            return Point((from.x + to.x) / 2, (from.y + to.y) / 2);
    }
}

// Distance from point to line segment
inline double pointToSegmentDistance(const Point& p, const Point& a, const Point& b) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double len2 = dx * dx + dy * dy;

    if (len2 < EPSILON) {
        return (p - a).length();
    }

    double t = std::max(0.0, std::min(1.0, ((p.x - a.x) * dx + (p.y - a.y) * dy) / len2));
    Point proj = Point(a.x + t * dx, a.y + t * dy);
    return (p - proj).length();
}

} // namespace routing
} // namespace elk
