// Eclipse Layout Kernel - C++ Port
// Layered graph internal structures (FAITHFUL PORT from Java)
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../core/types.h"
#include "../../core/properties.h"
#include "../../graph/graph.h"
#include <vector>
#include <list>
#include <string>
#include <memory>
#include <climits>
#include <random>

namespace elk {
namespace layered {

// Forward declarations
class LGraph;
class LNode;
class LPort;
class LEdge;
class LLabel;
class Layer;

// ============================================================================
// LMargin - Margin around elements (space reserved for ports/labels)
// ============================================================================

struct LMargin {
    double top = 0.0;
    double bottom = 0.0;
    double left = 0.0;
    double right = 0.0;

    LMargin() = default;
    LMargin(double all) : top(all), bottom(all), left(all), right(all) {}
    LMargin(double t, double b, double l, double r) : top(t), bottom(b), left(l), right(r) {}
};

// ============================================================================
// LPadding - Padding inside elements
// ============================================================================

struct LPadding {
    double top = 0.0;
    double bottom = 0.0;
    double left = 0.0;
    double right = 0.0;

    LPadding() = default;
    LPadding(double all) : top(all), bottom(all), left(all), right(all) {}
    LPadding(double t, double b, double l, double r) : top(t), bottom(b), left(l), right(r) {}
};

// ============================================================================
// LGraphElement - Base class for layered graph elements with properties
// ============================================================================

class LGraphElement : public PropertyHolder {
public:
    std::string id;

    virtual ~LGraphElement() = default;
    virtual std::string getDesignation() const { return id; }
};

// ============================================================================
// LShape - Base class for elements with position and size
// ============================================================================

class LShape : public LGraphElement {
public:
    Point position;  // KVector in Java
    Size size;       // KVector in Java

    Point getPosition() const { return position; }
    Size getSize() const { return size; }
    void setPosition(const Point& p) { position = p; }
    void setSize(const Size& s) { size = s; }
};

// ============================================================================
// LLabel - Label in layered graph
// ============================================================================

class LLabel : public LShape {
public:
    std::string text;

    LLabel() = default;
    explicit LLabel(const std::string& txt) : text(txt) {}

    std::string getText() const { return text; }
    void setText(const std::string& txt) { text = txt; }

    std::string getDesignation() const override {
        return text.empty() ? LGraphElement::getDesignation() : text;
    }
};

// ============================================================================
// NodeType - Type of node in layered graph
// ============================================================================

enum class NodeType {
    NORMAL,           // Normal node from original graph
    LONG_EDGE,        // Dummy node for long edge split
    EXTERNAL_PORT,    // Node representing external port
    NORTH_SOUTH_PORT, // Dummy for north/south port handling
    LABEL,            // Dummy for edge label
    BREAKING_POINT    // Dummy for graph wrapping
};

// ============================================================================
// LNode - Node in layered graph
// ============================================================================

class LNode : public LShape {
public:
    // Parent structures
    LGraph* graph = nullptr;
    Layer* layer = nullptr;

    // Node type
    NodeType type = NodeType::NORMAL;

    // Node content
    std::list<LPort*> ports;  // Use list for O(1) insert/remove
    std::vector<LLabel> labels;

    // Hierarchical support
    LGraph* nestedGraph = nullptr;

    // Layout spacing
    LMargin margin;
    LPadding padding;

    // Layer assignment
    int layerIndex = -1;  // Which layer
    int orderInLayer = -1; // Position within layer

    // Segment assignment (for LinearSegmentsNodePlacer)
    int segmentId = -1;  // LinearSegment ID this node belongs to

    // Edge priorities (for LinearSegmentsNodePlacer)
    int inputPriority = INT_MIN;   // Max priority of incoming edges
    int outputPriority = INT_MIN;  // Max priority of outgoing edges

    // Original graph element
    Node* originalNode = nullptr;

    // Cache for port side indices (optimization after PortListSorter)
    std::unordered_map<PortSide, std::pair<int, int>> portSideIndices;
    bool portSidesCached = false;

    LNode() = default;
    explicit LNode(LGraph* g) : graph(g) {}

    // Get layer assignment
    Layer* getLayer() const { return layer; }
    void setLayer(Layer* l);
    void setLayer(int index, Layer* l);

    // Get parent graph
    LGraph* getGraph() const;
    void setGraph(LGraph* g) { graph = g; }

    // Node type
    NodeType getType() const { return type; }
    void setType(NodeType t) { type = t; }

    // Port access
    std::list<LPort*>& getPorts() { return ports; }
    const std::list<LPort*>& getPorts() const { return ports; }

    // Get edges (through ports)
    std::vector<LEdge*> getIncomingEdges() const;
    std::vector<LEdge*> getOutgoingEdges() const;
    std::vector<LEdge*> getConnectedEdges() const;

    // Labels
    std::vector<LLabel>& getLabels() { return labels; }
    const std::vector<LLabel>& getLabels() const { return labels; }

    // Hierarchical
    LGraph* getNestedGraph() const { return nestedGraph; }
    void setNestedGraph(LGraph* ng) { nestedGraph = ng; }

    // Spacing
    LMargin& getMargin() { return margin; }
    const LMargin& getMargin() const { return margin; }
    LPadding& getPadding() { return padding; }
    const LPadding& getPadding() const { return padding; }

    // Index in layer
    int getIndex() const;

    // Port side caching
    void cachePortSides();
    std::vector<LPort*> getPortSideView(PortSide side);

    std::string getDesignation() const override;
};

// ============================================================================
// LPort - Port in layered graph
// ============================================================================

class LPort : public LShape {
public:
    // Parent node
    LNode* node = nullptr;

    // Port placement
    PortSide side = PortSide::UNDEFINED;
    Point anchor;  // Edge attachment point (relative to port position)
    bool explicitlySuppliedPortAnchor = false;

    // Port content
    LMargin margin;
    std::vector<LLabel> labels;

    // Connected edges (ports own edge connectivity in layered graph!)
    std::list<LEdge*> incomingEdges;  // Use list for O(1) insert/remove
    std::list<LEdge*> outgoingEdges;

    // Hierarchical support
    bool connectedToExternalNodes = true;

    // Original graph element
    Port* originalPort = nullptr;

    LPort() = default;

    // Owner node
    LNode* getNode() const { return node; }
    void setNode(LNode* n);

    // Port side
    PortSide getSide() const { return side; }
    void setSide(PortSide s);

    // Anchor point
    Point& getAnchor() { return anchor; }
    const Point& getAnchor() const { return anchor; }
    Point getAbsoluteAnchor() const;

    bool isExplicitlySuppliedPortAnchor() const { return explicitlySuppliedPortAnchor; }
    void setExplicitlySuppliedPortAnchor(bool fixed) { explicitlySuppliedPortAnchor = fixed; }

    // Margin & labels
    LMargin& getMargin() { return margin; }
    const LMargin& getMargin() const { return margin; }
    std::vector<LLabel>& getLabels() { return labels; }
    const std::vector<LLabel>& getLabels() const { return labels; }

    // Edge access
    std::list<LEdge*>& getIncomingEdges() { return incomingEdges; }
    const std::list<LEdge*>& getIncomingEdges() const { return incomingEdges; }
    std::list<LEdge*>& getOutgoingEdges() { return outgoingEdges; }
    const std::list<LEdge*>& getOutgoingEdges() const { return outgoingEdges; }

    // Degree & net flow
    int getDegree() const { return incomingEdges.size() + outgoingEdges.size(); }
    int getNetFlow() const { return incomingEdges.size() - outgoingEdges.size(); }

    // Index in node's port list
    int getIndex() const;

    std::string getDesignation() const override;
};

// ============================================================================
// LEdge - Edge in layered graph
// ============================================================================

class LEdge : public LGraphElement {
public:
    // Connected ports
    LPort* source = nullptr;
    LPort* target = nullptr;

    // Routing
    std::vector<Point> bendPoints;  // KVectorChain in Java

    // Labels
    std::vector<LLabel> labels;

    // Edge state
    bool reversed = false;  // If edge was reversed during cycle breaking

    // Original graph element
    Edge* originalEdge = nullptr;

    LEdge() = default;

    // Source/target
    LPort* getSource() const { return source; }
    void setSource(LPort* src);

    LPort* getTarget() const { return target; }
    void setTarget(LPort* tgt);
    void setTargetAndInsertAtIndex(LPort* tgt, int index);

    // Bend points
    std::vector<Point>& getBendPoints() { return bendPoints; }
    const std::vector<Point>& getBendPoints() const { return bendPoints; }

    // Labels
    std::vector<LLabel>& getLabels() { return labels; }
    const std::vector<LLabel>& getLabels() const { return labels; }

    // Edge properties
    bool isSelfLoop() const;
    bool isInLayerEdge() const;

    // Get other end
    LPort* getOther(LPort* port) const;
    LNode* getOther(LNode* node) const;

    // Reverse edge
    void reverse(LGraph* graph, bool adaptPorts);

    std::string getDesignation() const override;
};

// ============================================================================
// Layer - A layer of nodes in layered graph
// ============================================================================

class Layer {
public:
    LGraph* graph = nullptr;
    std::vector<LNode*> nodes;
    int index = -1;

    // Port extent tracking for proper spacing
    double maxPortExtentBelow = 0.0;  // Maximum port extension below nodes in this layer
    double maxPortExtentAbove = 0.0;  // Maximum port extension above nodes in this layer

    Layer() = default;
    explicit Layer(LGraph* g) : graph(g) {}

    std::vector<LNode*>& getNodes() { return nodes; }
    const std::vector<LNode*>& getNodes() const { return nodes; }

    LGraph* getGraph() const { return graph; }
    int getIndex() const { return index; }
};

// ============================================================================
// LinearSegment - Group of nodes for LinearSegmentsNodePlacer
// Port from: org.eclipse.elk.alg.layered.p4nodes.LinearSegmentsNodePlacer.LinearSegment
// ============================================================================

class LinearSegment {
public:
    std::vector<LNode*> nodes;           // Nodes in this segment
    int id = -1;                          // Segment identifier (also index in array)
    int indexInLastLayer = -1;            // Index in previous layer (for cycle avoidance)
    int lastLayer = -1;                   // Last layer where segment was seen
    double deflection = 0.0;              // Accumulated force (for balancing)
    int weight = 0;                       // Current weight
    LinearSegment* refSegment = nullptr;  // Reference segment (for region merging)
    NodeType nodeType = NodeType::NORMAL; // Type of nodes in segment

    // Get region representative
    LinearSegment* region() {
        LinearSegment* seg = this;
        while (seg->refSegment != nullptr) {
            seg = seg->refSegment;
        }
        return seg;
    }

    // Split segment before given node, return new segment with remaining nodes
    LinearSegment* split(LNode* node, int newId) {
        auto it = std::find(nodes.begin(), nodes.end(), node);
        if (it == nodes.end()) return nullptr;

        LinearSegment* newSegment = new LinearSegment();
        newSegment->id = newId;

        // Move nodes from 'node' onward to new segment
        for (auto moveIt = it; moveIt != nodes.end(); ++moveIt) {
            (*moveIt)->segmentId = newId;
            newSegment->nodes.push_back(*moveIt);
        }
        nodes.erase(it, nodes.end());

        return newSegment;
    }
};

// ============================================================================
// LGraph - The complete layered graph
// ============================================================================

class LGraph : public LGraphElement {
public:
    // Node and edge lists (owned)
    std::vector<std::unique_ptr<LNode>> lnodes;
    std::vector<std::unique_ptr<LEdge>> ledges;

    // Layers (owned)
    std::vector<std::unique_ptr<Layer>> layers;

    // Graph-level layout data
    LPadding padding;
    Point offset;
    Size size;  // Final graph size

    // Spacing values for edge routing
    double nodeNodeSpacing = 50.0;
    double edgeEdgeSpacing = 10.0;
    double edgeNodeSpacing = 10.0;

    // Random number generator for edge routing
    std::mt19937 random;

    // Parent graph (for hierarchical layout)
    LGraph* parentGraph = nullptr;
    LNode* parentNode = nullptr;

    LGraph() : random(42) {}  // Default seed

    // Add elements
    LNode* addNode();
    LEdge* addEdge();
    Layer* addLayer();
    Layer* addLayer(int index);

    // Access
    std::vector<std::unique_ptr<LNode>>& getLayerlessNodes() { return lnodes; }
    std::vector<std::unique_ptr<Layer>>& getLayers() { return layers; }

    LPadding& getPadding() { return padding; }
    const LPadding& getPadding() const { return padding; }

    Point& getOffset() { return offset; }
    const Point& getOffset() const { return offset; }
};

} // namespace layered
} // namespace elk
