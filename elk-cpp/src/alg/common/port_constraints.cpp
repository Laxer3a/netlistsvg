// Eclipse Layout Kernel - C++ Port
// Port constraints implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/alg/common/port_constraints.h"
#include <algorithm>
#include <cmath>

namespace elk {
namespace ports {

// ============================================================================
// PortPlacer Implementation
// ============================================================================

void PortPlacer::placePorts(const PlacementContext& ctx) {
    if (!ctx.node) return;

    // First assign ports to sides if needed
    if (ctx.strategy != PortPlacementStrategy::SIMPLE) {
        assignPortSides(ctx.node, ctx.constraints);
    }

    // Place ports on each side
    for (PortSide side : {PortSide::NORTH, PortSide::SOUTH, PortSide::EAST, PortSide::WEST}) {
        auto portsOnSide = getPortsOnSide(ctx.node, side);
        if (portsOnSide.empty()) continue;

        // Check for order constraints
        bool hasOrderConstraint = false;
        if (ctx.constraints) {
            auto constraints = ctx.constraints->getOrderConstraints(side);
            if (!constraints.empty()) {
                portsOnSide = constraints[0].orderedPorts;
                hasOrderConstraint = true;
            }
        }

        // Apply ordering strategy if no constraints
        if (!hasOrderConstraint) {
            if (ctx.strategy == PortPlacementStrategy::BARYCENTER) {
                orderPortsByBarycenter(ctx.node, side, portsOnSide);
            } else if (ctx.strategy == PortPlacementStrategy::MINIMIZE_CROSSINGS) {
                orderPortsForCrossingMinimization(ctx.node);
            }
        }

        // Place ports along the side
        placePortsOnSide(ctx.node, side, portsOnSide, ctx.distribution);
    }
}

void PortPlacer::placePortsOnSide(
    Node* node,
    PortSide side,
    const std::vector<Port*>& ports,
    const PortDistribution& dist) {

    if (ports.empty()) return;

    double availableLength = getAvailableLength(node, side);
    double usableLength = availableLength - 2 * dist.margin;

    if (usableLength < 0) usableLength = availableLength;

    double totalPortSize = 0;
    for (Port* port : ports) {
        totalPortSize += (side == PortSide::NORTH || side == PortSide::SOUTH) ?
                         port->size.width : port->size.height;
    }

    double spacing = dist.spacing;
    if (dist.equalSpacing && ports.size() > 1) {
        double availableForSpacing = usableLength - totalPortSize;
        spacing = availableForSpacing / (ports.size() + 1);
        if (spacing < 0) spacing = 0;
    }

    // Calculate starting offset
    double offset = dist.margin;
    if (dist.centerUnused && dist.equalSpacing) {
        offset = (availableLength - totalPortSize - spacing * (ports.size() - 1)) / 2;
        if (offset < dist.margin) offset = dist.margin;
    }

    // Place each port
    for (size_t i = 0; i < ports.size(); ++i) {
        Port* port = ports[i];

        port->position = calculatePortPosition(node, side, offset);
        port->side = side;

        double portSize = (side == PortSide::NORTH || side == PortSide::SOUTH) ?
                         port->size.width : port->size.height;
        offset += portSize + spacing;
    }
}

void PortPlacer::assignPortSides(Node* node, const PortConstraintManager* constraints) {
    if (!node) return;

    for (auto& port : node->ports) {
        // Check if port has fixed side constraint
        if (constraints) {
            PortConstraint pc = constraints->getPortConstraint(port.get());
            if (pc == PortConstraint::FIXED_SIDE ||
                pc == PortConstraint::FIXED_ORDER ||
                pc == PortConstraint::FIXED_POSITION) {
                continue;  // Side already fixed
            }
        }

        // Assign based on connectivity
        PortSideAssigner::assignBasedOnEdgeDirection(node);
    }
}

void PortPlacer::orderPortsForCrossingMinimization(Node* node) {
    // Simple crossing minimization: sort ports by barycenter
    for (PortSide side : {PortSide::NORTH, PortSide::SOUTH, PortSide::EAST, PortSide::WEST}) {
        auto portsOnSide = getPortsOnSide(node, side);
        orderPortsByBarycenter(node, side, portsOnSide);
    }
}

void PortPlacer::orderPortsByBarycenter(
    Node* node,
    PortSide side,
    const std::vector<Port*>& ports) {

    std::vector<std::pair<Port*, double>> portBarycenters;

    for (Port* port : ports) {
        double barycenter = calculateBarycenter(port);
        portBarycenters.push_back({port, barycenter});
    }

    // Sort by barycenter
    std::sort(portBarycenters.begin(), portBarycenters.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });

    // Update port indices
    for (size_t i = 0; i < portBarycenters.size(); ++i) {
        portBarycenters[i].first->index = static_cast<int>(i);
    }
}

double PortPlacer::calculateBarycenter(Port* port) {
    if (!port || !port->parent) return 0.0;

    double sum = 0.0;
    int count = 0;

    Node* parent = port->parent;

    // Find edges connected to this port
    for (const auto& edge : parent->edges) {
        bool isSource = std::find(edge->sourcePorts.begin(), edge->sourcePorts.end(), port)
                       != edge->sourcePorts.end();
        bool isTarget = std::find(edge->targetPorts.begin(), edge->targetPorts.end(), port)
                       != edge->targetPorts.end();

        if (isSource || isTarget) {
            // Get connected node position
            Node* connectedNode = nullptr;
            if (isSource && !edge->targetNodes.empty()) {
                connectedNode = edge->targetNodes[0];
            } else if (isTarget && !edge->sourceNodes.empty()) {
                connectedNode = edge->sourceNodes[0];
            }

            if (connectedNode) {
                Point connectedPos = connectedNode->getAbsolutePosition();
                Point parentPos = parent->getAbsolutePosition();

                // Use appropriate coordinate based on port side
                if (port->side == PortSide::NORTH || port->side == PortSide::SOUTH) {
                    sum += connectedPos.x - parentPos.x;
                } else {
                    sum += connectedPos.y - parentPos.y;
                }
                count++;
            }
        }
    }

    return (count > 0) ? sum / count : 0.0;
}

double PortPlacer::getAvailableLength(Node* node, PortSide side) {
    if (!node) return 0.0;

    switch (side) {
        case PortSide::NORTH:
        case PortSide::SOUTH:
            return node->size.width;
        case PortSide::EAST:
        case PortSide::WEST:
            return node->size.height;
        default:
            return 0.0;
    }
}

Point PortPlacer::calculatePortPosition(Node* node, PortSide side, double offset) {
    Point pos;

    switch (side) {
        case PortSide::NORTH:
            pos.x = offset;
            pos.y = 0;
            break;
        case PortSide::SOUTH:
            pos.x = offset;
            pos.y = node->size.height;
            break;
        case PortSide::EAST:
            pos.x = node->size.width;
            pos.y = offset;
            break;
        case PortSide::WEST:
            pos.x = 0;
            pos.y = offset;
            break;
        default:
            pos.x = offset;
            pos.y = 0;
            break;
    }

    return pos;
}

// ============================================================================
// PortSideAssigner Implementation
// ============================================================================

void PortSideAssigner::assignBasedOnEdgeDirection(Node* node) {
    if (!node) return;

    for (auto& port : node->ports) {
        if (port->side != PortSide::UNDEFINED) continue;

        port->side = determineBestSide(port.get(), node);
    }
}

void PortSideAssigner::assignToMinimizeEdgeLength(Node* node) {
    if (!node) return;

    for (auto& port : node->ports) {
        PortSide bestSide = PortSide::EAST;
        double minLength = std::numeric_limits<double>::max();

        // Try each side and measure total edge length
        for (PortSide side : {PortSide::NORTH, PortSide::SOUTH, PortSide::EAST, PortSide::WEST}) {
            port->side = side;
            Point portPos = port->getAbsolutePosition();

            double totalLength = 0.0;

            // Calculate total length to all connected nodes
            for (const auto& edge : node->edges) {
                bool connected = std::find(edge->sourcePorts.begin(), edge->sourcePorts.end(), port.get())
                                != edge->sourcePorts.end() ||
                                std::find(edge->targetPorts.begin(), edge->targetPorts.end(), port.get())
                                != edge->targetPorts.end();

                if (connected) {
                    // Get connected node
                    Node* target = nullptr;
                    if (!edge->targetNodes.empty()) target = edge->targetNodes[0];
                    else if (!edge->sourceNodes.empty()) target = edge->sourceNodes[0];

                    if (target) {
                        Point targetPos = target->getAbsolutePosition();
                        totalLength += (portPos - targetPos).length();
                    }
                }
            }

            if (totalLength < minLength) {
                minLength = totalLength;
                bestSide = side;
            }
        }

        port->side = bestSide;
    }
}

void PortSideAssigner::distributeEvenly(Node* node) {
    if (!node || node->ports.empty()) return;

    int portsPerSide = (node->ports.size() + 3) / 4;  // Round up
    std::vector<PortSide> sides = {PortSide::NORTH, PortSide::EAST,
                                   PortSide::SOUTH, PortSide::WEST};

    size_t sideIndex = 0;
    int countOnSide = 0;

    for (auto& port : node->ports) {
        port->side = sides[sideIndex];
        countOnSide++;

        if (countOnSide >= portsPerSide && sideIndex < sides.size() - 1) {
            sideIndex++;
            countOnSide = 0;
        }
    }
}

PortSide PortSideAssigner::determineBestSide(Port* port, Node* node) {
    if (!port || !node) return PortSide::EAST;

    // Find connected nodes and determine dominant direction
    Point nodeCenter = node->getAbsolutePosition();
    nodeCenter.x += node->size.width / 2;
    nodeCenter.y += node->size.height / 2;

    Point avgDirection(0, 0);
    int count = 0;

    for (const auto& edge : node->edges) {
        bool isConnected = std::find(edge->sourcePorts.begin(), edge->sourcePorts.end(), port)
                          != edge->sourcePorts.end() ||
                          std::find(edge->targetPorts.begin(), edge->targetPorts.end(), port)
                          != edge->targetPorts.end();

        if (isConnected) {
            Node* target = nullptr;
            if (!edge->targetNodes.empty()) target = edge->targetNodes[0];
            else if (!edge->sourceNodes.empty()) target = edge->sourceNodes[0];

            if (target && target != node) {
                Point targetCenter = target->getAbsolutePosition();
                targetCenter.x += target->size.width / 2;
                targetCenter.y += target->size.height / 2;

                avgDirection += (targetCenter - nodeCenter);
                count++;
            }
        }
    }

    if (count == 0) return PortSide::EAST;

    avgDirection = avgDirection / static_cast<double>(count);

    // Determine side based on direction
    if (std::abs(avgDirection.x) > std::abs(avgDirection.y)) {
        return (avgDirection.x > 0) ? PortSide::EAST : PortSide::WEST;
    } else {
        return (avgDirection.y > 0) ? PortSide::SOUTH : PortSide::NORTH;
    }
}

} // namespace ports
} // namespace elk
