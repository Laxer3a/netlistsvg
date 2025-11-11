// Eclipse Layout Kernel - C++ Port
// Routing direction enumeration
// SPDX-License-Identifier: EPL-2.0

#pragma once

namespace elk {
namespace layered {
namespace p5edges {

/**
 * Enumeration of available routing directions. This is different from a graph's Direction
 * because different routing directions may arise in a single graph. Also, we don't have
 * an east-to-west routing direction here.
 */
enum class RoutingDirection {
    /** west to east routing direction (left to right). */
    WEST_TO_EAST,

    /** north to south routing direction (top to bottom). */
    NORTH_TO_SOUTH,

    /** south to north routing direction (bottom to top). */
    SOUTH_TO_NORTH
};

} // namespace p5edges
} // namespace layered
} // namespace elk
