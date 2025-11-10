# ELK C++ API Usage Guide

## Overview

The ELK C++ library provides a graph layout API similar to the JavaScript elkjs library. Currently, input graphs are constructed programmatically in C++ (JSON parsing not yet implemented).

## Sample Files Location

### C++ Examples
All example files are in **`elk-cpp/examples/`**:

```
elk-cpp/examples/
├── simple_example.cpp          # Basic 2-node graph
├── complex_test.cpp            # Diamond graph + port extent tests
├── multilayer_test.cpp         # 8-node, 3-layer graph
├── svg_output_test.cpp         # Same as multilayer but generates SVG
├── netlistsvg_test.cpp         # Netlistsvg-style graph
└── json_test.cpp               # JSON parsing (not yet working)
```

### Compiled Binaries
After building with `cmake -DBUILD_EXAMPLES=ON`, binaries are in **`elk-cpp/build/`**:
- `./simple_example`
- `./complex_test`
- `./multilayer_test`
- `./svg_output_test`

### JavaScript Test Files
For comparison, JavaScript examples are in the repository root:
- `run_elk_direct.js` - Runs elkjs directly
- `run_elk_debug.js` - Runs elkjs with debug output
- `generate_js_svg.js` - Generates SVG from elkjs

## How to Use the API

### Method 1: Programmatic Graph Construction (Currently Used)

This is the **current method** - building graphs directly in C++ code.

#### Basic Example (simple_example.cpp)

```cpp
#include <elk/graph/graph.h>
#include <elk/alg/layered/layered_layout.h>

using namespace elk;

int main() {
    // Create root graph
    auto root = std::make_unique<Node>("root");

    // Add nodes
    auto n0 = root->addChild("n0");
    n0->size = Size(30, 25);

    auto n1 = root->addChild("n1");
    n1->size = Size(30, 25);

    // Add ports to nodes
    auto n0_out = n0->addPort("out", PortSide::EAST);
    n0_out->position = Point(30, 12.5);

    auto n1_in = n1->addPort("in", PortSide::WEST);
    n1_in->position = Point(0, 12.5);

    // Create edge connecting ports
    auto edge = root->addEdge("e0");
    edge->sourcePorts.push_back(n0_out);
    edge->targetPorts.push_back(n1_in);

    // Set up bidirectional references
    n0_out->outgoingEdges.push_back(edge);
    n1_in->incomingEdges.push_back(edge);

    // Create and configure layout algorithm
    layered::LayeredLayoutProvider layout;
    layout.setDirection(Direction::RIGHT);
    layout.setNodeSpacing(35.0);
    layout.setLayerSpacing(80.0);

    // Run layout!
    layout.layout(root.get(), nullptr);

    // Access results
    std::cout << "Node positions:\n";
    for (const auto& child : root->children) {
        std::cout << "  " << child->id << ": ("
                  << child->position.x << ", "
                  << child->position.y << ")\n";
    }

    return 0;
}
```

#### Running the Example

```bash
cd elk-cpp/build
cmake .. -DBUILD_EXAMPLES=ON
make simple_example
./simple_example
```

**Output:**
```
Node positions:
  n0: (12, 47)
  n1: (122, 47)
```

### Method 2: Using JSON Input (Not Yet Implemented)

**Status:** The JSON parsing code exists (`json_test.cpp`) but is **not yet functional**. It would work like this:

#### Planned API (Future)

```cpp
#include <elk/graph/json_parser.h>
#include <elk/alg/layered/layered_layout.h>
#include <fstream>

using namespace elk;

int main() {
    // Read JSON file
    std::ifstream file("graph.json");
    std::string json((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());

    // Parse JSON to graph (NOT YET WORKING)
    auto root = JsonParser::parse(json);

    // Run layout
    layered::LayeredLayoutProvider layout;
    layout.layout(root.get(), nullptr);

    return 0;
}
```

#### JSON Format (elkjs Compatible)

```json
{
  "id": "root",
  "layoutOptions": {
    "elk.algorithm": "layered",
    "elk.direction": "RIGHT",
    "elk.spacing.nodeNode": "35.0",
    "elk.layered.spacing.nodeNodeBetweenLayers": "80.0"
  },
  "children": [
    { "id": "n0", "width": 30, "height": 25 },
    { "id": "n1", "width": 30, "height": 25 }
  ],
  "edges": [
    { "id": "e0", "sources": ["n0"], "targets": ["n1"] }
  ]
}
```

**Why Not Working Yet:**
- No JSON parsing library integrated
- Port creation from JSON not implemented
- Edge-to-port mapping logic incomplete

## Current Test Approach: Hardcoded Graphs

All current tests use **hardcoded graph construction** in C++. Here are the main test patterns:

### Test 1: Simple Chain (complex_test.cpp)

```cpp
// Create 4 nodes in a chain: n0 → n1 → n2 → n3
for (int i = 0; i < 4; i++) {
    auto node = root->addChild("n" + std::to_string(i));
    node->size = Size(30, 25);

    auto inPort = node->addPort("in", PortSide::WEST);
    auto outPort = node->addPort("out", PortSide::EAST);
}

// Connect them
for (int i = 0; i < 3; i++) {
    auto edge = root->addEdge("e" + std::to_string(i));
    edge->sourcePorts.push_back(nodes[i]->ports[1].get());
    edge->targetPorts.push_back(nodes[i+1]->ports[0].get());
}
```

### Test 2: Diamond DAG (complex_test.cpp)

```cpp
// Diamond: n0 → n1, n0 → n2, n1 → n3, n2 → n3
auto n0 = root->addChild("n0");
auto n1 = root->addChild("n1");
auto n2 = root->addChild("n2");
auto n3 = root->addChild("n3");

// n0 has 1 output port, n1 and n2 have input+output, n3 has 1 input
auto e0 = root->addEdge("e0");
e0->sourcePorts.push_back(n0_out);
e0->targetPorts.push_back(n1_in);

// ... etc for all 4 edges
```

### Test 3: Multi-layer Graph (multilayer_test.cpp)

8 nodes across 3 layers with fan-out and fan-in pattern:
```
Layer 0: s0, s1 (sources)
Layer 1: p0, p1, p2, p3 (processing)
Layer 2: t0, t1 (sinks)

Edges: s0→{p0,p1}, s1→{p2,p3}, {p0,p1}→t0, {p2,p3}→t1
```

See `multilayer_test.cpp` for full code (~140 lines of setup).

## API Reference

### Core Classes

#### `elk::Node`
Represents a graph node.

**Methods:**
```cpp
Node* addChild(const std::string& id);
Port* addPort(const std::string& id, PortSide side);
Edge* addEdge(const std::string& id);
void setProperty(const std::string& key, const std::string& value);
```

**Fields:**
```cpp
std::string id;
Size size;                    // width, height
Point position;               // x, y (set by layout)
std::vector<std::unique_ptr<Node>> children;
std::vector<std::unique_ptr<Port>> ports;
std::vector<std::unique_ptr<Edge>> edges;
```

#### `elk::Port`
Represents a node port (connection point).

**Fields:**
```cpp
std::string id;
PortSide side;               // NORTH, SOUTH, EAST, WEST
Point position;              // Relative to node
Node* parent;                // Parent node
std::vector<Edge*> incomingEdges;
std::vector<Edge*> outgoingEdges;
```

#### `elk::Edge`
Represents a connection between ports.

**Fields:**
```cpp
std::string id;
std::vector<Port*> sourcePorts;
std::vector<Port*> targetPorts;
```

#### `elk::layered::LayeredLayoutProvider`
The layout algorithm implementation.

**Configuration Methods:**
```cpp
void setDirection(Direction dir);
  // Direction::RIGHT (default), LEFT, DOWN, UP

void setNodeSpacing(double spacing);
  // Space between nodes in same layer (default: 20.0)

void setLayerSpacing(double spacing);
  // Space between layers (default: 50.0)

void setCrossingMinimization(CrossingMinimizationStrategy strategy);
  // CrossingMinimizationStrategy::LAYER_SWEEP (default)

void setNodePlacement(NodePlacementStrategy strategy);
  // NodePlacementStrategy::LINEAR_SEGMENTS (default)
```

**Layout Method:**
```cpp
void layout(Node* graph, ProgressCallback progress = nullptr);
```

### Complete Working Example

See **`svg_output_test.cpp`** for a complete example that:
1. Creates an 8-node graph
2. Runs layout
3. Generates SVG output
4. Shows how to access all results

## Comparison: C++ vs JavaScript API

### JavaScript (elkjs)

```javascript
const ELK = require('elkjs');
const elk = new ELK();

const graph = {
  id: "root",
  children: [
    { id: "n0", width: 30, height: 25 },
    { id: "n1", width: 30, height: 25 }
  ],
  edges: [
    { id: "e0", sources: ["n0"], targets: ["n1"] }
  ]
};

elk.layout(graph).then(g => {
  console.log(g.children[0].x, g.children[0].y);
});
```

### C++ (current)

```cpp
auto root = std::make_unique<Node>("root");

auto n0 = root->addChild("n0");
n0->size = Size(30, 25);
auto n1 = root->addChild("n1");
n1->size = Size(30, 25);

// Must create ports explicitly
auto n0_out = n0->addPort("out", PortSide::EAST);
auto n1_in = n1->addPort("in", PortSide::WEST);

// Must create edge and wire up ports
auto edge = root->addEdge("e0");
edge->sourcePorts.push_back(n0_out);
edge->targetPorts.push_back(n1_in);
n0_out->outgoingEdges.push_back(edge);
n1_in->incomingEdges.push_back(edge);

layered::LayeredLayoutProvider layout;
layout.layout(root.get(), nullptr);

std::cout << n0->position.x << ", " << n0->position.y;
```

**Key Differences:**
1. ❌ C++ requires explicit port creation (JavaScript auto-generates)
2. ❌ C++ requires manual edge-port wiring
3. ❌ C++ does not support JSON input yet
4. ✅ C++ gives direct access to internal data structures
5. ✅ C++ is synchronous (no promises/callbacks)

## Running Tests

### Build All Examples

```bash
cd elk-cpp
mkdir -p build && cd build
cmake .. -DBUILD_EXAMPLES=ON
make
```

### Run Individual Tests

```bash
# Simple 2-node test
./simple_example

# Diamond + port extents
./complex_test

# 8-node multilayer
./multilayer_test

# Generate SVG output
./svg_output_test
ls -lh /tmp/cpp_layout.svg
```

### Compare with JavaScript

```bash
cd /home/user/netlistsvg

# Run C++ test
elk-cpp/build/svg_output_test

# Run JavaScript test
node generate_js_svg.js

# View comparison
firefox /tmp/comparison.html  # or any browser
```

## Logging and Debugging

The C++ implementation includes extensive logging to stderr:

```bash
./multilayer_test 2>&1 | less
```

**Log Sections:**
- `=== IMPORT GRAPH ===` - Graph import statistics
- `=== LONGEST PATH LAYERING ===` - Topological order and layer assignment
- `=== LAYER ASSIGNMENT ===` - Nodes per layer
- `=== MINIMIZE CROSSINGS ===` - Barycenter calculations, node reordering
- `=== SORT LINEAR SEGMENTS ===` - Segment creation for LinearSegmentsNodePlacer
- `=== CREATE UNBALANCED PLACEMENT ===` - Y-coordinate assignment
- `=== APPLY LAYOUT ===` - Final positions copied back to original nodes

## Output Formats

### 1. Console Output
All examples print node positions to stdout:
```
Node positions:
  s0: (12, 107)
  s1: (12, 47)
  ...
```

### 2. SVG Output
`svg_output_test.cpp` generates `/tmp/cpp_layout.svg`:
- Nodes as colored rectangles with labels
- Edges as lines with directional arrows
- Can be opened in any browser or SVG viewer

### 3. Programmatic Access
Access layout results directly:

```cpp
layout.layout(root.get(), nullptr);

for (const auto& child : root->children) {
    double x = child->position.x;
    double y = child->position.y;
    double w = child->size.width;
    double h = child->size.height;

    // Use positions for rendering, export, etc.
}
```

## Future Work: JSON Support

To add JSON input support, the following would be needed:

1. **Integrate JSON library** (e.g., nlohmann/json)
   ```cmake
   find_package(nlohmann_json REQUIRED)
   target_link_libraries(elk nlohmann_json::nlohmann_json)
   ```

2. **Implement parser** (`src/graph/json_parser.cpp`)
   - Parse nodes from `children` array
   - Auto-generate ports based on `edges`
   - Map edge sources/targets to ports

3. **Port creation heuristic**
   - For each edge, create ports if they don't exist
   - Assign port sides based on edge direction
   - Use netlistsvg conventions (EAST for outputs, WEST for inputs)

4. **Update examples**
   - Add `json_input_test.cpp` that actually works
   - Document JSON format compatibility with elkjs

This would make the C++ API match the JavaScript elkjs API more closely.

## Summary

| Feature | Status | Method |
|---------|--------|--------|
| Programmatic graph construction | ✅ Working | All example files |
| Layered layout algorithm | ✅ Working | LayeredLayoutProvider |
| LinearSegmentsNodePlacer | ✅ Working | Faithful Java port |
| Crossing minimization | ✅ Working | Barycenter + stable sort |
| SVG output | ✅ Working | svg_output_test.cpp |
| JSON input | ❌ Not yet | json_test.cpp (stub) |
| JavaScript API compatibility | ⚠️ Partial | Algorithm matches, API differs |

**Recommended Starting Point:** `multilayer_test.cpp` or `svg_output_test.cpp` - shows complete workflow from graph construction to output.
