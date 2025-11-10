// Eclipse Layout Kernel - C++ Port
// Graph model implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/graph/graph.h"

namespace elk {

Point Port::getAbsolutePosition() const {
    if (parent == nullptr) {
        return position;
    }
    Point parentPos = parent->getAbsolutePosition();
    return {
        parentPos.x + position.x,
        parentPos.y + position.y
    };
}

bool Edge::isSelfLoop() const {
    // Check if all sources and targets reference the same node
    Node* refNode = nullptr;

    for (Node* src : sourceNodes) {
        if (refNode == nullptr) refNode = src;
        else if (refNode != src) return false;
    }

    for (Port* src : sourcePorts) {
        if (src->parent == nullptr) continue;
        if (refNode == nullptr) refNode = src->parent;
        else if (refNode != src->parent) return false;
    }

    for (Node* tgt : targetNodes) {
        if (refNode == nullptr) refNode = tgt;
        else if (refNode != tgt) return false;
    }

    for (Port* tgt : targetPorts) {
        if (tgt->parent == nullptr) continue;
        if (refNode == nullptr) refNode = tgt->parent;
        else if (refNode != tgt->parent) return false;
    }

    return refNode != nullptr;
}

} // namespace elk
