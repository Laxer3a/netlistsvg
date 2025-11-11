// Eclipse Layout Kernel - C++ Port
// Orthogonal edge router - Phase 5
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../lgraph.h"

namespace elk {
namespace layered {
namespace p5edges {

/**
 * Edge routing implementation that creates orthogonal bend points. Inspired by:
 * - Georg Sander, Layout of directed hypergraphs with orthogonal hyperedges
 * - Giuseppe di Battista et al., Graph Drawing: Algorithms for the Visualization of Graphs
 *
 * This is Phase 5 of the ELK Layered algorithm.
 *
 * Precondition: the graph has a proper layering with assigned node and port positions;
 *               the size of each layer is correctly set
 *
 * Postcondition: each node is assigned a horizontal coordinate; the bend points of each
 *                edge are set; the width of the whole graph is set
 */
class OrthogonalEdgeRouter {
public:
    /**
     * Main entry point: routes all edges in the layered graph using orthogonal routing.
     * This also computes and sets horizontal node positions, taking into account the
     * dynamic spacing required for edge routing slots.
     *
     * @param layeredGraph the layered graph to process
     */
    static void process(LGraph* layeredGraph);
};

} // namespace p5edges
} // namespace layered
} // namespace elk
