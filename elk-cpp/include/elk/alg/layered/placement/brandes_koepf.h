// Eclipse Layout Kernel - C++ Port
// Brandes-Köpf algorithm for node placement with edge straightening
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../../core/types.h"
#include "../layered_layout.h"
#include <vector>
#include <unordered_map>

namespace elk {
namespace layered {
namespace placement {

// ============================================================================
// Brandes-Köpf Types
// ============================================================================

enum class BKDirection {
    UP_LEFT,
    UP_RIGHT,
    DOWN_LEFT,
    DOWN_RIGHT
};

struct BKBlock {
    std::vector<LNode*> nodes;     // Nodes in this block
    LNode* root = nullptr;         // Root node of block
    double position = 0.0;         // Block position
    double width = 0.0;            // Block width
};

struct BKAlignment {
    std::unordered_map<LNode*, LNode*> root;      // Root of each node's block
    std::unordered_map<LNode*, LNode*> align;     // Aligned neighbor
    std::unordered_map<LNode*, BKBlock*> block;   // Block for each node
};

// ============================================================================
// Brandes-Köpf Configuration
// ============================================================================

struct BrandesKoepfConfig {
    double nodeSpacing = 20.0;         // Minimum spacing between nodes
    double layerSpacing = 80.0;        // Spacing between layers

    bool straightenEdges = true;       // Try to straighten edges
    bool balanceBlocks = true;         // Balance block placement
    bool compactBlocks = true;         // Compact blocks horizontally

    double edgeNodeSpacing = 10.0;     // Spacing between edge and non-aligned node
    int minBlockSize = 1;              // Minimum nodes in a block
};

// ============================================================================
// Brandes-Köpf Placer
// ============================================================================

class BrandesKoepfPlacer {
public:
    static void place(std::vector<Layer>& layers,
                     const BrandesKoepfConfig& config);

private:
    // Phase 1: Vertical alignment (4 passes)
    static BKAlignment verticalAlignment(
        const std::vector<Layer>& layers,
        BKDirection direction,
        const BrandesKoepfConfig& config);

    // Phase 2: Horizontal compaction
    static void horizontalCompaction(
        BKAlignment& alignment,
        const std::vector<Layer>& layers,
        BKDirection direction,
        const BrandesKoepfConfig& config);

    // Phase 3: Compute final positions
    static void computePositions(
        std::vector<Layer>& layers,
        const std::vector<BKAlignment>& alignments,
        const BrandesKoepfConfig& config);

    // Alignment helpers
    static void markType1Conflicts(
        const std::vector<Layer>& layers,
        std::unordered_map<LNode*, std::vector<LNode*>>& conflicts);

    static LNode* median(const std::vector<LNode*>& neighbors);

    // Compaction helpers
    static void placeBlock(
        BKBlock* block,
        const std::vector<Layer>& layers,
        std::unordered_map<BKBlock*, double>& sink,
        std::unordered_map<BKBlock*, double>& shift,
        const BrandesKoepfConfig& config);

    static double separation(BKBlock* leftBlock, BKBlock* rightBlock,
                           const BrandesKoepfConfig& config);

    // Balance positions from multiple alignments
    static double balancePosition(const std::vector<double>& positions);

    // Get neighbors in specified direction
    static std::vector<LNode*> getNeighbors(
        LNode* node,
        const std::vector<Layer>& layers,
        BKDirection direction);

    // Check if nodes can be aligned
    static bool canAlign(LNode* u, LNode* v,
                        const std::unordered_map<LNode*, std::vector<LNode*>>& conflicts);
};

// ============================================================================
// Block Builder
// ============================================================================

class BlockBuilder {
public:
    // Build blocks from alignment
    static std::vector<BKBlock> build(
        const BKAlignment& alignment,
        const std::vector<Layer>& layers);

private:
    // Find all roots
    static std::vector<LNode*> findRoots(const BKAlignment& alignment);

    // Collect nodes in block
    static std::vector<LNode*> collectBlockNodes(
        LNode* root,
        const BKAlignment& alignment);

    // Calculate block width
    static double calculateBlockWidth(
        const std::vector<LNode*>& nodes,
        const BrandesKoepfConfig& config);
};

// ============================================================================
// Conflict Detector
// ============================================================================

class ConflictDetector {
public:
    // Detect type-1 conflicts (crossing conflicts)
    static void detectType1Conflicts(
        const std::vector<Layer>& layers,
        std::unordered_map<LNode*, std::vector<LNode*>>& conflicts);

private:
    // Check if two edge segments cross
    static bool edgesCross(LNode* u1, LNode* v1, LNode* u2, LNode* v2,
                          const std::vector<Layer>& layers);

    // Get node position in layer
    static int getPosition(LNode* node, const Layer& layer);
};

// ============================================================================
// Utility Functions
// ============================================================================

// Get layer index for node
inline int getLayerIndex(LNode* node, const std::vector<Layer>& layers) {
    for (size_t i = 0; i < layers.size(); ++i) {
        for (LNode* n : layers[i].nodes) {
            if (n == node) {
                return i;
            }
        }
    }
    return -1;
}

// Get node index in layer
inline int getNodeIndex(LNode* node, const Layer& layer) {
    for (size_t i = 0; i < layer.nodes.size(); ++i) {
        if (layer.nodes[i] == node) {
            return i;
        }
    }
    return -1;
}

// Get adjacent layer index
inline int getAdjacentLayerIndex(int currentLayer, BKDirection direction,
                                const std::vector<Layer>& layers) {
    switch (direction) {
        case BKDirection::UP_LEFT:
        case BKDirection::UP_RIGHT:
            return currentLayer - 1;
        case BKDirection::DOWN_LEFT:
        case BKDirection::DOWN_RIGHT:
            return currentLayer + 1;
        default:
            return -1;
    }
}

// Check if direction is leftward
inline bool isLeftward(BKDirection direction) {
    return direction == BKDirection::UP_LEFT || direction == BKDirection::DOWN_LEFT;
}

// Check if direction is upward
inline bool isUpward(BKDirection direction) {
    return direction == BKDirection::UP_LEFT || direction == BKDirection::UP_RIGHT;
}

// Get median node from list
inline LNode* getMedianNode(const std::vector<LNode*>& nodes) {
    if (nodes.empty()) return nullptr;
    if (nodes.size() == 1) return nodes[0];

    size_t m = nodes.size() / 2;
    return nodes[m];
}

// Calculate average position
inline double averagePosition(const std::vector<double>& positions) {
    if (positions.empty()) return 0.0;

    double sum = 0.0;
    for (double pos : positions) {
        sum += pos;
    }

    return sum / positions.size();
}

// Calculate median position
inline double medianPosition(std::vector<double> positions) {
    if (positions.empty()) return 0.0;

    std::sort(positions.begin(), positions.end());

    size_t n = positions.size();
    if (n % 2 == 0) {
        return (positions[n / 2 - 1] + positions[n / 2]) / 2.0;
    } else {
        return positions[n / 2];
    }
}

// Get minimum spacing between blocks
inline double getMinSpacing(const BKBlock& left, const BKBlock& right,
                           const BrandesKoepfConfig& config) {
    return config.nodeSpacing;
}

} // namespace placement
} // namespace layered
} // namespace elk
