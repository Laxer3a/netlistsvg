// Eclipse Layout Kernel - C++ Port
// Hyperedge segment splitter
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "hyper_edge_segment.h"
#include "hyper_edge_segment_dependency.h"
#include <vector>
#include <set>

namespace elk {
namespace layered {
namespace p5edges {

// Forward declaration
class OrthogonalRoutingGenerator;

/**
 * Responsible for splitting HyperEdgeSegments in order to avoid overlaps. This class assumes
 * that critical dependency cycles have been found and that a set of critical dependencies
 * whose removal will break those cycles has been computed. These dependencies will determine
 * the HyperEdgeSegments that will be split.
 *
 * All throughout this code, the terminology will refer to the horizontal layout case.
 */
class HyperEdgeSegmentSplitter {
public:
    /**
     * Creates a new instance to be used by the given routing generator.
     */
    explicit HyperEdgeSegmentSplitter(OrthogonalRoutingGenerator* routingGenerator);

    /**
     * Breaks critical dependency cycles by resolving the given dependencies. For each dependency,
     * we split one of the involved HyperEdgeSegments, which will result in modified dependencies
     * and additional segments, which are added to the given list of segments.
     */
    void splitSegments(const std::vector<HyperEdgeSegmentDependency*>& dependenciesToResolve,
                       std::vector<HyperEdgeSegment*>& segments,
                       double criticalConflictThreshold);

private:
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Helper Classes

    /**
     * Represents a free area between two horizontal edge segments, which can be used to route
     * new horizontal segments that connect the two parts of a split HyperEdgeSegment.
     */
    struct FreeArea {
        /** Start of the free area. */
        double startPosition;
        /** End of the free area. */
        double endPosition;
        /** Convenience variable that stores the size of the area. */
        double size;

        FreeArea(double start, double end)
            : startPosition(start), endPosition(end), size(end - start) {}
    };

    /**
     * An indication of what would happen if a given segment was connected to its split partner
     * through this area.
     */
    struct AreaRating {
        /** The number of dependencies caused by using this area. */
        int dependencies;
        /** The number of crossings using this area would cause. */
        int crossings;

        AreaRating(int deps, int cross) : dependencies(deps), crossings(cross) {}
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Finding Space

    /**
     * Assembles the areas between horizontal segments that are large enough to allow another
     * horizontal segment to slip in.
     */
    std::vector<FreeArea> findFreeAreas(const std::vector<HyperEdgeSegment*>& segments,
                                        double criticalConflictThreshold);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Split Segment Decisions

    /**
     * Given a list of hyperedge dependencies, this method assembles a list of HyperEdgeSegments
     * which, if split, will cause the critical cycles caused by the dependencies to be broken.
     */
    std::vector<HyperEdgeSegment*> decideWhichSegmentsToSplit(
        const std::vector<HyperEdgeSegmentDependency*>& dependencies);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Actual Splitting

    /**
     * Splits the given segment at the optimal position.
     */
    void split(HyperEdgeSegment* segment,
               std::vector<HyperEdgeSegment*>& segments,
               std::vector<FreeArea>& freeAreas,
               double criticalConflictThreshold);

    /**
     * Adds all necessary dependencies for the given segment and its split partner. The segment
     * was recently split, and neither it nor its split partner have any dependencies beyond
     * the critical ones.
     */
    void updateDependencies(HyperEdgeSegment* segment, const std::vector<HyperEdgeSegment*>& segments);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Split Position Computation

    /**
     * Returns a hopefully good position to split the given segment at, and updates the list of
     * free areas since an area now either disappears or is split into two smaller areas.
     */
    double computePositionToSplitAndUpdateFreeAreas(HyperEdgeSegment* segment,
                                                     std::vector<FreeArea>& freeAreas,
                                                     double criticalConflictThreshold);

    /**
     * Finds the best area to use for splitting the given segment between the given two area
     * indices (both inclusive).
     */
    int chooseBestAreaIndex(HyperEdgeSegment* segment,
                            const std::vector<FreeArea>& freeAreas,
                            int fromIndex,
                            int toIndex);

    /**
     * Rates what would happen if the given split segments were connected through the given area.
     */
    AreaRating rateArea(HyperEdgeSegment* segment,
                        HyperEdgeSegment* splitSegment,
                        HyperEdgeSegment* splitPartner,
                        const FreeArea& area);

    /**
     * Considers both orderings (s1 left of s2, and vice versa) and considers the number of
     * crossings that would ensue. Based on the crossings, determines the minimum number of
     * dependencies and crossings we'd have to expect.
     */
    void updateConsideringBothOrderings(AreaRating& rating,
                                        HyperEdgeSegment* s1,
                                        HyperEdgeSegment* s2);

    /**
     * Counts the number of crossings that would ensue between a left and a right segment.
     */
    int countCrossingsForSingleOrdering(HyperEdgeSegment* left, HyperEdgeSegment* right);

    /**
     * Determines whether the given area with the given rating is better than another.
     */
    bool isBetter(const FreeArea& currArea, const AreaRating& currRating,
                  const FreeArea& bestArea, const AreaRating& bestRating);

    /**
     * When the given area is being used, it falls into two parts which may be usable themselves.
     * This method does the necessary bookkeeping.
     */
    void useArea(std::vector<FreeArea>& freeAreas, int usedAreaIndex, double criticalConflictThreshold);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Utility Methods

    static double center(HyperEdgeSegment* s);
    static double center(const FreeArea& a);
    static double center(double p1, double p2);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties

    /** The routing generator that created us. */
    OrthogonalRoutingGenerator* routingGenerator_;
};

} // namespace p5edges
} // namespace layered
} // namespace elk
