// Eclipse Layout Kernel - C++ Port
// Graph model: Node, Edge, Port, Label
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../core/types.h"
#include "../core/properties.h"
#include <vector>
#include <memory>
#include <string>

namespace elk {

// Forward declarations
class Node;
class Edge;
class Port;
class Label;
class EdgeSection;

// ============================================================================
// Label - Text label that can be attached to nodes, edges, or ports
// ============================================================================

class Label : public PropertyHolder {
public:
    std::string text;
    Point position;
    Size size;

    // Parent element (can be Node, Edge, or Port)
    // In Java this is ElkGraphElement, but we don't have a common base class
    // so we use void* with a type tag
    void* parent = nullptr;
    enum class ParentType { NONE, NODE, EDGE, PORT };
    ParentType parentType = ParentType::NONE;

    std::string id;

    Label() = default;
    Label(const std::string& txt) : text(txt) {}
    Label(const std::string& txt, double w, double h)
        : text(txt), size(w, h) {}

    Rect getBounds() const { return {position.x, position.y, size.width, size.height}; }
};

// ============================================================================
// Port - Connection point on a node
// ============================================================================

class Port : public PropertyHolder {
public:
    Point position;      // Relative to parent node
    Size size;
    PortSide side = PortSide::UNDEFINED;
    std::vector<Label> labels;

    // Edge connectivity (from ElkConnectableShape)
    std::vector<Edge*> incomingEdges;  // Non-owning pointers to edges targeting this port
    std::vector<Edge*> outgoingEdges;  // Non-owning pointers to edges sourcing from this port

    int index = -1;      // Index within port list (for ordering)

    Node* parent = nullptr;  // Non-owning pointer
    std::string id;

    Port() = default;
    explicit Port(const std::string& id_) : id(id_) {}

    Rect getBounds() const { return {position.x, position.y, size.width, size.height}; }
    Point getAbsolutePosition() const;  // Implemented in cpp
};

// ============================================================================
// EdgeSection - Routing information for an edge (or part of a hyperedge)
// ============================================================================

class EdgeSection : public PropertyHolder {
public:
    Point startPoint;
    Point endPoint;
    std::vector<Point> bendPoints;

    // Parent edge
    Edge* parent = nullptr;  // Non-owning pointer to parent edge

    // Connections to shapes (for outer sections of hyperedges)
    // These can be either Node* or Port* - we need to track which type
    void* incomingShape = nullptr;   // ElkConnectableShape (Node or Port)
    void* outgoingShape = nullptr;   // ElkConnectableShape (Node or Port)
    enum class ShapeType { NONE, NODE, PORT };
    ShapeType incomingShapeType = ShapeType::NONE;
    ShapeType outgoingShapeType = ShapeType::NONE;

    // Connections to other sections (for inner sections of hyperedges)
    std::vector<EdgeSection*> incomingSections;  // Non-owning
    std::vector<EdgeSection*> outgoingSections;  // Non-owning

    std::string identifier;

    double getLength() const {
        double length = 0.0;
        Point prev = startPoint;
        for (const auto& bp : bendPoints) {
            length += (bp - prev).length();
            prev = bp;
        }
        length += (endPoint - prev).length();
        return length;
    }

    // Helper methods to set shapes with type safety
    void setIncomingNode(Node* node) {
        incomingShape = node;
        incomingShapeType = ShapeType::NODE;
    }

    void setIncomingPort(Port* port) {
        incomingShape = port;
        incomingShapeType = ShapeType::PORT;
    }

    void setOutgoingNode(Node* node) {
        outgoingShape = node;
        outgoingShapeType = ShapeType::NODE;
    }

    void setOutgoingPort(Port* port) {
        outgoingShape = port;
        outgoingShapeType = ShapeType::PORT;
    }

    Node* getIncomingNode() const {
        return (incomingShapeType == ShapeType::NODE) ? static_cast<Node*>(incomingShape) : nullptr;
    }

    Port* getIncomingPort() const {
        return (incomingShapeType == ShapeType::PORT) ? static_cast<Port*>(incomingShape) : nullptr;
    }

    Node* getOutgoingNode() const {
        return (outgoingShapeType == ShapeType::NODE) ? static_cast<Node*>(outgoingShape) : nullptr;
    }

    Port* getOutgoingPort() const {
        return (outgoingShapeType == ShapeType::PORT) ? static_cast<Port*>(outgoingShape) : nullptr;
    }
};

// ============================================================================
// Edge - Connection between nodes/ports
// ============================================================================

class Edge : public PropertyHolder {
public:
    std::vector<EdgeSection> sections;
    std::vector<Label> labels;
    std::vector<Point> junctionPoints;  // Points where edges join/split (for multi-driver/receiver nets)

    // Sources and targets (can be nodes or ports)
    std::vector<Node*> sourceNodes;
    std::vector<Port*> sourcePorts;
    std::vector<Node*> targetNodes;
    std::vector<Port*> targetPorts;

    Node* containingNode = nullptr;  // Non-owning pointer
    std::string id;

    Edge() = default;
    explicit Edge(const std::string& id_) : id(id_) {}

    bool isHyperedge() const {
        return (sourceNodes.size() + sourcePorts.size() > 1) ||
               (targetNodes.size() + targetPorts.size() > 1);
    }

    bool isSelfLoop() const;  // Implemented in cpp
};

// ============================================================================
// Node - Graph node with position, size, and hierarchy
// ============================================================================

class Node : public PropertyHolder {
public:
    Point position;
    Size size;
    Padding padding;

    std::vector<std::unique_ptr<Node>> children;     // Owned child nodes
    std::vector<std::unique_ptr<Port>> ports;        // Owned ports
    std::vector<std::unique_ptr<Edge>> edges;        // Owned edges (contained in this node)
    std::vector<Label> labels;

    // Edge connectivity (from ElkConnectableShape)
    std::vector<Edge*> incomingEdges;  // Non-owning pointers to edges targeting this node
    std::vector<Edge*> outgoingEdges;  // Non-owning pointers to edges sourcing from this node

    Node* parent = nullptr;  // Non-owning pointer to parent
    std::string id;

    Node() = default;
    explicit Node(const std::string& id_) : id(id_) {}

    // Factory methods for building graph
    Node* addChild(const std::string& childId = "") {
        auto child = std::make_unique<Node>(childId);
        child->parent = this;
        children.push_back(std::move(child));
        return children.back().get();
    }

    Port* addPort(const std::string& portId = "", PortSide side = PortSide::UNDEFINED) {
        auto port = std::make_unique<Port>(portId);
        port->parent = this;
        port->side = side;
        ports.push_back(std::move(port));
        return ports.back().get();
    }

    Edge* addEdge(const std::string& edgeId = "") {
        auto edge = std::make_unique<Edge>(edgeId);
        edge->containingNode = this;
        edges.push_back(std::move(edge));
        return edges.back().get();
    }

    // Convenience methods
    bool isHierarchical() const { return !children.empty(); }

    Rect getBounds() const {
        return {position.x, position.y, size.width, size.height};
    }

    Point getAbsolutePosition() const {
        if (parent == nullptr) {
            return position;
        }
        Point parentPos = parent->getAbsolutePosition();
        return {parentPos.x + position.x + parent->padding.left,
                parentPos.y + position.y + parent->padding.top};
    }

    // Find nodes/ports/edges by ID
    Node* findNode(const std::string& nodeId) {
        if (id == nodeId) return this;
        for (auto& child : children) {
            if (auto found = child->findNode(nodeId)) {
                return found;
            }
        }
        return nullptr;
    }

    Port* findPort(const std::string& portId) {
        for (auto& port : ports) {
            if (port->id == portId) {
                return port.get();
            }
        }
        return nullptr;
    }

    // Get all edges (including in child nodes)
    void collectAllEdges(std::vector<Edge*>& allEdges) {
        for (auto& edge : edges) {
            allEdges.push_back(edge.get());
        }
        for (auto& child : children) {
            child->collectAllEdges(allEdges);
        }
    }

    // Calculate bounding box including all children
    Rect calculateBoundingBox() const {
        if (children.empty()) {
            return getBounds();
        }

        Rect bounds = getBounds();
        for (const auto& child : children) {
            Rect childBounds = child->calculateBoundingBox();
            Point childAbsPos = child->getAbsolutePosition();
            Rect absChildBounds = {
                childAbsPos.x,
                childAbsPos.y,
                childBounds.width,
                childBounds.height
            };
            bounds = bounds.union_(absChildBounds);
        }
        return bounds;
    }
};

// ============================================================================
// Helper functions for building graphs
// ============================================================================

inline Edge* connectNodes(Node* from, Node* to, Node* container) {
    Edge* edge = container->addEdge();
    edge->sourceNodes.push_back(from);
    edge->targetNodes.push_back(to);
    return edge;
}

inline Edge* connectPorts(Port* from, Port* to, Node* container) {
    Edge* edge = container->addEdge();
    edge->sourcePorts.push_back(from);
    edge->targetPorts.push_back(to);
    return edge;
}

inline Edge* connectNodeToPort(Node* from, Port* to, Node* container) {
    Edge* edge = container->addEdge();
    edge->sourceNodes.push_back(from);
    edge->targetPorts.push_back(to);
    return edge;
}

inline Edge* connectPortToNode(Port* from, Node* to, Node* container) {
    Edge* edge = container->addEdge();
    edge->sourcePorts.push_back(from);
    edge->targetNodes.push_back(to);
    return edge;
}

} // namespace elk
