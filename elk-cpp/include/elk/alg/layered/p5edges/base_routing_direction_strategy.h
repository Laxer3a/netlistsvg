// Eclipse Layout Kernel - C++ Port
// Base routing direction strategy
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../lgraph.h"
#include "routing_direction.h"
#include "hyper_edge_segment.h"
#include <set>
#include <memory>

namespace elk {
namespace layered {
namespace p5edges {

// Forward declaration
class HyperEdgeSegment;

/**
 * A routing direction strategy adapts the OrthogonalRoutingGenerator to different routing directions.
 * Commonly, edges are routed from a left source layer to a right target layer. However, with northern
 * and southern external ports, this can be different. Routing direction strategies abstract from the
 * actual direction.
 *
 * Use forRoutingDirection() to obtain a routing direction strategy for a given RoutingDirection.
 */
class BaseRoutingDirectionStrategy {
public:
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Creation

    /**
     * Returns an implementation suitable for the given routing direction.
     *
     * @param direction the direction to return a routing direction strategy for.
     * @return a suitable strategy.
     */
    static BaseRoutingDirectionStrategy* forRoutingDirection(RoutingDirection direction);

    /**
     * Virtual destructor for proper cleanup of derived classes.
     */
    virtual ~BaseRoutingDirectionStrategy() = default;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Accessors

    /**
     * Returns the set of junction points created so far.
     */
    std::set<KVector>& getCreatedJunctionPoints() { return createdJunctionPoints; }
    const std::set<KVector>& getCreatedJunctionPoints() const { return createdJunctionPoints; }

    /**
     * Removes all junction points created so far.
     */
    void clearCreatedJunctionPoints() { createdJunctionPoints.clear(); }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // To be implemented by subclasses

    /**
     * Returns the port's position on a hyper edge axis. In the west-to-east routing case, this would
     * be the port's exact y coordinate.
     *
     * @param port the port.
     * @return the port's coordinate on the hyper edge axis.
     */
    virtual double getPortPositionOnHyperNode(LPort* port) = 0;

    /**
     * Returns the side of ports that should be considered on a source layer. For a west-to-east routing,
     * this would probably be the eastern ports of each western layer.
     *
     * @return the side of ports to be considered in the source layer.
     */
    virtual PortSide getSourcePortSide() = 0;

    /**
     * Returns the side of ports that should be considered on a target layer. For a west-to-east routing,
     * this would probably be the western ports of each eastern layer.
     *
     * @return the side of ports to be considered in the target layer.
     */
    virtual PortSide getTargetPortSide() = 0;

    /**
     * Calculates and assigns bend points for edges incident to the ports belonging to the given hyper edge.
     *
     * @param hyperNode the hyper edge.
     * @param startPos the position of the trunk of the first hyper edge between the layers. This position,
     *                 together with the current hyper node's rank allows the calculation of the hyper node's
     *                 trunk's position.
     * @param edgeSpacing spacing between adjacent edges. This is used to turn an edge segment's routing slot
     *                    into a proper position.
     */
    virtual void calculateBendPoints(HyperEdgeSegment* hyperNode, double startPos, double edgeSpacing) = 0;

protected:
    /**
     * Add a junction point to the given edge if necessary. It is necessary to add a junction point if the
     * bend point is not at one of the two end positions of the hyperedge segment.
     *
     * @param edge an edge
     * @param segment the corresponding hyperedge segment
     * @param pos the bend point position
     * @param vertical true if the connecting segment is vertical, false if it is horizontal
     */
    void addJunctionPointIfNecessary(LEdge* edge, HyperEdgeSegment* segment, const KVector& pos, bool vertical);

private:
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties

    /** Set of already created junction points, to avoid multiple points at the same position. */
    std::set<KVector> createdJunctionPoints;
};

} // namespace p5edges
} // namespace layered
} // namespace elk
