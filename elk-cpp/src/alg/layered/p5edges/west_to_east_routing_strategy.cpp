// Eclipse Layout Kernel - C++ Port
// West to east routing strategy implementation
// SPDX-License-Identifier: EPL-2.0

#include "../../../../include/elk/alg/layered/p5edges/west_to_east_routing_strategy.h"
#include <cmath>
#include <iostream>

namespace elk {
namespace layered {
namespace p5edges {

// Forward declaration of tolerance constant (defined in OrthogonalRoutingGenerator)
static constexpr double ORTHOGONAL_ROUTING_TOLERANCE = 1e-3;

double WestToEastRoutingStrategy::getPortPositionOnHyperNode(LPort* port) {
    double nodeY = port->getNode()->getPosition().y;
    double portY = port->getPosition().y;
    double anchorY = port->getAnchor().y;
    double total = nodeY + portY + anchorY;
    std::cerr << "      getPortPositionOnHyperNode: node.y=" << nodeY << " port.y=" << portY << " anchor.y=" << anchorY << " total=" << total << "\n";
    return total;
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

    std::cerr << "    calculateBendPoints: startPos=" << startPos << " slot=" << segment->getRoutingSlot() << " segmentX=" << segmentX << "\n";
    std::cerr << "    calculateBendPoints: segment has " << segment->getPorts().size() << " ports\n";
    for (LPort* port : segment->getPorts()) {
        double sourceY = port->getAbsoluteAnchor().y;

        std::cerr << "      Port has " << port->getOutgoingEdges().size() << " outgoing edges\n";
        for (LEdge* edge : port->getOutgoingEdges()) {
            std::string edgeId = edge->originalEdge ? edge->originalEdge->id : "unknown";
            std::cerr << "        Processing edge " << edgeId << "\n";
            if (!edge->isSelfLoop()) {
                LPort* target = edge->getTarget();
                double targetY = target->getAbsoluteAnchor().y;

                std::cerr << "          sourceY=" << sourceY << " targetY=" << targetY << "\n";
                if (std::abs(sourceY - targetY) > ORTHOGONAL_ROUTING_TOLERANCE) {
                    // Skip if this edge already has bend points from a DIFFERENT segment
                    // Check: if edge has bend points, verify they match this segment's X position
                    if (!edge->getBendPoints().empty()) {
                        double existingX = edge->getBendPoints()[0].x;
                        if (std::abs(existingX - segmentX) > ORTHOGONAL_ROUTING_TOLERANCE) {
                            std::cerr << "          Edge " << edgeId << " has bend points at X=" << existingX
                                      << " but this segment is at X=" << segmentX << " - REPLACING with correct position\n";
                            edge->getBendPoints().clear();
                        } else {
                            std::cerr << "          Edge " << edgeId << " already has correct bend points at X=" << existingX
                                      << ", skipping\n";
                            continue;
                        }
                    }

                    std::cerr << "          Adding bend points to edge " << edgeId << ", current bendPoints=" << edge->getBendPoints().size() << "\n";
                    // We'll update these if we find that the segment was split
                    double currentX = segmentX;
                    HyperEdgeSegment* currentSegment = segment;

                    Point bend(currentX, sourceY);
                    edge->getBendPoints().push_back(bend);
                    std::cerr << "          Bend[0]: (" << bend.x << ", " << bend.y << ")\n";
                    addJunctionPointIfNecessary(edge, currentSegment, bend, true);

                    // If this segment was split, we need two additional bend points
                    HyperEdgeSegment* splitPartner = segment->getSplitPartner();
                    if (splitPartner != nullptr) {
                        std::cerr << "          *** SPLIT SEGMENT for edge " << edgeId << " ***\n";
                        std::cerr << "          Partner slot=" << splitPartner->getRoutingSlot() << "\n";
                        double splitY = splitPartner->getIncomingConnectionCoordinates().front();

                        bend = Point(currentX, splitY);
                        edge->getBendPoints().push_back(bend);
                        std::cerr << "          Bend[1]: (" << bend.x << ", " << bend.y << ") - vertical to splitY\n";
                        addJunctionPointIfNecessary(edge, currentSegment, bend, true);

                        // Advance to the split partner's routing slot
                        double prevX = currentX;
                        currentX = startPos + splitPartner->getRoutingSlot() * edgeSpacing;
                        std::cerr << "          Split partner X: startPos=" << startPos << " + slot=" << splitPartner->getRoutingSlot() << " * spacing=" << edgeSpacing << " = " << currentX << "\n";
                        currentSegment = splitPartner;

                        bend = Point(currentX, splitY);
                        edge->getBendPoints().push_back(bend);
                        std::cerr << "          Bend[2]: (" << bend.x << ", " << bend.y << ") - horizontal to partner slot\n";
                        addJunctionPointIfNecessary(edge, currentSegment, bend, true);
                    }

                    bend = Point(currentX, targetY);
                    edge->getBendPoints().push_back(bend);
                    std::cerr << "          Bend[final]: (" << bend.x << ", " << bend.y << ") - vertical to target\n";
                    addJunctionPointIfNecessary(edge, currentSegment, bend, true);
                }
            }
        }
    }
}

} // namespace p5edges
} // namespace layered
} // namespace elk
