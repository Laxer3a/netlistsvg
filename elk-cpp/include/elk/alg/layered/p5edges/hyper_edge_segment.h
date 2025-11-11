// Eclipse Layout Kernel - C++ Port
// Hyperedge segment - represents the "trunk" of a hyper edge
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../lgraph.h"
#include "hyper_edge_segment_dependency.h"
#include <list>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <cmath>

namespace elk {
namespace layered {
namespace p5edges {

// Forward declarations
class BaseRoutingDirectionStrategy;

/**
 * Instances of this class represent the "trunk" of a hyper edge. In left-to-right layouts,
 * this will be the vertical segment of a hyperedge. Such a segment has a list of coordinates
 * for each of its two sides that specifies where incoming or outgoing connections enter and
 * leave the segment, respectively.
 *
 * The range of coordinates spanned by a hyper edge segment determines its extent, which can
 * be requested by calling getStartCoordinate() and getEndCoordinate().
 *
 * Hyperedge segments can be split by the HyperEdgeSegmentSplitter if they are part of a
 * cyclic critical dependency. Splitting an edge segment will cause involved edges to take
 * a longer detour, but will resolve edge overlaps.
 *
 * Instances of this class are comparable based on the value of mark.
 */
class HyperEdgeSegment {
public:
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Initialization

    /**
     * Creates a new instance for the given routing strategy.
     *
     * @param routingStrategy the routing strategy. Only required if addPortPositions() will be called.
     */
    explicit HyperEdgeSegment(BaseRoutingDirectionStrategy* routingStrategy);

    /**
     * Adds the positions of the given port and all connected ports.
     *
     * @param port a port.
     * @param hyperEdgeSegmentMap map of ports to existing hyperedge segments.
     */
    void addPortPositions(LPort* port, std::map<LPort*, HyperEdgeSegment*>& hyperEdgeSegmentMap);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Getters and Setters

    /**
     * Returns the ports incident to this segment.
     */
    std::vector<LPort*>& getPorts() { return ports_; }
    const std::vector<LPort*>& getPorts() const { return ports_; }

    /**
     * Returns this segment's routing slot.
     */
    int getRoutingSlot() const { return routingSlot_; }

    /**
     * Sets this segment's routing slot.
     */
    void setRoutingSlot(int slot) { routingSlot_ = slot; }

    /**
     * Returns the coordinate where this segment begins.
     */
    double getStartCoordinate() const { return startPosition_; }

    /**
     * Returns the coordinate where this segment ends.
     */
    double getEndCoordinate() const { return endPosition_; }

    /**
     * Returns the (sorted) list of coordinates where incoming connections enter this segment.
     */
    std::list<double>& getIncomingConnectionCoordinates() { return incomingConnectionCoordinates_; }
    const std::list<double>& getIncomingConnectionCoordinates() const { return incomingConnectionCoordinates_; }

    /**
     * Returns the (sorted) list of coordinates where outgoing connections leave this segment.
     */
    std::list<double>& getOutgoingConnectionCoordinates() { return outgoingConnectionCoordinates_; }
    const std::list<double>& getOutgoingConnectionCoordinates() const { return outgoingConnectionCoordinates_; }

    /**
     * Return the outgoing dependencies to other hyper edge segments.
     */
    std::vector<HyperEdgeSegmentDependency*>& getOutgoingSegmentDependencies() {
        return outgoingSegmentDependencies_;
    }
    const std::vector<HyperEdgeSegmentDependency*>& getOutgoingSegmentDependencies() const {
        return outgoingSegmentDependencies_;
    }

    /**
     * Returns the combined weight of all outgoing dependencies.
     */
    int getOutWeight() const { return outDepWeight_; }

    /**
     * Sets the combined weight of all outgoing dependencies.
     */
    void setOutWeight(int outWeight) { outDepWeight_ = outWeight; }

    /**
     * Returns the combined weight of critical outgoing dependencies.
     */
    int getCriticalOutWeight() const { return criticalOutDepWeight_; }

    /**
     * Sets the combined weight of critical outgoing dependencies.
     */
    void setCriticalOutWeight(int outWeight) { criticalOutDepWeight_ = outWeight; }

    /**
     * Return the incoming dependencies from other hyper edge segments.
     */
    std::vector<HyperEdgeSegmentDependency*>& getIncomingSegmentDependencies() {
        return incomingSegmentDependencies_;
    }
    const std::vector<HyperEdgeSegmentDependency*>& getIncomingSegmentDependencies() const {
        return incomingSegmentDependencies_;
    }

    /**
     * Returns the weight of incoming dependencies.
     */
    int getInWeight() const { return inDepWeight_; }

    /**
     * Sets the weight of incoming dependencies.
     */
    void setInWeight(int inWeight) { inDepWeight_ = inWeight; }

    /**
     * Returns the combined weight of critical incoming dependencies.
     */
    int getCriticalInWeight() const { return criticalInDepWeight_; }

    /**
     * Sets the combined weight of critical incoming dependencies.
     */
    void setCriticalInWeight(int inWeight) { criticalInDepWeight_ = inWeight; }

    /**
     * Returns the split partner, that is, the other segment involved in splitting a segment
     * into two, or nullptr if this segment was not split.
     */
    HyperEdgeSegment* getSplitPartner() const { return splitPartner_; }

    /**
     * Sets the split partner, that is, the other segment involved in splitting a segment into two.
     */
    void setSplitPartner(HyperEdgeSegment* splitPartner) { splitPartner_ = splitPartner; }

    /**
     * Returns the segment that caused this one to be split, if any. This is only set on one of
     * the split partners.
     */
    HyperEdgeSegment* getSplitBy() const { return splitBy_; }

    /**
     * Sets the segment that caused this one to be split, if any.
     */
    void setSplitBy(HyperEdgeSegment* splitBy) { splitBy_ = splitBy; }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Utilities

    /**
     * Convenience method which returns the size of this segment, which is its end coordinate
     * minus its start coordinate.
     */
    double getLength() const { return getEndCoordinate() - getStartCoordinate(); }

    /**
     * Checks whether this segment connects two or more ports.
     */
    bool representsHyperedge() const {
        return getIncomingConnectionCoordinates().size() + getOutgoingConnectionCoordinates().size() > 2;
    }

    /**
     * Checks whether this segment was introduced while splitting another segment.
     */
    bool isDummy() const { return splitPartner_ != nullptr && splitBy_ == nullptr; }

    /**
     * Recomputes the start and end coordinate based on incoming and outgoing connection coordinates.
     */
    void recomputeExtent();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Splitting

    /**
     * Simulates what would happen during a split. The returned pair contains two new HyperEdgeSegments:
     * the first simulates what would happen to this instance, and the second simulates the split partner.
     */
    std::pair<HyperEdgeSegment*, HyperEdgeSegment*> simulateSplit();

    /**
     * Splits this segment into two and returns the new segment. The segments will be linked at the
     * given position. This segment will retain all incoming connection coordinates, but all of its
     * outgoing connection coordinates will move over to the new segment. This will completely clear
     * all dependencies since they may have become obsolete.
     *
     * @param splitPosition position to split the two segments.
     * @return the new segment for convenience.
     */
    HyperEdgeSegment* splitAt(double splitPosition);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Comparable and Object Methods

    /**
     * Compare based on mark value.
     */
    bool operator<(const HyperEdgeSegment& other) const { return mark < other.mark; }
    bool operator==(const HyperEdgeSegment& other) const { return mark == other.mark; }

    /**
     * Hash based on mark value.
     */
    size_t hash() const { return static_cast<size_t>(mark); }

    /**
     * String representation.
     */
    std::string toString() const;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Public Fields

    /** Mark value used for cycle breaking (to be accessed directly). */
    int mark = 0;

private:
    /**
     * Inserts a value into a sorted list, maintaining sorted order.
     */
    static void insertSorted(std::list<double>& list, double value);

    /**
     * Recomputes extent based on a list of positions.
     */
    void recomputeExtent(const std::list<double>& positions);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties

    /** Routing strategy which will ultimately decide how edges will be routed. */
    BaseRoutingDirectionStrategy* routingStrategy_;

    /** Ports represented by this hypernode. */
    std::vector<LPort*> ports_;

    /** The routing slot determines the horizontal distance to the preceding layer. */
    int routingSlot_ = 0;

    /** Start position of this edge segment (in horizontal layouts, this is the topmost y coordinate). */
    double startPosition_ = NAN;
    /** End position of this edge segment (in horizontal layouts, this is the bottommost y coordinate). */
    double endPosition_ = NAN;

    /** Sorted list of coordinates where incoming connections enter this segment. */
    std::list<double> incomingConnectionCoordinates_;
    /** Sorted list of coordinates where outgoing connections leave this segment. */
    std::list<double> outgoingConnectionCoordinates_;

    /** List of outgoing dependencies to other edge segments. */
    std::vector<HyperEdgeSegmentDependency*> outgoingSegmentDependencies_;
    /** Combined weight of all outgoing dependencies. */
    int outDepWeight_ = 0;
    /** Combined weight of critical outgoing dependencies. */
    int criticalOutDepWeight_ = 0;

    /** List of incoming dependencies from other edge segments. */
    std::vector<HyperEdgeSegmentDependency*> incomingSegmentDependencies_;
    /** Combined weight of all incoming dependencies. */
    int inDepWeight_ = 0;
    /** Combined weight of critical incoming dependencies. */
    int criticalInDepWeight_ = 0;

    /** If this segment is the result of a split segment, this is the other segment; otherwise it is nullptr. */
    HyperEdgeSegment* splitPartner_ = nullptr;
    /** The segment that caused this segment to be split, if any (only set on one of the split partners). */
    HyperEdgeSegment* splitBy_ = nullptr;
};

} // namespace p5edges
} // namespace layered
} // namespace elk
