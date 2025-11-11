// Eclipse Layout Kernel - C++ Port
// Orthogonal routing generator
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../lgraph.h"
#include "hyper_edge_segment.h"
#include "hyper_edge_segment_dependency.h"
#include "routing_direction.h"
#include <vector>
#include <map>
#include <random>
#include <string>

namespace elk {
namespace layered {
namespace p5edges {

// Forward declarations
class BaseRoutingDirectionStrategy;
class HyperEdgeSegmentSplitter;

/**
 * Edge routing implementation that creates orthogonal bend points. Inspired by:
 * - Georg Sander. Layout of directed hypergraphs with orthogonal hyperedges. In Proceedings of
 *   the 11th International Symposium on Graph Drawing (GD '03), volume 2912 of LNCS, pp. 381-386.
 *   Springer, 2004.
 * - Giuseppe di Battista, Peter Eades, Roberto Tamassia, Ioannis G. Tollis, Graph Drawing:
 *   Algorithms for the Visualization of Graphs, Prentice Hall, New Jersey, 1999 (Section 9.4,
 *   for cycle breaking in the hyperedge segment graph)
 *
 * This is a generic implementation that can be applied to all four routing directions.
 * Usually, edges will be routed from west to east. However, with northern and southern
 * external ports, this changes: edges are routed from south to north and north to south,
 * respectively.
 *
 * When instantiating a new routing generator, the concrete directional strategy must be
 * specified. Once that is done, routeEdges() is called repeatedly to route edges between
 * given lists of nodes.
 */
class OrthogonalRoutingGenerator {
public:
    ///////////////////////////////////////////////////////////////////////////////
    // Constants

    /** Differences below this tolerance value are treated as zero. */
    static constexpr double TOLERANCE = 1e-3;

    ///////////////////////////////////////////////////////////////////////////////
    // Constructor

    /**
     * Constructs a new instance.
     *
     * @param direction the direction edges should point at.
     * @param edgeSpacing the space between edges.
     * @param debugPrefix prefix of debug output files, or nullptr if no debug output should be generated.
     */
    OrthogonalRoutingGenerator(RoutingDirection direction, double edgeSpacing, const std::string& debugPrefix = "");

    /**
     * Destructor to clean up routing strategy.
     */
    ~OrthogonalRoutingGenerator();

    ///////////////////////////////////////////////////////////////////////////////
    // Edge Routing

    /**
     * Route edges between the given layers.
     *
     * @param layeredGraph the layered graph.
     * @param sourceLayerNodes the left layer. May be nullptr.
     * @param sourceLayerIndex the source layer's index. Ignored if there is no source layer.
     * @param targetLayerNodes the right layer. May be nullptr.
     * @param startPos horizontal position of the first routing slot
     * @return the number of routing slots for this layer
     */
    int routeEdges(LGraph* layeredGraph,
                   const std::vector<LNode*>* sourceLayerNodes,
                   int sourceLayerIndex,
                   const std::vector<LNode*>* targetLayerNodes,
                   double startPos);

    ///////////////////////////////////////////////////////////////////////////////
    // Public Static Methods (used by HyperEdgeSegmentSplitter)

    /**
     * Counts the number of crossings for a given list of positions.
     *
     * @param posis sorted list of positions
     * @param start start of the critical area
     * @param end end of the critical area
     * @return number of positions in the critical area
     */
    static int countCrossings(const std::list<double>& posis, double start, double end);

    /**
     * Finds and breaks non-critical cycles by removing and reversing non-critical dependencies.
     * This method is used by the self loop routing code as well.
     */
    static void breakNonCriticalCycles(std::vector<HyperEdgeSegment*>& edgeSegments, std::mt19937& random);

    ///////////////////////////////////////////////////////////////////////////////
    // Package-visible Methods (used by HyperEdgeSegmentSplitter)

    /**
     * Create dependencies between the two given hyperedge segments, if one is needed. This method
     * is used not just here, but also by HyperEdgeSegmentSplitter.
     *
     * @param he1 first hyperedge segments
     * @param he2 second hyperedge segments
     * @return the number of critical dependencies that were added
     */
    int createDependencyIfNecessary(HyperEdgeSegment* he1, HyperEdgeSegment* he2);

private:
    ///////////////////////////////////////////////////////////////////////////////
    // Constants

    /** A special return value used by the conflict counting method. */
    static constexpr int CRITICAL_CONFLICTS_DETECTED = -1;

    /** Factor for edge spacing used to determine the conflictThreshold (determined experimentally). */
    static constexpr double CONFLICT_THRESHOLD_FACTOR = 0.5;
    /** Factor to compute criticalConflictThreshold (determined experimentally). */
    static constexpr double CRITICAL_CONFLICT_THRESHOLD_FACTOR = 0.2;

    /** Weight penalty for (non-critical) conflicts. */
    static constexpr int CONFLICT_PENALTY = 1;
    /** Weight penalty for crossings. */
    static constexpr int CROSSING_PENALTY = 16;

    ///////////////////////////////////////////////////////////////////////////////
    // Helper Methods

    /**
     * Computes and returns the minimum distance between any two adjacent source connections and
     * any two adjacent target connections.
     */
    double minimumHorizontalSegmentDistance(const std::vector<HyperEdgeSegment*>& edgeSegments);

    /**
     * Returns the smallest difference between any two numbers in the given vector of numbers.
     * If there are less than two numbers, we return Double::MAX_VALUE.
     */
    static double minimumDifference(std::vector<double>& numbers);

    /**
     * Creates hyperedge segments for the given layer.
     *
     * @param nodes the layer. May be nullptr, in which case nothing happens.
     * @param portSide side of the output ports for whose outgoing edges hyperedge segments should be created.
     * @param hyperEdges list the created hyperedge segments should be added to.
     * @param portToHyperEdgeSegmentMap map from ports to hyperedge segments that should be filled.
     */
    void createHyperEdgeSegments(const std::vector<LNode*>* nodes,
                                 PortSide portSide,
                                 std::vector<HyperEdgeSegment*>& hyperEdges,
                                 std::map<LPort*, HyperEdgeSegment*>& portToHyperEdgeSegmentMap);

    /**
     * Counts the number of conflicts for the given lists of positions.
     *
     * @param posis1 sorted list of positions
     * @param posis2 sorted list of positions
     * @return number of positions that overlap, or CRITICAL_CONFLICTS_DETECTED if a critical conflict was detected.
     */
    int countConflicts(const std::list<double>& posis1, const std::list<double>& posis2);

    /**
     * Finds and breaks critical cycles to avoid edge overlaps. The critical cycles we find will
     * be handled by splitting an edge segment into two.
     */
    void breakCriticalCycles(std::vector<HyperEdgeSegment*>& edgeSegments, std::mt19937& random);

    /**
     * Perform a topological numbering of the given hyperedge segments.
     *
     * @param segments list of hyperedge segments
     */
    static void topologicalNumbering(std::vector<HyperEdgeSegment*>& segments);

    ///////////////////////////////////////////////////////////////////////////////
    // Member Variables

    /** We'll be using this thing to split hyper edge segments, if necessary. */
    HyperEdgeSegmentSplitter* segmentSplitter_;

    /** Routing direction strategy. */
    BaseRoutingDirectionStrategy* routingStrategy_;

    /** Spacing between edges. */
    double edgeSpacing_;

    /**
     * Threshold at which horizontal line segments are considered to be too close to one another.
     * A conflict will cause the involved hyperedges to be assigned to different routing slots
     * in order to resolve the conflict.
     *
     * This value is a constant fraction of the edge spacing.
     */
    double conflictThreshold_;

    /**
     * Threshold at which horizontal line segments are considered to overlap. A critical conflict
     * will cause the involved hyperedges to be assigned to different routing slots in order to
     * resolve the critical conflict. If there is a cycle of critical conflicts, there is no
     * assignment that will resolve all overlaps. This will result in one of the involved
     * hyperedges to be split into two vertical segments.
     *
     * This value depends on how close edges from the source layer and edges from the target layer
     * come to each other, respectively. We cannot change that. The critical conflict threshold is
     * thus a fraction of that minimum proximity and is different for each pair of layers. Hence,
     * this is not a constant.
     */
    double criticalConflictThreshold_;

    /** Prefix of debug output files. */
    std::string debugPrefix_;
};

} // namespace p5edges
} // namespace layered
} // namespace elk
