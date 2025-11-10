// Eclipse Layout Kernel - C++ Port
// MrTree (Multi-Root Tree) layout algorithm
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../core/layout_provider.h"
#include "../../core/types.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace elk {
namespace mrtree {

// ============================================================================
// Tree Node - Internal representation
// ============================================================================

struct TreeNode {
    Node* originalNode = nullptr;
    TreeNode* parent = nullptr;
    std::vector<TreeNode*> children;

    Point position;
    Size size;
    int depth = 0;           // Distance from root
    double mod = 0.0;        // Modifier for relative positioning
    double prelim = 0.0;     // Preliminary x coordinate
    double change = 0.0;     // Change for subtree
    double shift = 0.0;      // Shift for subtree

    int number = 0;          // Child number
    TreeNode* thread = nullptr;  // Threading for traversal
    TreeNode* ancestor = nullptr;
};

// ============================================================================
// MrTree Configuration
// ============================================================================

enum class TreeLayoutDirection {
    DOWN,    // Root at top, grows downward
    UP,      // Root at bottom, grows upward
    RIGHT,   // Root at left, grows right
    LEFT     // Root at right, grows left
};

enum class TreeNodePlacement {
    SIMPLE,           // Simple recursive placement
    REINGOLD_TILFORD, // Reingold-Tilford algorithm (better aesthetics)
    WALKER,           // Walker's linear-time algorithm
    BUCHHEIM          // Buchheim's improved algorithm
};

struct MrTreeConfig {
    TreeLayoutDirection direction = TreeLayoutDirection::DOWN;
    TreeNodePlacement placement = TreeNodePlacement::BUCHHEIM;

    double nodeSpacing = 20.0;      // Horizontal spacing between siblings
    double levelSpacing = 80.0;     // Vertical spacing between levels
    double treeSpacing = 100.0;     // Spacing between separate trees
    double aspectRatio = 1.0;       // Preferred aspect ratio

    bool compactTrees = true;       // Pack trees tightly
    bool orderBySize = true;        // Order trees by size (largest first)
};

// ============================================================================
// MrTree Layout Provider
// ============================================================================

class MrTreeLayoutProvider : public ILayoutProvider {
public:
    MrTreeLayoutProvider();

    void layout(Node* graph, ProgressCallback progress = nullptr) override;
    std::string getAlgorithmId() const override { return "elk.mrtree"; }

    // Configuration
    void setDirection(TreeLayoutDirection dir) { config_.direction = dir; }
    void setPlacement(TreeNodePlacement placement) { config_.placement = placement; }
    void setNodeSpacing(double spacing) { config_.nodeSpacing = spacing; }
    void setLevelSpacing(double spacing) { config_.levelSpacing = spacing; }
    void setTreeSpacing(double spacing) { config_.treeSpacing = spacing; }

private:
    // Phase 1: Identify tree roots and build tree structures
    std::vector<TreeNode*> identifyTrees(Node* graph);
    void buildTree(Node* root, TreeNode* treeRoot,
                   std::unordered_set<Node*>& visited,
                   std::unordered_map<Node*, TreeNode*>& nodeMap);

    // Phase 2: Layout individual trees
    void layoutTree(TreeNode* root);

    // Buchheim algorithm phases
    void firstWalk(TreeNode* node);
    void secondWalk(TreeNode* node, double m);

    // Helper methods for Buchheim
    TreeNode* ancestor(TreeNode* node, TreeNode* vim, TreeNode* defaultAncestor);
    void moveSubtree(TreeNode* wm, TreeNode* wp, double shift);
    void executeShifts(TreeNode* node);
    TreeNode* apportion(TreeNode* node, TreeNode* defaultAncestor);

    // Walker's algorithm
    void walkerLayout(TreeNode* node, int depth);

    // Phase 3: Arrange multiple trees
    void arrangeTrees(std::vector<TreeNode*>& trees);

    // Phase 4: Apply layout back to graph
    void applyLayout(const std::vector<TreeNode*>& trees);

    // Utilities
    void calculateTreeBounds(TreeNode* root, Rect& bounds);
    void offsetTree(TreeNode* root, const Point& offset);
    int calculateTreeSize(TreeNode* root);
    void cleanup(const std::vector<TreeNode*>& trees);

    MrTreeConfig config_;
};

// ============================================================================
// Tree Utilities
// ============================================================================

// Check if node is a potential tree root (no incoming edges or specified as root)
inline bool isPotentialRoot(Node* node, const std::vector<std::unique_ptr<Edge>>& edges) {
    for (const auto& edge : edges) {
        for (Node* target : edge->targetNodes) {
            if (target == node) {
                return false;  // Has incoming edge
            }
        }
    }
    return true;
}

// Get children of a node
inline std::vector<Node*> getChildren(Node* node, const std::vector<std::unique_ptr<Edge>>& edges) {
    std::vector<Node*> children;
    for (const auto& edge : edges) {
        for (Node* source : edge->sourceNodes) {
            if (source == node) {
                for (Node* target : edge->targetNodes) {
                    children.push_back(target);
                }
            }
        }
    }
    return children;
}

// Calculate contour of tree for tight packing
inline double getLeftContour(TreeNode* node, int level,
                             std::vector<double>& leftContour) {
    if (level >= leftContour.size()) {
        leftContour.resize(level + 1, std::numeric_limits<double>::max());
    }
    leftContour[level] = std::min(leftContour[level], node->position.x);

    for (TreeNode* child : node->children) {
        getLeftContour(child, level + 1, leftContour);
    }
    return leftContour[level];
}

inline double getRightContour(TreeNode* node, int level,
                              std::vector<double>& rightContour) {
    if (level >= rightContour.size()) {
        rightContour.resize(level + 1, std::numeric_limits<double>::lowest());
    }
    rightContour[level] = std::max(rightContour[level],
                                   node->position.x + node->size.width);

    for (TreeNode* child : node->children) {
        getRightContour(child, level + 1, rightContour);
    }
    return rightContour[level];
}

} // namespace mrtree
} // namespace elk
