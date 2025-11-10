// Eclipse Layout Kernel - C++ Port
// Port constraints and ordering
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../core/types.h"
#include "../../graph/graph.h"
#include <vector>
#include <algorithm>

namespace elk {
namespace ports {

// ============================================================================
// Port Constraint Types
// ============================================================================

enum class PortConstraint {
    FREE,               // Ports can be placed anywhere
    FIXED_SIDE,         // Port side is fixed, order is free
    FIXED_ORDER,        // Port side and order are fixed
    FIXED_POSITION,     // Port position is completely fixed
    FIXED_RATIO         // Port position as ratio of node side
};

// ============================================================================
// Port Placement Strategy
// ============================================================================

enum class PortPlacementStrategy {
    SIMPLE,             // Evenly distribute ports
    MINIMIZE_CROSSINGS, // Minimize edge crossings
    BARYCENTER,         // Place based on connected node positions
    NETWORK_SIMPLEX     // Optimal placement using network simplex
};

// ============================================================================
// Port Order Constraint
// ============================================================================

struct PortOrderConstraint {
    std::vector<Port*> orderedPorts;  // Ports in desired order
    PortSide side;                     // Side this constraint applies to
    bool strict = true;                // Must maintain exact order

    PortOrderConstraint() = default;
    PortOrderConstraint(PortSide s, const std::vector<Port*>& ports, bool isStrict = true)
        : orderedPorts(ports), side(s), strict(isStrict) {}
};

// ============================================================================
// Port Distribution Configuration
// ============================================================================

struct PortDistribution {
    double spacing = 10.0;           // Minimum spacing between ports
    double margin = 5.0;             // Margin from node corners
    bool equalSpacing = true;        // Equal spacing vs. barycenter
    bool centerUnused = true;        // Center ports if not all space used
};

// ============================================================================
// Port Constraint Manager
// ============================================================================

class PortConstraintManager {
public:
    // Set constraint for a specific port
    void setPortConstraint(Port* port, PortConstraint constraint) {
        portConstraints_[port] = constraint;
    }

    // Get constraint for a port
    PortConstraint getPortConstraint(Port* port) const {
        auto it = portConstraints_.find(port);
        return (it != portConstraints_.end()) ? it->second : PortConstraint::FREE;
    }

    // Add port order constraint
    void addOrderConstraint(const PortOrderConstraint& constraint) {
        orderConstraints_.push_back(constraint);
    }

    // Get all order constraints for a side
    std::vector<PortOrderConstraint> getOrderConstraints(PortSide side) const {
        std::vector<PortOrderConstraint> result;
        for (const auto& c : orderConstraints_) {
            if (c.side == side) {
                result.push_back(c);
            }
        }
        return result;
    }

    // Clear all constraints
    void clear() {
        portConstraints_.clear();
        orderConstraints_.clear();
    }

private:
    std::unordered_map<Port*, PortConstraint> portConstraints_;
    std::vector<PortOrderConstraint> orderConstraints_;
};

// ============================================================================
// Port Placement Algorithm
// ============================================================================

class PortPlacer {
public:
    struct PlacementContext {
        Node* node;
        PortConstraintManager* constraints = nullptr;
        PortPlacementStrategy strategy = PortPlacementStrategy::SIMPLE;
        PortDistribution distribution;
    };

    // Place all ports on a node according to constraints and strategy
    static void placePorts(const PlacementContext& ctx);

    // Place ports on a specific side
    static void placePortsOnSide(
        Node* node,
        PortSide side,
        const std::vector<Port*>& ports,
        const PortDistribution& dist);

    // Assign ports to sides based on connectivity
    static void assignPortSides(
        Node* node,
        const PortConstraintManager* constraints = nullptr);

    // Order ports on each side to minimize crossings
    static void orderPortsForCrossingMinimization(Node* node);

    // Order ports using barycenter heuristic
    static void orderPortsByBarycenter(
        Node* node,
        PortSide side,
        const std::vector<Port*>& ports);

private:
    // Calculate barycenter position for a port
    static double calculateBarycenter(Port* port);

    // Get available length for port placement on a side
    static double getAvailableLength(Node* node, PortSide side);

    // Calculate position along node edge
    static Point calculatePortPosition(
        Node* node,
        PortSide side,
        double offset);
};

// ============================================================================
// Port Side Assignment
// ============================================================================

class PortSideAssigner {
public:
    // Assign ports to sides based on edge directions
    static void assignBasedOnEdgeDirection(Node* node);

    // Assign ports to minimize total edge length
    static void assignToMinimizeEdgeLength(Node* node);

    // Assign ports evenly across sides
    static void distributeEvenly(Node* node);

private:
    // Determine best side for a port based on connected nodes
    static PortSide determineBestSide(Port* port, Node* node);

    // Calculate edge direction from node center to target
    static Direction getEdgeDirection(Node* from, Node* to);
};

// ============================================================================
// Helper Functions
// ============================================================================

// Get all ports on a specific side
inline std::vector<Port*> getPortsOnSide(Node* node, PortSide side) {
    std::vector<Port*> result;
    for (auto& port : node->ports) {
        if (port->side == side) {
            result.push_back(port.get());
        }
    }
    return result;
}

// Get port index within its side
inline int getPortIndexOnSide(Port* port) {
    if (!port || !port->parent) return -1;

    std::vector<Port*> portsOnSide = getPortsOnSide(port->parent, port->side);
    auto it = std::find(portsOnSide.begin(), portsOnSide.end(), port);
    return (it != portsOnSide.end()) ? static_cast<int>(it - portsOnSide.begin()) : -1;
}

// Sort ports by their position along the node edge
inline void sortPortsByPosition(std::vector<Port*>& ports, PortSide side) {
    std::sort(ports.begin(), ports.end(), [side](Port* a, Port* b) {
        if (side == PortSide::NORTH || side == PortSide::SOUTH) {
            return a->position.x < b->position.x;
        } else {
            return a->position.y < b->position.y;
        }
    });
}

// Convert side to direction
inline Direction sideToDirection(PortSide side) {
    switch (side) {
        case PortSide::NORTH: return Direction::UP;
        case PortSide::SOUTH: return Direction::DOWN;
        case PortSide::EAST:  return Direction::RIGHT;
        case PortSide::WEST:  return Direction::LEFT;
        default: return Direction::UNDEFINED;
    }
}

// Convert direction to port side
inline PortSide directionToSide(Direction dir) {
    switch (dir) {
        case Direction::UP:    return PortSide::NORTH;
        case Direction::DOWN:  return PortSide::SOUTH;
        case Direction::RIGHT: return PortSide::EAST;
        case Direction::LEFT:  return PortSide::WEST;
        default: return PortSide::UNDEFINED;
    }
}

} // namespace ports
} // namespace elk
