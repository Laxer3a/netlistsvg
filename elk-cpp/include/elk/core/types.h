// Eclipse Layout Kernel - C++ Port
// Basic geometric and utility types
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include <cmath>
#include <algorithm>

namespace elk {

// ============================================================================
// Basic Geometric Types
// ============================================================================

struct Point {
    double x = 0.0;
    double y = 0.0;

    Point() = default;
    Point(double x_, double y_) : x(x_), y(y_) {}

    Point operator+(const Point& other) const { return {x + other.x, y + other.y}; }
    Point operator-(const Point& other) const { return {x - other.x, y + other.y}; }
    Point operator*(double scale) const { return {x * scale, y * scale}; }
    Point operator/(double scale) const { return {x / scale, y / scale}; }

    Point& operator+=(const Point& other) { x += other.x; y += other.y; return *this; }
    Point& operator-=(const Point& other) { x -= other.x; y -= other.y; return *this; }

    double length() const { return std::sqrt(x * x + y * y); }
    double lengthSquared() const { return x * x + y * y; }

    Point normalized() const {
        double len = length();
        return len > 0 ? Point(x / len, y / len) : Point(0, 0);
    }
};

struct Size {
    double width = 0.0;
    double height = 0.0;

    Size() = default;
    Size(double w, double h) : width(w), height(h) {}
};

struct Rect {
    double x = 0.0;
    double y = 0.0;
    double width = 0.0;
    double height = 0.0;

    Rect() = default;
    Rect(double x_, double y_, double w, double h)
        : x(x_), y(y_), width(w), height(h) {}

    double left() const { return x; }
    double right() const { return x + width; }
    double top() const { return y; }
    double bottom() const { return y + height; }

    Point topLeft() const { return {x, y}; }
    Point topRight() const { return {x + width, y}; }
    Point bottomLeft() const { return {x, y + height}; }
    Point bottomRight() const { return {x + width, y + height}; }
    Point center() const { return {x + width / 2, y + height / 2}; }

    bool contains(const Point& p) const {
        return p.x >= x && p.x <= x + width &&
               p.y >= y && p.y <= y + height;
    }

    bool intersects(const Rect& other) const {
        return !(other.left() > right() || other.right() < left() ||
                 other.top() > bottom() || other.bottom() < top());
    }

    Rect union_(const Rect& other) const {
        double l = std::min(left(), other.left());
        double t = std::min(top(), other.top());
        double r = std::max(right(), other.right());
        double b = std::max(bottom(), other.bottom());
        return {l, t, r - l, b - t};
    }
};

struct Padding {
    double top = 0.0;
    double right = 0.0;
    double bottom = 0.0;
    double left = 0.0;

    Padding() = default;
    Padding(double all) : top(all), right(all), bottom(all), left(all) {}
    Padding(double t, double r, double b, double l)
        : top(t), right(r), bottom(b), left(l) {}
};

struct Margin {
    double top = 0.0;
    double right = 0.0;
    double bottom = 0.0;
    double left = 0.0;

    Margin() = default;
    Margin(double all) : top(all), right(all), bottom(all), left(all) {}
    Margin(double t, double r, double b, double l)
        : top(t), right(r), bottom(b), left(l) {}
};

// ============================================================================
// Enumerations
// ============================================================================

enum class Direction {
    UNDEFINED,
    RIGHT,    // Left to right
    LEFT,     // Right to left
    DOWN,     // Top to bottom
    UP        // Bottom to top
};

enum class PortSide {
    UNDEFINED,
    NORTH,
    SOUTH,
    EAST,
    WEST
};

enum class EdgeRouting {
    UNDEFINED,
    POLYLINE,      // Simple straight line segments
    ORTHOGONAL,    // Orthogonal (manhattan) routing
    SPLINES        // Curved splines
};

enum class NodePlacementStrategy {
    SIMPLE,
    LINEAR_SEGMENTS,
    BRANDES_KOEPF,
    NETWORK_SIMPLEX
};

enum class CrossingMinimizationStrategy {
    LAYER_SWEEP,
    INTERACTIVE,
    NONE
};

// ============================================================================
// Constants
// ============================================================================

constexpr double EPSILON = 1e-6;
constexpr double DEFAULT_SPACING = 20.0;
constexpr double DEFAULT_PORT_SPACING = 10.0;
constexpr double DEFAULT_BORDER_SPACING = 12.0;

} // namespace elk
