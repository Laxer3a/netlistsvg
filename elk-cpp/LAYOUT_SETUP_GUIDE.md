# ELK C++ Layout Setup Guide

This guide covers all layout-related configuration for elk-cpp. It does **not** cover graph connectivity (adding nodes/edges), only layout parameters and properties.

## Table of Contents
- [Basic Layout Setup](#basic-layout-setup)
- [Layout Direction](#layout-direction)
- [Spacing Configuration](#spacing-configuration)
- [Port Configuration](#port-configuration)
- [Layer Constraints](#layer-constraints)
- [Port Constraints](#port-constraints)
- [Complete Example](#complete-example)

---

## Basic Layout Setup

### 1. Create Layout Provider

```cpp
#include <elk/alg/layered/layered_layout.h>

elk::layered::LayeredLayoutProvider layout;
```

### 2. Run Layout

```cpp
// After creating your graph and all nodes/edges/ports
layout.layout(root.get(), nullptr);
```

The `layout()` function:
- First parameter: pointer to root `Node`
- Second parameter: optional `IElkProgressMonitor*` (use `nullptr` if not needed)

After calling `layout()`, all node positions and edge routing are calculated and stored in the graph.

---

## Layout Direction

Controls the flow direction of the layered layout.

```cpp
#include <elk/core/types.h>

// Set direction before calling layout()
layout.setDirection(Direction::RIGHT);  // Left-to-right (default)
layout.setDirection(Direction::DOWN);   // Top-to-bottom
layout.setDirection(Direction::LEFT);   // Right-to-left
layout.setDirection(Direction::UP);     // Bottom-to-top
```

**Available Directions:**
- `Direction::RIGHT` - Layers flow left-to-right (inputs on left, outputs on right)
- `Direction::DOWN` - Layers flow top-to-bottom
- `Direction::LEFT` - Layers flow right-to-left
- `Direction::UP` - Layers flow bottom-to-top

---

## Spacing Configuration

### Node Spacing

Space between nodes **within the same layer** (vertical spacing for RIGHT direction).

```cpp
layout.setNodeSpacing(35.0);  // pixels
```

**Default:** 50.0px
**Recommended range:** 20-50px

### Layer Spacing

Space **between layers** (horizontal spacing for RIGHT direction).

```cpp
layout.setLayerSpacing(35.0);  // pixels
```

**Default:** 50.0px
**Recommended range:** 35-80px

### Edge Spacing

Spacing for edge routing (affects orthogonal edge router).

```cpp
// These are currently configured as graph properties during import
// Default values:
// - nodeNodeSpacing: 50.0
// - edgeEdgeSpacing: 10.0
// - edgeNodeSpacing: 10.0
```

**Note:** Edge spacing is read from the internal layered graph and is set during import. Future API may expose these directly on the layout provider.

---

## Port Configuration

Ports are the connection points on nodes where edges attach.

### Port Side

Specify which side of the node the port is on:

```cpp
#include <elk/core/types.h>

auto port = node->addPort("port_name", PortSide::EAST);
```

**Available Sides:**
- `PortSide::NORTH` - Top of node
- `PortSide::SOUTH` - Bottom of node
- `PortSide::EAST` - Right side of node
- `PortSide::WEST` - Left side of node
- `PortSide::UNDEFINED` - Let layout algorithm decide

### Port Position

Set the exact (x, y) position of the port **relative to the node's top-left corner**:

```cpp
auto port = node->addPort("output", PortSide::EAST);
port->position = Point(30, 12.5);  // x=30px (right edge), y=12.5px (middle)
```

**Examples:**
```cpp
// Node size: 30x25
auto node = root->addChild("mynode");
node->size = Size(30, 25);

// Port on right edge, vertically centered
auto out_port = node->addPort("out", PortSide::EAST);
out_port->position = Point(30, 12.5);  // width=30, height/2=12.5

// Port on left edge, vertically centered
auto in_port = node->addPort("in", PortSide::WEST);
in_port->position = Point(0, 12.5);

// Port on top edge, horizontally centered
auto top_port = node->addPort("clk", PortSide::NORTH);
top_port->position = Point(15, 0);  // width/2=15, y=0
```

### Port Anchors (Optional)

For fine-grained control over edge attachment points:

```cpp
port->anchor = Point(0, 0);  // Offset from port position
port->explicitlySuppliedPortAnchor = true;
```

**Note:** Most use cases only need port position, not anchors.

---

## Layer Constraints

Force specific nodes to specific layers (e.g., all inputs on left, all outputs on right).

### Setting Layer Constraints

```cpp
// Force node to FIRST layer (leftmost for Direction::RIGHT)
node->setProperty("org.eclipse.elk.layered.layering.layerConstraint",
                  std::string("FIRST"));

// Force node to LAST layer (rightmost for Direction::RIGHT)
node->setProperty("org.eclipse.elk.layered.layering.layerConstraint",
                  std::string("LAST"));
```

**Available Constraints:**

| Constraint | Description |
|------------|-------------|
| `"FIRST"` | Force to first layer (leftmost column for RIGHT direction) |
| `"LAST"` | Force to last layer (rightmost column for RIGHT direction) |
| `"FIRST_SEPARATE"` | Force to first layer in separate rank |
| `"LAST_SEPARATE"` | Force to last layer in separate rank |

### Example: Input/Output Pinning

```cpp
// Create input nodes
auto clock = root->addChild("clock");
clock->size = Size(30, 20);
clock->setProperty("org.eclipse.elk.layered.layering.layerConstraint",
                   std::string("FIRST"));
auto clock_out = clock->addPort("Y", PortSide::EAST);
clock_out->position = Point(30, 10);

// Create output nodes
auto result = root->addChild("result");
result->size = Size(30, 20);
result->setProperty("org.eclipse.elk.layered.layering.layerConstraint",
                    std::string("LAST"));
auto result_in = result->addPort("A", PortSide::WEST);
result_in->position = Point(0, 10);
```

**Result:** All inputs will be in the leftmost layer, all outputs in the rightmost layer.

---

## Port Constraints

Control whether the layout algorithm can move ports or must respect their positions.

### Fixed Port Positions

Tell ELK to **not move ports** and use their exact positions:

```cpp
node->setProperty("org.eclipse.elk.portConstraints",
                  std::string("FIXED_POS"));
```

**Port Constraint Options:**

| Value | Description |
|-------|-------------|
| `"FREE"` | Layout can place ports anywhere (default) |
| `"FIXED_SIDE"` | Layout respects port side but can change order |
| `"FIXED_ORDER"` | Layout respects port order on each side |
| `"FIXED_POS"` | Layout must respect exact port positions |

### Example: Node with Fixed Ports

```cpp
auto processor = root->addChild("proc");
processor->size = Size(40, 30);
processor->setProperty("org.eclipse.elk.portConstraints",
                       std::string("FIXED_POS"));

// These exact positions will be preserved
auto in1 = processor->addPort("in1", PortSide::WEST);
in1->position = Point(0, 10);

auto in2 = processor->addPort("in2", PortSide::WEST);
in2->position = Point(0, 20);

auto out = processor->addPort("out", PortSide::EAST);
out->position = Point(40, 15);
```

---

## Complete Example

Here's a full example showing all layout setup options:

```cpp
#include <elk/graph/graph.h>
#include <elk/alg/layered/layered_layout.h>
#include <memory>

using namespace elk;

int main() {
    // 1. Create root graph
    auto root = std::make_unique<Node>("my_circuit");

    // 2. Create input node (pinned to first layer)
    auto input = root->addChild("input");
    input->size = Size(30, 20);
    input->setProperty("org.eclipse.elk.layered.layering.layerConstraint",
                       std::string("FIRST"));
    input->setProperty("org.eclipse.elk.portConstraints",
                       std::string("FIXED_POS"));
    auto input_out = input->addPort("out", PortSide::EAST);
    input_out->position = Point(30, 10);

    // 3. Create processing node
    auto processor = root->addChild("processor");
    processor->size = Size(40, 30);
    processor->setProperty("org.eclipse.elk.portConstraints",
                           std::string("FIXED_POS"));
    auto proc_in = processor->addPort("in", PortSide::WEST);
    proc_in->position = Point(0, 15);
    auto proc_out = processor->addPort("out", PortSide::EAST);
    proc_out->position = Point(40, 15);

    // 4. Create output node (pinned to last layer)
    auto output = root->addChild("output");
    output->size = Size(30, 20);
    output->setProperty("org.eclipse.elk.layered.layering.layerConstraint",
                        std::string("LAST"));
    output->setProperty("org.eclipse.elk.portConstraints",
                        std::string("FIXED_POS"));
    auto output_in = output->addPort("in", PortSide::WEST);
    output_in->position = Point(0, 10);

    // 5. Create edges (connectivity - see separate guide)
    auto e1 = root->addEdge("e1");
    e1->sourcePorts.push_back(input_out);
    e1->targetPorts.push_back(proc_in);
    input_out->outgoingEdges.push_back(e1);
    proc_in->incomingEdges.push_back(e1);

    auto e2 = root->addEdge("e2");
    e2->sourcePorts.push_back(proc_out);
    e2->targetPorts.push_back(output_in);
    proc_out->outgoingEdges.push_back(e2);
    output_in->incomingEdges.push_back(e2);

    // 6. Configure layout
    layered::LayeredLayoutProvider layout;
    layout.setDirection(Direction::RIGHT);    // Left-to-right flow
    layout.setNodeSpacing(35.0);             // 35px between nodes in layer
    layout.setLayerSpacing(80.0);            // 80px between layers

    // 7. Run layout
    layout.layout(root.get(), nullptr);

    // 8. Results are now in the graph
    // Access positioned nodes:
    for (const auto& child : root->children) {
        // child->position contains final (x, y)
        // child->size contains final (width, height)
    }

    // Access routed edges:
    for (const auto& edge : root->edges) {
        if (!edge->sections.empty()) {
            const auto& section = edge->sections[0];
            // section.startPoint - edge start coordinates
            // section.bendPoints - vector of bend points (for orthogonal routing)
            // section.endPoint - edge end coordinates
        }
    }

    return 0;
}
```

---

## Properties API

### Setting Properties on Nodes

```cpp
node->setProperty(key, value);
```

**Property Types:**
- `std::string` for string properties
- `double` for numeric properties
- Custom types (see core/properties.h)

### Common ELK Properties

| Property Key | Type | Description |
|--------------|------|-------------|
| `"org.eclipse.elk.layered.layering.layerConstraint"` | `std::string` | Layer constraint (FIRST, LAST, etc.) |
| `"org.eclipse.elk.portConstraints"` | `std::string` | Port constraint (FREE, FIXED_POS, etc.) |
| `"org.eclipse.elk.nodeSize.constraints"` | `std::string` | Node sizing constraints |
| `"org.eclipse.elk.nodeSize.minimum"` | Custom | Minimum node size |

### Reading Properties

```cpp
if (node->hasProperty("org.eclipse.elk.portConstraints")) {
    std::string constraint = node->getProperty<std::string>("org.eclipse.elk.portConstraints");
}
```

---

## Best Practices

### 1. **Always Set Port Sides**
```cpp
// Good
auto port = node->addPort("out", PortSide::EAST);

// Avoid
auto port = node->addPort("out", PortSide::UNDEFINED);
```

### 2. **Use FIXED_POS for Predictable Layouts**
```cpp
node->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));
```

### 3. **Set Consistent Spacing**
```cpp
// Use same spacing as your design system
layout.setNodeSpacing(35.0);
layout.setLayerSpacing(35.0);
```

### 4. **Pin Input/Output Nodes**
```cpp
// Inputs to first layer
input_node->setProperty("org.eclipse.elk.layered.layering.layerConstraint",
                        std::string("FIRST"));

// Outputs to last layer
output_node->setProperty("org.eclipse.elk.layered.layering.layerConstraint",
                         std::string("LAST"));
```

### 5. **Calculate Port Positions Relative to Node Size**
```cpp
auto node = root->addChild("mynode");
node->size = Size(width, height);

// Right edge, centered
auto out = node->addPort("out", PortSide::EAST);
out->position = Point(width, height / 2.0);

// Left edge, centered
auto in = node->addPort("in", PortSide::WEST);
in->position = Point(0, height / 2.0);
```

---

## Troubleshooting

### Ports Don't Appear at Expected Positions

**Problem:** Port positions are being changed by layout algorithm.

**Solution:** Set `FIXED_POS` constraint:
```cpp
node->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));
```

### Nodes Not Pinned to First/Last Layer

**Problem:** Layer constraints not being applied.

**Solution:** Ensure property is set **before** calling `layout()`:
```cpp
node->setProperty("org.eclipse.elk.layered.layering.layerConstraint",
                  std::string("FIRST"));
layout.layout(root.get(), nullptr);  // Must be after setProperty
```

### Edges Not Routed Orthogonally

**Problem:** Edges are straight lines instead of 90-degree paths.

**Solution:** OrthogonalEdgeRouter is automatically used. Check that:
1. Layout was run successfully
2. Edge sections are populated: `!edge->sections.empty()`
3. Bend points exist: `!edge->sections[0].bendPoints.empty()`

### Spacing Too Tight/Loose

**Problem:** Nodes are too close or too far apart.

**Solution:** Adjust spacing parameters:
```cpp
layout.setNodeSpacing(20.0);   // Decrease for tighter layout
layout.setLayerSpacing(100.0); // Increase for more horizontal space
```

---

## See Also

- **API_USAGE.md** - General API usage and graph construction
- **IMPLEMENTATION_STATUS.md** - Feature support status
- **examples/svg_output_test.cpp** - Simple layout example
- **examples/up3down5_test.cpp** - Complex layout with constraints

---

## Quick Reference Card

```cpp
// === LAYOUT PROVIDER ===
layered::LayeredLayoutProvider layout;
layout.setDirection(Direction::RIGHT);
layout.setNodeSpacing(35.0);
layout.setLayerSpacing(80.0);
layout.layout(root.get(), nullptr);

// === NODE SETUP ===
auto node = root->addChild("node_id");
node->size = Size(width, height);
node->setProperty("org.eclipse.elk.layered.layering.layerConstraint",
                  std::string("FIRST"));  // or "LAST"
node->setProperty("org.eclipse.elk.portConstraints",
                  std::string("FIXED_POS"));

// === PORT SETUP ===
auto port = node->addPort("port_id", PortSide::EAST);  // or WEST/NORTH/SOUTH
port->position = Point(x, y);  // Relative to node's top-left

// === EDGE ROUTING (after layout) ===
for (const auto& edge : root->edges) {
    if (!edge->sections.empty()) {
        auto& section = edge->sections[0];
        Point start = section.startPoint;
        std::vector<Point> bends = section.bendPoints;
        Point end = section.endPoint;
    }
}
```
