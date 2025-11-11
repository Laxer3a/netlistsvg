// Eclipse Layout Kernel - C++ Port
// Layered graph implementation (FAITHFUL PORT from Java)
// SPDX-License-Identifier: EPL-2.0

#include "elk/alg/layered/lgraph.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace elk {
namespace layered {

// ============================================================================
// LNode Implementation
// ============================================================================

void LNode::setLayer(Layer* l) {
    if (layer != nullptr) {
        auto& layerNodes = layer->getNodes();
        layerNodes.erase(std::remove(layerNodes.begin(), layerNodes.end(), this), layerNodes.end());
    }

    layer = l;

    if (layer != nullptr) {
        layer->getNodes().push_back(this);
    }
}

void LNode::setLayer(int index, Layer* l) {
    if (l != nullptr && (index < 0 || index > static_cast<int>(l->getNodes().size()))) {
        throw std::invalid_argument("index must be >= 0 and <= layer node count");
    }

    if (layer != nullptr) {
        auto& layerNodes = layer->getNodes();
        layerNodes.erase(std::remove(layerNodes.begin(), layerNodes.end(), this), layerNodes.end());
    }

    layer = l;

    if (l != nullptr) {
        l->getNodes().insert(l->getNodes().begin() + index, this);
    }
}

LGraph* LNode::getGraph() const {
    if (graph == nullptr && layer != nullptr) {
        return layer->getGraph();
    }
    return graph;
}

std::vector<LEdge*> LNode::getIncomingEdges() const {
    std::vector<LEdge*> result;
    for (LPort* port : ports) {
        for (LEdge* edge : port->getIncomingEdges()) {
            result.push_back(edge);
        }
    }
    return result;
}

std::vector<LEdge*> LNode::getOutgoingEdges() const {
    std::vector<LEdge*> result;
    for (LPort* port : ports) {
        for (LEdge* edge : port->getOutgoingEdges()) {
            result.push_back(edge);
        }
    }
    return result;
}

std::vector<LEdge*> LNode::getConnectedEdges() const {
    std::vector<LEdge*> result;
    for (LPort* port : ports) {
        for (LEdge* edge : port->getIncomingEdges()) {
            result.push_back(edge);
        }
        for (LEdge* edge : port->getOutgoingEdges()) {
            result.push_back(edge);
        }
    }
    return result;
}

int LNode::getIndex() const {
    if (layer == nullptr) {
        return -1;
    }
    const auto& nodes = layer->getNodes();
    auto it = std::find(nodes.begin(), nodes.end(), this);
    if (it != nodes.end()) {
        return std::distance(nodes.begin(), it);
    }
    return -1;
}

void LNode::cachePortSides() {
    portSidesCached = true;
    portSideIndices.clear();

    if (ports.empty()) return;

    int firstIndexForCurrentSide = 0;
    PortSide currentSide = ports.front()->getSide();
    int currentIndex = 0;

    auto it = ports.begin();
    for (; it != ports.end(); ++it, ++currentIndex) {
        LPort* port = *it;
        if (port->getSide() != currentSide) {
            if (firstIndexForCurrentSide != currentIndex) {
                portSideIndices[currentSide] = {firstIndexForCurrentSide, currentIndex};
            }
            currentSide = port->getSide();
            firstIndexForCurrentSide = currentIndex;
        }
    }
    portSideIndices[currentSide] = {firstIndexForCurrentSide, currentIndex};
}

std::vector<LPort*> LNode::getPortSideView(PortSide side) {
    std::vector<LPort*> result;

    if (!portSidesCached) {
        cachePortSides();
    }

    auto it = portSideIndices.find(side);
    if (it != portSideIndices.end()) {
        auto [start, end] = it->second;
        auto portIt = ports.begin();
        std::advance(portIt, start);
        for (int i = start; i < end && portIt != ports.end(); ++i, ++portIt) {
            result.push_back(*portIt);
        }
    }

    return result;
}

std::string LNode::getDesignation() const {
    if (!labels.empty() && !labels[0].getText().empty()) {
        return labels[0].getText();
    }
    if (!id.empty()) {
        return id;
    }
    return std::to_string(getIndex());
}

// ============================================================================
// LPort Implementation
// ============================================================================

void LPort::setNode(LNode* n) {
    if (node != nullptr) {
        node->getPorts().remove(this);
    }

    node = n;

    if (node != nullptr) {
        node->getPorts().push_back(this);
    }
}

void LPort::setSide(PortSide s) {
    side = s;

    // Set default anchor based on side if not explicitly supplied
    if (!explicitlySuppliedPortAnchor) {
        switch (side) {
        case PortSide::NORTH:
            anchor.x = size.width / 2;
            anchor.y = 0;
            break;
        case PortSide::EAST:
            anchor.x = size.width;
            anchor.y = size.height / 2;
            break;
        case PortSide::SOUTH:
            anchor.x = size.width / 2;
            anchor.y = size.height;
            break;
        case PortSide::WEST:
            anchor.x = 0;
            anchor.y = size.height / 2;
            break;
        default:
            break;
        }
    }
}

Point LPort::getAbsoluteAnchor() const {
    if (node == nullptr) {
        return anchor;
    }
    Point result{
        node->getPosition().x + position.x + anchor.x,
        node->getPosition().y + position.y + anchor.y
    };
    std::cerr << "      LPort::getAbsoluteAnchor: node=(" << node->getPosition().x << "," << node->getPosition().y
              << ") port=(" << position.x << "," << position.y
              << ") anchor=(" << anchor.x << "," << anchor.y
              << ") result=(" << result.x << "," << result.y << ")\n";
    return result;
}

int LPort::getIndex() const {
    if (node == nullptr) {
        return -1;
    }
    const auto& nodePorts = node->getPorts();
    auto it = std::find(nodePorts.begin(), nodePorts.end(), this);
    if (it != nodePorts.end()) {
        return std::distance(nodePorts.begin(), it);
    }
    return -1;
}

std::vector<LPort*> LPort::getConnectedPorts() const {
    std::vector<LPort*> connectedPorts;

    // Add source ports of incoming edges
    for (LEdge* edge : incomingEdges) {
        if (edge->getSource() != nullptr) {
            connectedPorts.push_back(edge->getSource());
        }
    }

    // Add target ports of outgoing edges
    for (LEdge* edge : outgoingEdges) {
        if (edge->getTarget() != nullptr) {
            connectedPorts.push_back(edge->getTarget());
        }
    }

    return connectedPorts;
}

std::string LPort::getDesignation() const {
    if (!labels.empty() && !labels[0].getText().empty()) {
        return labels[0].getText();
    }
    if (!id.empty()) {
        return id;
    }
    return std::to_string(getIndex());
}

// ============================================================================
// LEdge Implementation
// ============================================================================

void LEdge::setSource(LPort* src) {
    if (source != nullptr) {
        source->getOutgoingEdges().remove(this);
    }

    source = src;

    if (source != nullptr) {
        source->getOutgoingEdges().push_back(this);
        // Debug check
        if (source->getNode() && source->getNode()->originalNode) {
            // This will be too verbose, skip for now
        }
    }
}

void LEdge::setTarget(LPort* tgt) {
    if (target != nullptr) {
        target->getIncomingEdges().remove(this);
    }

    target = tgt;

    if (target != nullptr) {
        target->getIncomingEdges().push_back(this);
    }
}

void LEdge::setTargetAndInsertAtIndex(LPort* tgt, int index) {
    if (target != nullptr) {
        target->getIncomingEdges().remove(this);
    }

    target = tgt;

    if (target != nullptr) {
        auto& edges = target->getIncomingEdges();
        auto it = edges.begin();
        std::advance(it, index);
        edges.insert(it, this);
    }
}

bool LEdge::isSelfLoop() const {
    if (source == nullptr || target == nullptr) {
        return false;
    }
    return source->getNode() != nullptr && source->getNode() == target->getNode();
}

bool LEdge::isInLayerEdge() const {
    if (isSelfLoop()) {
        return false;
    }
    if (source == nullptr || target == nullptr) {
        return false;
    }
    LNode* srcNode = source->getNode();
    LNode* tgtNode = target->getNode();
    if (srcNode == nullptr || tgtNode == nullptr) {
        return false;
    }
    return srcNode->getLayer() == tgtNode->getLayer();
}

LPort* LEdge::getOther(LPort* port) const {
    if (port == source) {
        return target;
    } else if (port == target) {
        return source;
    } else {
        throw std::invalid_argument("'port' must be either the source port or target port of the edge");
    }
}

LNode* LEdge::getOther(LNode* node) const {
    if (source == nullptr || target == nullptr) {
        throw std::invalid_argument("edge has no source or target");
    }
    if (node == source->getNode()) {
        return target->getNode();
    } else if (node == target->getNode()) {
        return source->getNode();
    } else {
        throw std::invalid_argument("'node' must either be the source node or target node of the edge");
    }
}

void LEdge::reverse(LGraph* graph, bool adaptPorts) {
    // Store old endpoints
    LPort* oldSource = source;
    LPort* oldTarget = target;

    // Clear connections
    setSource(nullptr);
    setTarget(nullptr);

    // Reconnect reversed (simplified - no collector port handling for now)
    setSource(oldTarget);
    setTarget(oldSource);

    // Reverse REVERSED property
    bool reversed = hasProperty("REVERSED") ? getProperty<bool>("REVERSED") : false;
    setProperty("REVERSED", !reversed);

    // Reverse bend points
    std::reverse(bendPoints.begin(), bendPoints.end());

    // TODO: Switch end label placements (HEAD <-> TAIL)
}

std::string LEdge::getDesignation() const {
    if (!labels.empty() && !labels[0].getText().empty()) {
        return labels[0].getText();
    }
    return id;
}

// ============================================================================
// LGraph Implementation
// ============================================================================

LNode* LGraph::addNode() {
    auto node = std::make_unique<LNode>(this);
    LNode* ptr = node.get();
    lnodes.push_back(std::move(node));
    return ptr;
}

LEdge* LGraph::addEdge() {
    auto edge = std::make_unique<LEdge>();
    LEdge* ptr = edge.get();
    ledges.push_back(std::move(edge));
    return ptr;
}

Layer* LGraph::addLayer() {
    auto layer = std::make_unique<Layer>(this);
    layer->index = layers.size();
    Layer* ptr = layer.get();
    layers.push_back(std::move(layer));
    return ptr;
}

Layer* LGraph::addLayer(int index) {
    if (index < 0 || index > static_cast<int>(layers.size())) {
        throw std::invalid_argument("index out of range");
    }

    auto layer = std::make_unique<Layer>(this);
    layer->index = index;
    Layer* ptr = layer.get();

    layers.insert(layers.begin() + index, std::move(layer));

    // Update indices of subsequent layers
    for (size_t i = index + 1; i < layers.size(); ++i) {
        layers[i]->index = i;
    }

    return ptr;
}

} // namespace layered
} // namespace elk
