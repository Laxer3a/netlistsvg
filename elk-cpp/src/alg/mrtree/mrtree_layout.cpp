// Eclipse Layout Kernel - C++ Port
// MrTree implementation (Buchheim algorithm)
// SPDX-License-Identifier: EPL-2.0

#include "elk/alg/mrtree/mrtree_layout.h"
#include <algorithm>
#include <limits>
#include <cmath>

namespace elk {
namespace mrtree {

MrTreeLayoutProvider::MrTreeLayoutProvider() {}

void MrTreeLayoutProvider::layout(Node* graph, ProgressCallback progress) {
    if (!graph) return;

    if (progress) progress("Identifying trees", 0.1);
    auto trees = identifyTrees(graph);

    if (trees.empty()) return;

    if (progress) progress("Laying out trees", 0.3);
    for (auto* tree : trees) {
        layoutTree(tree);
    }

    if (progress) progress("Arranging trees", 0.7);
    arrangeTrees(trees);

    if (progress) progress("Applying layout", 0.9);
    applyLayout(trees);

    cleanup(trees);
    if (progress) progress("Complete", 1.0);
}

std::vector<TreeNode*> MrTreeLayoutProvider::identifyTrees(Node* graph) {
    std::vector<TreeNode*> trees;
    std::unordered_set<Node*> visited;
    std::unordered_map<Node*, TreeNode*> nodeMap;

    // Find roots
    for (auto& child : graph->children) {
        if (isPotentialRoot(child.get(), graph->edges)) {
            TreeNode* treeRoot = new TreeNode();
            treeRoot->originalNode = child.get();
            treeRoot->size = child->size;
            nodeMap[child.get()] = treeRoot;

            buildTree(child.get(), treeRoot, visited, nodeMap);
            trees.push_back(treeRoot);
        }
    }

    // Handle orphaned nodes
    for (auto& child : graph->children) {
        if (visited.find(child.get()) == visited.end()) {
            TreeNode* treeRoot = new TreeNode();
            treeRoot->originalNode = child.get();
            treeRoot->size = child->size;
            trees.push_back(treeRoot);
            visited.insert(child.get());
        }
    }

    // Sort by size if configured
    if (config_.orderBySize) {
        std::sort(trees.begin(), trees.end(), [this](TreeNode* a, TreeNode* b) {
            return calculateTreeSize(a) > calculateTreeSize(b);
        });
    }

    return trees;
}

void MrTreeLayoutProvider::buildTree(Node* node, TreeNode* treeNode,
                                     std::unordered_set<Node*>& visited,
                                     std::unordered_map<Node*, TreeNode*>& nodeMap) {
    visited.insert(node);

    auto children = getChildren(node, treeNode->originalNode->parent->edges);
    for (Node* childNode : children) {
        if (visited.find(childNode) == visited.end()) {
            TreeNode* childTreeNode = new TreeNode();
            childTreeNode->originalNode = childNode;
            childTreeNode->parent = treeNode;
            childTreeNode->size = childNode->size;
            childTreeNode->depth = treeNode->depth + 1;
            childTreeNode->number = treeNode->children.size();

            nodeMap[childNode] = childTreeNode;
            treeNode->children.push_back(childTreeNode);

            buildTree(childNode, childTreeNode, visited, nodeMap);
        }
    }
}

void MrTreeLayoutProvider::layoutTree(TreeNode* root) {
    if (!root) return;

    if (config_.placement == TreeNodePlacement::BUCHHEIM) {
        firstWalk(root);
        secondWalk(root, -root->prelim);
    } else {
        walkerLayout(root, 0);
    }
}

void MrTreeLayoutProvider::firstWalk(TreeNode* node) {
    if (node->children.empty()) {
        // Leaf node
        if (node->number > 0) {
            TreeNode* leftSibling = node->parent->children[node->number - 1];
            node->prelim = leftSibling->prelim + config_.nodeSpacing +
                          (leftSibling->size.width + node->size.width) / 2;
        } else {
            node->prelim = 0;
        }
    } else {
        // Internal node
        TreeNode* defaultAncestor = node->children[0];

        for (TreeNode* child : node->children) {
            firstWalk(child);
            defaultAncestor = apportion(child, defaultAncestor);
        }

        executeShifts(node);

        double midpoint = (node->children.front()->prelim +
                          node->children.back()->prelim) / 2;

        if (node->number > 0) {
            TreeNode* leftSibling = node->parent->children[node->number - 1];
            node->prelim = leftSibling->prelim + config_.nodeSpacing +
                          (leftSibling->size.width + node->size.width) / 2;
            node->mod = node->prelim - midpoint;
        } else {
            node->prelim = midpoint;
        }
    }
}

void MrTreeLayoutProvider::secondWalk(TreeNode* node, double m) {
    node->position.x = node->prelim + m;
    node->position.y = node->depth * config_.levelSpacing;

    for (TreeNode* child : node->children) {
        secondWalk(child, m + node->mod);
    }
}

TreeNode* MrTreeLayoutProvider::apportion(TreeNode* node, TreeNode* defaultAncestor) {
    if (node->number > 0) {
        TreeNode* leftSibling = node->parent->children[node->number - 1];

        TreeNode* vir = node;
        TreeNode* vor = node;
        TreeNode* vil = leftSibling;
        TreeNode* vol = node->parent->children[0];

        double sir = node->mod;
        double sor = node->mod;
        double sil = vil->mod;
        double sol = vol->mod;

        while (vil && vir) {
            // Navigate to next level
            if (!vil->children.empty()) vil = vil->children.back();
            else vil = nullptr;

            if (!vir->children.empty()) vir = vir->children.front();
            else vir = nullptr;

            if (vil && vir) {
                TreeNode* virAncestor = ancestor(vir, node, defaultAncestor);
                double shift = (vil->prelim + sil) - (vir->prelim + sir) +
                              config_.nodeSpacing;

                if (shift > 0) {
                    moveSubtree(virAncestor, node, shift);
                    sir += shift;
                    sor += shift;
                }

                sil += vil->mod;
                sir += vir->mod;
            }
        }
    }

    return defaultAncestor;
}

TreeNode* MrTreeLayoutProvider::ancestor(TreeNode* node, TreeNode* vim,
                                        TreeNode* defaultAncestor) {
    if (node->ancestor && node->ancestor->parent == vim->parent) {
        return node->ancestor;
    }
    return defaultAncestor;
}

void MrTreeLayoutProvider::moveSubtree(TreeNode* wm, TreeNode* wp, double shift) {
    int subtrees = wp->number - wm->number;
    if (subtrees > 0) {
        wp->change -= shift / subtrees;
        wp->shift += shift;
        wm->change += shift / subtrees;
        wp->prelim += shift;
        wp->mod += shift;
    }
}

void MrTreeLayoutProvider::executeShifts(TreeNode* node) {
    double shift = 0;
    double change = 0;

    for (int i = node->children.size() - 1; i >= 0; --i) {
        TreeNode* child = node->children[i];
        child->prelim += shift;
        child->mod += shift;
        change += child->change;
        shift += child->shift + change;
    }
}

void MrTreeLayoutProvider::walkerLayout(TreeNode* node, int depth) {
    node->position.y = depth * config_.levelSpacing;

    if (node->children.empty()) {
        if (node->number == 0) {
            node->position.x = 0;
        } else {
            TreeNode* leftSibling = node->parent->children[node->number - 1];
            node->position.x = leftSibling->position.x + config_.nodeSpacing +
                              (leftSibling->size.width + node->size.width) / 2;
        }
    } else {
        for (TreeNode* child : node->children) {
            walkerLayout(child, depth + 1);
        }

        double leftmost = node->children.front()->position.x;
        double rightmost = node->children.back()->position.x;
        node->position.x = (leftmost + rightmost) / 2;
    }
}

void MrTreeLayoutProvider::arrangeTrees(std::vector<TreeNode*>& trees) {
    double currentX = 0;

    for (TreeNode* tree : trees) {
        Rect bounds;
        calculateTreeBounds(tree, bounds);

        // Offset to avoid negative coordinates
        offsetTree(tree, Point(currentX - bounds.x, -bounds.y));

        currentX += bounds.width + config_.treeSpacing;
    }
}

void MrTreeLayoutProvider::applyLayout(const std::vector<TreeNode*>& trees) {
    for (TreeNode* tree : trees) {
        std::vector<TreeNode*> stack = {tree};

        while (!stack.empty()) {
            TreeNode* node = stack.back();
            stack.pop_back();

            if (node->originalNode) {
                node->originalNode->position = node->position;
            }

            for (TreeNode* child : node->children) {
                stack.push_back(child);
            }
        }
    }
}

void MrTreeLayoutProvider::calculateTreeBounds(TreeNode* root, Rect& bounds) {
    if (!root) return;

    bounds.x = root->position.x;
    bounds.y = root->position.y;
    bounds.width = root->size.width;
    bounds.height = root->size.height;

    std::vector<TreeNode*> stack = {root};
    while (!stack.empty()) {
        TreeNode* node = stack.back();
        stack.pop_back();

        double right = node->position.x + node->size.width;
        double bottom = node->position.y + node->size.height;

        bounds.x = std::min(bounds.x, node->position.x);
        bounds.y = std::min(bounds.y, node->position.y);
        bounds.width = std::max(bounds.width, right - bounds.x);
        bounds.height = std::max(bounds.height, bottom - bounds.y);

        for (TreeNode* child : node->children) {
            stack.push_back(child);
        }
    }
}

void MrTreeLayoutProvider::offsetTree(TreeNode* root, const Point& offset) {
    std::vector<TreeNode*> stack = {root};
    while (!stack.empty()) {
        TreeNode* node = stack.back();
        stack.pop_back();

        node->position.x += offset.x;
        node->position.y += offset.y;

        for (TreeNode* child : node->children) {
            stack.push_back(child);
        }
    }
}

int MrTreeLayoutProvider::calculateTreeSize(TreeNode* root) {
    if (!root) return 0;

    int size = 1;
    for (TreeNode* child : root->children) {
        size += calculateTreeSize(child);
    }
    return size;
}

void MrTreeLayoutProvider::cleanup(const std::vector<TreeNode*>& trees) {
    for (TreeNode* tree : trees) {
        std::vector<TreeNode*> stack = {tree};
        while (!stack.empty()) {
            TreeNode* node = stack.back();
            stack.pop_back();

            for (TreeNode* child : node->children) {
                stack.push_back(child);
            }

            delete node;
        }
    }
}

} // namespace mrtree
} // namespace elk
