// Eclipse Layout Kernel - C++ Port
// West to east routing strategy
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "base_routing_direction_strategy.h"

namespace elk {
namespace layered {
namespace p5edges {

/**
 * Routing strategy for routing layers from west to east (left to right).
 */
class WestToEastRoutingStrategy : public BaseRoutingDirectionStrategy {
public:
    double getPortPositionOnHyperNode(LPort* port) override;
    PortSide getSourcePortSide() override;
    PortSide getTargetPortSide() override;
    void calculateBendPoints(HyperEdgeSegment* segment, double startPos, double edgeSpacing) override;
};

} // namespace p5edges
} // namespace layered
} // namespace elk
