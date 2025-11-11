// Eclipse Layout Kernel - C++ Port
// West to east routing strategy implementation
// SPDX-License-Identifier: EPL-2.0

#include "../../../../include/elk/alg/layered/p5edges/west_to_east_routing_strategy.h"
#include <cmath>

namespace elk {
namespace layered {
namespace p5edges {

// Forward declaration of tolerance constant (defined in OrthogonalRoutingGenerator)
static constexpr double ORTHOGONAL_ROUTING_TOLERANCE = 1e-3;

double WestToEastRoutingStrategy::getPortPositionOnHyperNode(LPort* port) {
    return port->getNode()->getPosition().y + port->getPosition().y + port->getAnchor().y;
}

PortSide WestToEastRoutingStrategy::getSourcePortSide() {
    return PortSide::EAST;
}

PortSide WestToEastRoutingStrategy::getTargetPortSide() {
    return PortSide::WEST;
}

void WestToEastRoutingStrategy::calculateBendPoints(HyperEdgeSegment* segment, double startPos, double edgeSpacing) {
    // We don't do anything with dummy segments; they are dealt with when their partner is processed
    if (segment->isDummy()) {
        return;
    }

    // Calculate coordinates for each port's bend points
    double segmentX = startPos + segment->getRoutingSlot() * edgeSpacing;

    for (LPort* port : segment->getPorts()) {
        double sourceY = port->getAbsoluteAnchor().y;

        for (LEdge* edge : port->getOutgoingEdges()) {
            if (!edge->isSelfLoop()) {
                LPort* target = edge->getTarget();
                double targetY = target->getAbsoluteAnchor().y;

                if (std::abs(sourceY - targetY) > ORTHOGONAL_ROUTING_TOLERANCE) {
                    // We'll update these if we find that the segment was split
                    double currentX = segmentX;
                    HyperEdgeSegment* currentSegment = segment;

                    KVector bend(currentX, sourceY);
                    edge->getBendPoints().push_back(bend);
                    addJunctionPointIfNecessary(edge, currentSegment, bend, true);

                    // If this segment was split, we need two additional bend points
                    HyperEdgeSegment* splitPartner = segment->getSplitPartner();
                    if (splitPartner != nullptr) {
                        double splitY = splitPartner->getIncomingConnectionCoordinates().front();

                        bend = KVector(currentX, splitY);
                        edge->getBendPoints().push_back(bend);
                        addJunctionPointIfNecessary(edge, currentSegment, bend, true);

                        // Advance to the split partner's routing slot
                        currentX = startPos + splitPartner->getRoutingSlot() * edgeSpacing;
                        currentSegment = splitPartner;

                        bend = KVector(currentX, splitY);
                        edge->getBendPoints().push_back(bend);
                        addJunctionPointIfNecessary(edge, currentSegment, bend, true);
                    }

                    bend = KVector(currentX, targetY);
                    edge->getBendPoints().push_back(bend);
                    addJunctionPointIfNecessary(edge, currentSegment, bend, true);
                }
            }
        }
    }
}

} // namespace p5edges
} // namespace layered
} // namespace elk
