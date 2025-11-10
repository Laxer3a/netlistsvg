# ELK LinearSegmentsNodePlacer Porting Plan

## Current Status

### What Works (Simple Algorithm)
The C++ implementation uses a simplified layer-by-layer placement:
- ✓ Correctly handles port extents (spacing between port edges)
- ✓ Works for simple chains and DAGs (2-5 nodes)
- ✓ Direction handling (RIGHT, LEFT, DOWN, UP)
- ✓ Graph padding (12px)

### What's Missing (For 100% Compliance)
The Java ELK uses **LinearSegmentsNodePlacer** (883 lines) with sophisticated features:
- ✗ Linear segments (groups of dummy nodes for long edges)
- ✗ Dependency graph with topological sort for segment ordering
- ✗ Sophisticated spacing calculation via `Spacings.getVerticalSpacing()`
- ✗ Node margins calculated from port/label extents
- ✗ Balancing with pendulum/rubber iterations
- ✗ Post-processing optimizations

## Java LinearSegmentsNodePlacer Algorithm

### Phase 1: Sort Linear Segments (lines 216-314)

**Purpose**: Create and topologically sort linear segments

**Key Methods**:
```java
private LinearSegment[] sortLinearSegments(LGraph layeredGraph) {
    // 1. Calculate edge priorities (INPUT_PRIO, OUTPUT_PRIO)
    // 2. Create linear segments via fillSegment()
    // 3. Build dependency graph between segments
    // 4. Topological sort to determine placement order
    // 5. Return sorted array of segments
}
```

**Linear Segment Creation** (`fillSegment`, line 461):
- Groups nodes connected by long edges into segments
- Handles LONG_EDGE and NORTH_SOUTH_PORT dummy types
- Recursive traversal to extend segments

**Dependency Graph** (`createDependencyGraphEdges`, line 332):
- Creates ordering constraints between segments
- Splits segments to avoid cycles
- Tracks segment indices across layers

### Phase 2: Create Unbalanced Placement (lines 516-559)

**Purpose**: Initial node positioning based on segment order

**Algorithm**:
```java
for (LinearSegment segment : linearSegments) {
    double uppermostPlace = 0.0;
    for (LNode node : segment.nodes) {
        // Get spacing from previous node in this layer
        spacing = spacings.getVerticalSpacing(recentNode[layer], node);
        uppermostPlace = max(uppermostPlace, layer.size + spacing);
    }

    // Place all nodes in segment at uppermostPlace
    for (LNode node : segment.nodes) {
        node.y = uppermostPlace + node.margin.top;
        layer.size = uppermostPlace + node.margin.top + node.size + node.margin.bottom;
    }
}
```

**Key Difference**: Uses `node.margin` (calculated from ports/labels) and sophisticated spacing

### Phase 3: Balance Placement (lines 590-741)

**Purpose**: Optimize node positions using force-directed approach

**Modes**:
1. **FORW_PENDULUM**: Only incoming edges considered
2. **BACKW_PENDULUM**: Only outgoing edges considered
3. **RUBBER**: Both directions considered

**Algorithm** (simplified):
```java
do {
    // Calculate deflection (optimal movement) for each segment
    for (LinearSegment segment : linearSegments) {
        calcDeflection(segment, incoming, outgoing);
    }

    // Merge regions that touch
    mergeRegions(linearSegments);

    // Apply deflections
    for (LinearSegment segment : linearSegments) {
        applyDeflection(segment);
    }

    // Check convergence
} while (!converged && iterations < MAX);
```

### Phase 4: Post-Process (lines 743-883)

**Purpose**: Final adjustments

- Straighten edges
- Remove overlaps
- Adjust for labels

## Critical Dependencies

### 1. Spacings Class (LayeredOptions/Spacings.java)

**Method**: `getVerticalSpacing(LNode n1, LNode n2)`

**Logic**:
```java
NodeType t1 = n1.getType();
NodeType t2 = n2.getType();
IProperty<Double> layoutOption = nodeTypeSpacingOptionsVertical[t1][t2];

// Get spacing for each node (can have individual overrides)
Double s1 = getIndividualOrDefault(n1, layoutOption);
Double s2 = getIndividualOrDefault(n2, layoutOption);

return Math.max(s1, s2);  // Use maximum of the two
```

**Key Point**: Different node types (NORMAL, LONG_EDGE, NORTH_SOUTH_PORT, etc.) have different spacing rules

### 2. Node Margins (LNode.getMargin())

**Calculated by**: `InnermostNodeMarginCalculator`

**Purpose**: Reserve space around node for:
- Ports extending beyond node bounds
- Port labels
- Self-loops (added later)
- Comment boxes (added later)

**Algorithm** (from elk-core NodeDimensionCalculation):
```java
for (LPort port : node.getPorts()) {
    // Calculate how far port extends beyond node
    double portOffset = calculatePortOffset(port);

    // Update margin based on port side
    switch (port.side) {
        case NORTH: margin.top = max(margin.top, portOffset); break;
        case SOUTH: margin.bottom = max(margin.bottom, portOffset); break;
        case WEST: margin.left = max(margin.left, portOffset); break;
        case EAST: margin.right = max(margin.right, portOffset); break;
    }
}
```

## Porting Strategy

### Approach 1: Full Port (Recommended for 100% Compliance)

1. **Port data structures** (~100 lines)
   - LinearSegment class with all fields
   - LMargin structure
   - Enhanced LNode with margin support

2. **Port segment creation** (~200 lines)
   - fillSegment method
   - Dependency graph creation
   - Topological sort

3. **Port Spacings calculation** (~150 lines)
   - Node type spacing matrix
   - getVerticalSpacing implementation
   - Individual property override support

4. **Port margin calculation** (~100 lines)
   - InnermostNodeMarginCalculator
   - Port offset calculation

5. **Port unbalanced placement** (~50 lines)
   - Using margins and proper spacing

6. **Port balancing** (~300 lines)
   - calcDeflection
   - Region merging
   - Pendulum/rubber iterations

7. **Port post-processing** (~100 lines)
   - Final adjustments

**Estimated Effort**: 1000+ lines of C++ code, 2-3 days of careful porting

### Approach 2: Incremental (Pragmatic)

1. **Phase 1**: Port just the spacing calculation (~200 lines)
   - Will fix most positioning issues
   - Keep simple linear placement for now

2. **Phase 2**: Add node margins (~150 lines)
   - Will fix port extent issues for complex graphs

3. **Phase 3**: Add linear segments if needed (~400 lines)
   - Only if graphs with many long edges fail

4. **Phase 4**: Add balancing if needed (~300 lines)
   - Only if position optimization is critical

## Testing Plan

### Incremental Testing

After each phase:
1. Test with `up3down5.json` (31 nodes, 40 edges)
2. Compare node positions with JavaScript output
3. Document remaining discrepancies
4. Proceed to next phase if discrepancies > tolerance

### Acceptance Criteria for 100% Compliance

For each test file in `test/digital/` and `test/analog/`:
- Node positions match within 0.1 pixels
- Graph dimensions match exactly
- Edge routing matches (if implemented)

## Recommendation

For **100% compliance with netlistsvg**, I recommend:

1. **Start with Approach 2, Phase 1**: Port the spacing calculation
   - This will likely fix 80% of positioning issues
   - Can verify with real circuits quickly

2. **Evaluate results**: Run all test files and measure discrepancy
   - If < 5% of positions differ significantly, good enough for many use cases
   - If > 5%, continue with more phases

3. **Full port only if required**: If exact match is mandatory for production
   - Allocate 2-3 days for careful porting
   - Extensive testing at each step

Would you like me to proceed with Approach 2, Phase 1 (spacing calculation)?
