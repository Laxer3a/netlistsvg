// Eclipse Layout Kernel - C++ Port
// Incremental layout support
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../core/types.h"
#include "../../graph/graph.h"
#include <vector>
#include <unordered_set>

namespace elk {
namespace incremental {

// ============================================================================
// Incremental Layout Types
// ============================================================================

enum class ChangeType {
    NODE_ADDED,
    NODE_REMOVED,
    NODE_MOVED,
    EDGE_ADDED,
    EDGE_REMOVED
};

struct GraphChange {
    ChangeType type;
    Node* node = nullptr;
    Edge* edge = nullptr;
    Point oldPosition;
    Point newPosition;
};

struct IncrementalConfig {
    bool preservePositions = true;
    bool animateChanges = false;
    double stabilityFactor = 0.8;  // How much to preserve old layout
};

// ============================================================================
// Incremental Layout Engine
// ============================================================================

class IncrementalLayoutEngine {
public:
    static void layout(Node* graph, const std::vector<GraphChange>& changes,
                      const IncrementalConfig& config);

private:
    static std::unordered_set<Node*> findAffectedNodes(
        Node* graph, const std::vector<GraphChange>& changes);

    static void preserveUnaffectedNodes(Node* graph,
                                       const std::unordered_set<Node*>& affected);

    static void layoutAffectedRegion(Node* graph,
                                    const std::unordered_set<Node*>& affected,
                                    const IncrementalConfig& config);
};

} // namespace incremental
} // namespace elk
