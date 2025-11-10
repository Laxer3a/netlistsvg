// Eclipse Layout Kernel - C++ Port
// Edge routing implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/alg/common/edge_routing.h"
#include <algorithm>
#include <cmath>

namespace elk {
namespace routing {

// ============================================================================
// OrthogonalEdgeRouter Implementation
// ============================================================================

void OrthogonalEdgeRouter::routeEdge(Edge* edge, const RoutingContext& ctx) {
    if (!edge || edge->sections.empty()) return;

    for (auto& section : edge->sections) {
        Point start = section.startPoint;
        Point end = section.endPoint;

        // Determine preferred routing direction
        Direction dir = Direction::RIGHT;
        if (std::abs(end.y - start.y) > std::abs(end.x - start.x)) {
            dir = (end.y > start.y) ? Direction::DOWN : Direction::UP;
        } else {
            dir = (end.x > start.x) ? Direction::RIGHT : Direction::LEFT;
        }

        // Create basic orthogonal path
        auto path = createOrthogonalPath(start, end, dir);

        // Collect obstacles (other nodes)
        std::vector<Rect> obstacles;
        if (ctx.graph) {
            Node* sourceNode = edge->sourceNodes.empty() ? nullptr : edge->sourceNodes[0];
            Node* targetNode = edge->targetNodes.empty() ? nullptr : edge->targetNodes[0];

            for (const auto& node : ctx.graph->children) {
                if (node.get() != sourceNode && node.get() != targetNode) {
                    obstacles.push_back(node->getBounds());
                }
            }
        }

        // Avoid obstacles
        if (!obstacles.empty()) {
            path = avoidObstacles(path, obstacles, ctx.edgeSpacing);
        }

        // Simplify path
        if (ctx.simplifyRoutes) {
            path = simplifyPath(path);
        }

        // Update edge section
        section.bendPoints = path;
    }
}

void OrthogonalEdgeRouter::routeAllEdges(Node* graph, const RoutingContext& ctx) {
    if (!graph) return;

    for (auto& edge : graph->edges) {
        routeEdge(edge.get(), ctx);
    }
}

std::vector<Point> OrthogonalEdgeRouter::createOrthogonalPath(
    const Point& start, const Point& end, Direction preferredDirection) {

    std::vector<Point> path;

    if (preferredDirection == Direction::RIGHT || preferredDirection == Direction::LEFT) {
        // Horizontal first
        double midX = (start.x + end.x) / 2;
        path.push_back(Point(midX, start.y));
        path.push_back(Point(midX, end.y));
    } else {
        // Vertical first
        double midY = (start.y + end.y) / 2;
        path.push_back(Point(start.x, midY));
        path.push_back(Point(end.x, midY));
    }

    return path;
}

std::vector<Point> OrthogonalEdgeRouter::avoidObstacles(
    const std::vector<Point>& path,
    const std::vector<Rect>& obstacles,
    double spacing) {

    std::vector<Point> result = path;

    // Simple obstacle avoidance: shift segments that intersect obstacles
    for (size_t i = 0; i < result.size(); ++i) {
        Point& p = result[i];

        for (const Rect& obstacle : obstacles) {
            Rect expanded = obstacle;
            expanded.x -= spacing;
            expanded.y -= spacing;
            expanded.width += 2 * spacing;
            expanded.height += 2 * spacing;

            if (expanded.contains(p)) {
                // Move point outside obstacle
                double dx1 = p.x - expanded.left();
                double dx2 = expanded.right() - p.x;
                double dy1 = p.y - expanded.top();
                double dy2 = expanded.bottom() - p.y;

                double minDist = std::min({dx1, dx2, dy1, dy2});

                if (minDist == dx1) p.x = expanded.left() - 1;
                else if (minDist == dx2) p.x = expanded.right() + 1;
                else if (minDist == dy1) p.y = expanded.top() - 1;
                else p.y = expanded.bottom() + 1;
            }
        }
    }

    return result;
}

std::vector<Point> OrthogonalEdgeRouter::simplifyPath(const std::vector<Point>& path) {
    if (path.size() <= 2) return path;

    std::vector<Point> simplified;
    simplified.push_back(path[0]);

    for (size_t i = 1; i < path.size() - 1; ++i) {
        const Point& prev = path[i - 1];
        const Point& curr = path[i];
        const Point& next = path[i + 1];

        // Check if current point is necessary (not collinear)
        bool horizontal = (std::abs(prev.y - curr.y) < EPSILON && std::abs(curr.y - next.y) < EPSILON);
        bool vertical = (std::abs(prev.x - curr.x) < EPSILON && std::abs(curr.x - next.x) < EPSILON);

        if (!horizontal && !vertical) {
            simplified.push_back(curr);
        }
    }

    simplified.push_back(path.back());
    return simplified;
}

// ============================================================================
// SplineEdgeRouter Implementation
// ============================================================================

void SplineEdgeRouter::routeEdge(Edge* edge, const SplineConfig& cfg) {
    if (!edge || edge->sections.empty()) return;

    for (auto& section : edge->sections) {
        std::vector<Point> waypoints;
        waypoints.push_back(section.startPoint);
        waypoints.insert(waypoints.end(), section.bendPoints.begin(), section.bendPoints.end());
        waypoints.push_back(section.endPoint);

        if (waypoints.size() < 2) continue;

        // Generate smooth spline
        auto spline = catmullRomSpline(waypoints, cfg.subdivisions, cfg.tension);

        // Update section with spline points
        section.bendPoints.clear();
        for (size_t i = 1; i < spline.size() - 1; ++i) {
            section.bendPoints.push_back(spline[i]);
        }
    }
}

void SplineEdgeRouter::routeAllEdges(Node* graph, const SplineConfig& cfg) {
    if (!graph) return;

    for (auto& edge : graph->edges) {
        routeEdge(edge.get(), cfg);
    }
}

std::vector<Point> SplineEdgeRouter::catmullRomSpline(
    const std::vector<Point>& controlPoints,
    int subdivisions,
    double tension) {

    if (controlPoints.size() < 2) return controlPoints;
    if (controlPoints.size() == 2) {
        // Just interpolate linearly
        std::vector<Point> result;
        for (int i = 0; i <= subdivisions; ++i) {
            double t = static_cast<double>(i) / subdivisions;
            result.push_back(controlPoints[0] * (1 - t) + controlPoints[1] * t);
        }
        return result;
    }

    std::vector<Point> result;
    result.push_back(controlPoints[0]);

    for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
        Point p0 = (i == 0) ? controlPoints[i] : controlPoints[i - 1];
        Point p1 = controlPoints[i];
        Point p2 = controlPoints[i + 1];
        Point p3 = (i + 2 < controlPoints.size()) ? controlPoints[i + 2] : controlPoints[i + 1];

        for (int j = 1; j <= subdivisions; ++j) {
            double t = static_cast<double>(j) / subdivisions;
            double t2 = t * t;
            double t3 = t2 * t;

            // Catmull-Rom formula
            double ax = -tension * t3 + 2 * tension * t2 - tension * t;
            double bx = (2 - tension) * t3 + (tension - 3) * t2 + 1;
            double cx = (tension - 2) * t3 + (3 - 2 * tension) * t2 + tension * t;
            double dx = tension * t3 - tension * t2;

            Point pt;
            pt.x = ax * p0.x + bx * p1.x + cx * p2.x + dx * p3.x;
            pt.y = ax * p0.y + bx * p1.y + cx * p2.y + dx * p3.y;

            result.push_back(pt);
        }
    }

    return result;
}

// ============================================================================
// PolylineRouter Implementation
// ============================================================================

void PolylineRouter::routeEdge(Edge* edge, const PolylineConfig& cfg) {
    if (!edge || edge->sections.empty()) return;

    for (auto& section : edge->sections) {
        std::vector<Point> points;
        points.push_back(section.startPoint);
        points.insert(points.end(), section.bendPoints.begin(), section.bendPoints.end());
        points.push_back(section.endPoint);

        if (cfg.straightenEdges) {
            points = optimizePolyline(points);
        }

        if (cfg.bendRadius > 0) {
            points = roundCorners(points, cfg.bendRadius);
        }

        // Update section
        section.bendPoints.clear();
        for (size_t i = 1; i < points.size() - 1; ++i) {
            section.bendPoints.push_back(points[i]);
        }
    }
}

std::vector<Point> PolylineRouter::optimizePolyline(const std::vector<Point>& points) {
    if (points.size() <= 2) return points;

    std::vector<Point> optimized;
    optimized.push_back(points[0]);

    for (size_t i = 1; i < points.size() - 1; ++i) {
        // Keep point if it changes direction significantly
        Point v1 = points[i] - points[i - 1];
        Point v2 = points[i + 1] - points[i];

        double dot = v1.x * v2.x + v1.y * v2.y;
        double len1 = v1.length();
        double len2 = v2.length();

        if (len1 > EPSILON && len2 > EPSILON) {
            double cosAngle = dot / (len1 * len2);
            if (cosAngle < 0.95) {  // Angle > ~18 degrees
                optimized.push_back(points[i]);
            }
        }
    }

    optimized.push_back(points.back());
    return optimized;
}

std::vector<Point> PolylineRouter::roundCorners(const std::vector<Point>& points, double radius) {
    if (points.size() <= 2 || radius <= 0) return points;

    std::vector<Point> rounded;
    rounded.push_back(points[0]);

    for (size_t i = 1; i < points.size() - 1; ++i) {
        Point prev = points[i - 1];
        Point curr = points[i];
        Point next = points[i + 1];

        // Add rounded corner approximation
        Point v1 = (prev - curr).normalized();
        Point v2 = (next - curr).normalized();

        Point corner1 = curr + v1 * radius;
        Point corner2 = curr + v2 * radius;

        rounded.push_back(corner1);
        rounded.push_back(curr);  // Actual corner
        rounded.push_back(corner2);
    }

    rounded.push_back(points.back());
    return rounded;
}

} // namespace routing
} // namespace elk
