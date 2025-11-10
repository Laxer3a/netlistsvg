// Eclipse Layout Kernel - C++ Port
// Recursive/hierarchical layout strategies
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "layout_provider.h"
#include "types.h"
#include "../graph/graph.h"
#include <functional>

namespace elk {

// ============================================================================
// Recursive Layout Strategy
// ============================================================================

enum class RecursiveStrategy {
    TOP_DOWN,      // Layout parent first, then children
    BOTTOM_UP,     // Layout children first, then parent
    INSIDE_OUT     // Layout from center outward
};

struct RecursiveConfig {
    RecursiveStrategy strategy = RecursiveStrategy::TOP_DOWN;
    bool layoutChildren = true;
    bool adjustParentSize = true;
    double minParentPadding = 10.0;
};

// ============================================================================
// Recursive Layout Engine
// ============================================================================

class RecursiveLayoutEngine {
public:
    static void layout(Node* root, ILayoutProvider* provider,
                      const RecursiveConfig& config);

private:
    static void layoutTopDown(Node* node, ILayoutProvider* provider,
                             const RecursiveConfig& config);

    static void layoutBottomUp(Node* node, ILayoutProvider* provider,
                              const RecursiveConfig& config);

    static void adjustParentSize(Node* parent, const RecursiveConfig& config);
};

} // namespace elk
