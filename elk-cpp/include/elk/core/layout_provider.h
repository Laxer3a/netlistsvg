// Eclipse Layout Kernel - C++ Port
// Layout provider interface
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../graph/graph.h"
#include <functional>

namespace elk {

// Progress monitoring callback
using ProgressCallback = std::function<void(const std::string& task, double progress)>;

// ============================================================================
// ILayoutProvider - Interface for layout algorithms
// ============================================================================

class ILayoutProvider {
public:
    virtual ~ILayoutProvider() = default;

    // Main layout function
    virtual void layout(Node* graph, ProgressCallback progress = nullptr) = 0;

    // Get algorithm name/identifier
    virtual std::string getAlgorithmId() const = 0;
};

// ============================================================================
// LayoutContext - Additional context for layout algorithms
// ============================================================================

struct LayoutContext {
    bool animate = false;              // Whether to support animation
    bool includeChildren = true;       // Layout hierarchical children
    double borderSpacing = 12.0;       // Space between nodes and container border
    double nodeSpacing = 20.0;         // Default spacing between nodes
    double portSpacing = 10.0;         // Default spacing between ports
    Direction layoutDirection = Direction::RIGHT;
    EdgeRouting edgeRouting = EdgeRouting::POLYLINE;
};

} // namespace elk
