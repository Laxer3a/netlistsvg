// Eclipse Layout Kernel - C++ Port
// Recursive layout implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/core/recursive_layout.h"

namespace elk {

void RecursiveLayoutEngine::layout(Node* root, ILayoutProvider* provider,
                                   const RecursiveConfig& config) {
    if (!root || !provider) return;

    switch (config.strategy) {
        case RecursiveStrategy::TOP_DOWN:
            layoutTopDown(root, provider, config);
            break;
        case RecursiveStrategy::BOTTOM_UP:
            layoutBottomUp(root, provider, config);
            break;
        default:
            layoutTopDown(root, provider, config);
    }
}

void RecursiveLayoutEngine::layoutTopDown(Node* node, ILayoutProvider* provider,
                                         const RecursiveConfig& config) {
    if (!node) return;

    // Layout current node
    provider->layout(node);

    // Layout children recursively
    if (config.layoutChildren) {
        for (auto& child : node->children) {
            layoutTopDown(child.get(), provider, config);
        }
    }

    // Adjust parent size if needed
    if (config.adjustParentSize) {
        adjustParentSize(node, config);
    }
}

void RecursiveLayoutEngine::layoutBottomUp(Node* node, ILayoutProvider* provider,
                                          const RecursiveConfig& config) {
    if (!node) return;

    // Layout children first
    if (config.layoutChildren) {
        for (auto& child : node->children) {
            layoutBottomUp(child.get(), provider, config);
        }
    }

    // Then layout current node
    provider->layout(node);

    // Adjust parent size
    if (config.adjustParentSize) {
        adjustParentSize(node, config);
    }
}

void RecursiveLayoutEngine::adjustParentSize(Node* parent,
                                            const RecursiveConfig& config) {
    if (!parent || parent->children.empty()) return;

    double maxX = 0.0, maxY = 0.0;

    for (const auto& child : parent->children) {
        maxX = std::max(maxX, child->position.x + child->size.width);
        maxY = std::max(maxY, child->position.y + child->size.height);
    }

    parent->size.width = maxX + config.minParentPadding;
    parent->size.height = maxY + config.minParentPadding;
}

} // namespace elk
