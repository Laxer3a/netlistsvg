// Eclipse Layout Kernel - C++ Port
// Hyperedge cycle detector
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "hyper_edge_segment.h"
#include "hyper_edge_segment_dependency.h"
#include <vector>
#include <list>
#include <random>

namespace elk {
namespace layered {
namespace p5edges {

/**
 * Finds a set of dependencies to remove or reverse to break cycles in the conflict graph
 * of HyperEdgeSegments and HyperEdgeSegmentDependencies. The hope is to return the
 * minimum-weight set of dependencies such that their reversal or removal causes the graph
 * to become acyclic.
 *
 * Inspired by:
 *   Eades, Lin, Smyth. A fast and effective heuristic for the feedback arc set problem.
 *   In Information Processing Letters, 1993.
 */
class HyperEdgeCycleDetector {
public:
    /**
     * Finds a set of dependencies whose reversal or removal will make the graph acyclic.
     * The method can concentrate on critical dependencies only, or detect cycles among all
     * dependencies. The latter assumes that there are no cycles among critical dependencies
     * left and will never return a critical dependency in the result.
     *
     * @param segments list of hyper edge segments.
     * @param criticalOnly true if we should only detect cycles among critical dependencies.
     * @param random random number generator.
     * @return list of dependencies whose removal or reversal will make the graph acyclic.
     */
    static std::vector<HyperEdgeSegmentDependency*> detectCycles(
        const std::vector<HyperEdgeSegment*>& segments,
        bool criticalOnly,
        std::mt19937& random);

private:
    /**
     * No instances allowed - static utility class.
     */
    HyperEdgeCycleDetector() = delete;

    /**
     * Initializes the mark, in weight and out weight of each hyper edge segment. Also adds
     * all sources (segments without incoming weight 0) and sinks (segments with outgoing
     * weight 0) to their respective lists. Once this is complete, all segments are marked
     * from -1 to -segments.size().
     */
    static void initialize(
        const std::vector<HyperEdgeSegment*>& segments,
        std::list<HyperEdgeSegment*>& sources,
        std::list<HyperEdgeSegment*>& sinks,
        bool criticalOnly);

    /**
     * Computes marks for all segments based on a linear ordering of the segments. Marks
     * will be mutually different, and sinks will have higher marks than sources.
     */
    static void computeLinearOrderingMarks(
        const std::vector<HyperEdgeSegment*>& segments,
        std::list<HyperEdgeSegment*>& sources,
        std::list<HyperEdgeSegment*>& sinks,
        bool criticalOnly,
        std::mt19937& random);

    /**
     * Updates in-weight and out-weight values of the neighbors of the given node, simulating
     * its removal from the graph. The sources and sinks lists are also updated.
     *
     * @param node node for which neighbors are updated
     * @param sources list of sources
     * @param sinks list of sinks
     * @param criticalOnly true if only critical dependencies should be taken into account.
     */
    static void updateNeighbors(
        HyperEdgeSegment* node,
        std::list<HyperEdgeSegment*>& sources,
        std::list<HyperEdgeSegment*>& sinks,
        bool criticalOnly);
};

} // namespace p5edges
} // namespace layered
} // namespace elk
