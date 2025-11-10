// Eclipse Layout Kernel - C++ Port
// Consolidated header for all layout algorithms and features
// SPDX-License-Identifier: EPL-2.0

#pragma once

// Core algorithms
#include "force/force_layout.h"
#include "layered/layered_layout.h"
#include "mrtree/mrtree_layout.h"
#include "rectpacking/rectpacking_layout.h"

// Common utilities
#include "common/edge_routing.h"
#include "common/port_constraints.h"
#include "common/label_placement.h"
#include "common/compaction.h"

// Advanced layered features
#include "layered/intermediate/self_loops.h"
#include "layered/intermediate/greedy_switch.h"
#include "layered/crossing/median_heuristic.h"
#include "layered/placement/network_simplex.h"
#include "layered/placement/brandes_koepf.h"

// Advanced features
#include "common/node_spacing.h"
#include "common/comments.h"
#include "common/incremental.h"

// Layout strategies
#include "../core/recursive_layout.h"
#include "../core/configuration.h"

namespace elk {

// Convenience function to get layout provider by name
inline ILayoutProvider* createLayoutProvider(const std::string& algorithmId) {
    if (algorithmId == "elk.force" || algorithmId == "force") {
        return new force::ForceLayoutProvider();
    } else if (algorithmId == "elk.layered" || algorithmId == "layered") {
        return new layered::LayeredLayoutProvider();
    } else if (algorithmId == "elk.mrtree" || algorithmId == "mrtree") {
        return new mrtree::MrTreeLayoutProvider();
    } else if (algorithmId == "elk.rectpacking" || algorithmId == "rectpacking") {
        return new rectpacking::RectPackingLayoutProvider();
    }
    return nullptr;  // Unknown algorithm
}

} // namespace elk
