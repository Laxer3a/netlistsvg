// Eclipse Layout Kernel - C++ Port
// Base routing direction strategy implementation
// SPDX-License-Identifier: EPL-2.0

#include "../../../../include/elk/alg/layered/p5edges/base_routing_direction_strategy.h"
#include "../../../../include/elk/alg/layered/p5edges/west_to_east_routing_strategy.h"
#include <stdexcept>
#include <cmath>

namespace elk {
namespace layered {
namespace p5edges {

// Forward declaration of tolerance constant (defined in OrthogonalRoutingGenerator)
// We use a local constant here to avoid circular dependency
static constexpr double ORTHOGONAL_ROUTING_TOLERANCE = 1e-3;

BaseRoutingDirectionStrategy* BaseRoutingDirectionStrategy::forRoutingDirection(RoutingDirection direction) {
    switch (direction) {
    case RoutingDirection::WEST_TO_EAST:
        return new WestToEastRoutingStrategy();
    case RoutingDirection::NORTH_TO_SOUTH:
        // TODO: Implement when needed
        throw std::invalid_argument("NORTH_TO_SOUTH routing not yet implemented");
    case RoutingDirection::SOUTH_TO_NORTH:
        // TODO: Implement when needed
        throw std::invalid_argument("SOUTH_TO_NORTH routing not yet implemented");
    default:
        throw std::invalid_argument("Invalid routing direction");
    }
}

void BaseRoutingDirectionStrategy::addJunctionPointIfNecessary(
        LEdge* edge,
        HyperEdgeSegment* segment,
        const KVector& pos,
        bool vertical) {

    double p = vertical ? pos.y : pos.x;

    // If we already have this junction point, don't bother
    if (createdJunctionPoints.find(pos) != createdJunctionPoints.end()) {
        return;
    }

    // Whether the point lies somewhere inside the edge segment (without boundaries)
    bool pointInsideEdgeSegment = p > segment->getStartCoordinate() && p < segment->getEndCoordinate();

    // Check if the point lies somewhere at the segment's boundary
    bool pointAtSegmentBoundary = false;
    if (!segment->getIncomingConnectionCoordinates().empty() &&
        !segment->getOutgoingConnectionCoordinates().empty()) {

        // Is the bend point at the start and joins another edge at the same position?
        pointAtSegmentBoundary |=
            std::abs(p - segment->getIncomingConnectionCoordinates().front()) < ORTHOGONAL_ROUTING_TOLERANCE &&
            std::abs(p - segment->getOutgoingConnectionCoordinates().front()) < ORTHOGONAL_ROUTING_TOLERANCE;

        // Is the bend point at the end and joins another edge at the same position?
        pointAtSegmentBoundary |=
            std::abs(p - segment->getIncomingConnectionCoordinates().back()) < ORTHOGONAL_ROUTING_TOLERANCE &&
            std::abs(p - segment->getOutgoingConnectionCoordinates().back()) < ORTHOGONAL_ROUTING_TOLERANCE;
    }

    if (pointInsideEdgeSegment || pointAtSegmentBoundary) {
        // Create a new junction point for the edge at the bend point's position
        auto& junctionPoints = edge->junctionPoints;

        KVector jpoint = pos;
        junctionPoints.push_back(jpoint);
        createdJunctionPoints.insert(jpoint);
    }
}

} // namespace p5edges
} // namespace layered
} // namespace elk
