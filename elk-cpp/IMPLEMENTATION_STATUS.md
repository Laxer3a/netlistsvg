# ELK C++ Implementation Status

## Summary

The C++ ELK port successfully handles **simple graphs** (2-5 nodes) with exact match to JavaScript elkjs. However, **complex netlists** like `up3down5.json` (31 nodes, 40 edges) require the full LinearSegmentsNodePlacer algorithm (883 lines).

## What's Implemented ✓

### Core Graph Model
- `Node`, `Edge`, `Port`, `Label` classes
- Property system
- Hierarchical graph support
- Direction handling (RIGHT, LEFT, DOWN, UP)

### Layered Layout (Simplified)
- Cycle breaking (greedy reversal)
- Layer assignment (longest path)
- Simple linear coordinate placement
- Port extent spacing (2-pass algorithm)
- Graph padding (12px)
- Basic edge routing

### Test Coverage
- ✓ 2-node graph (vcc_and_gnd): **EXACT MATCH**
- ✓ 4-node chain: Works correctly
- ✓ 4-node DAG (diamond): Works correctly
- ✓ Port extent spacing: Verified correct
- ✗ 31-node circuit (up3down5): **NOT TESTED YET**

## What's Missing for 100% Compliance ✗

### 1. LinearSegmentsNodePlacer (883 lines)

#### Data Structures
- [ ] `LinearSegment` class
  - `List<LNode> nodes` - nodes in segment
  - `int id` - segment identifier
  - `double deflection` - force calculation
  - `int weight` - segment weight
  - `LinearSegment refSegment` - for region merging

- [ ] `LMargin` in `LNode`
  - `double top, bottom, left, right` - margins for ports/labels

- [ ] Priority properties
  - `INPUT_PRIO` - max incoming edge priority
  - `OUTPUT_PRIO` - max outgoing edge priority

#### Phase 1: Sort Linear Segments (~400 lines)
- [ ] `fillSegment()` - recursive segment creation for long edges
- [ ] `createDependencyGraphEdges()` - builds segment ordering with cycle detection
- [ ] Topological sort - determines placement order
- [ ] Split segments to avoid cycles

**Status**: Not implemented. Currently all nodes treated independently.

#### Phase 2: Unbalanced Placement (~100 lines)
- [ ] `Spacings` class with node type matrix
- [ ] `getVerticalSpacing(LNode n1, LNode n2)` - sophisticated spacing calc
- [ ] Use node margins in placement
- [ ] Respect individual spacing overrides

**Status**: Partially implemented. Uses simple port extent spacing, not full Spacings system.

#### Phase 3: Balance Placement (~300 lines)
- [ ] `calcDeflection()` - calculate optimal node movement
- [ ] Region merging - group touching segments
- [ ] Pendulum iterations (forward/backward)
- [ ] Rubber iterations (bidirectional)
- [ ] Convergence detection

**Status**: Not implemented. No optimization phase.

#### Phase 4: Post-Process (~100 lines)
- [ ] Straighten edges
- [ ] Remove overlaps
- [ ] Label adjustments

**Status**: Not implemented.

### 2. Node Margin Calculation

#### InnermostNodeMarginCalculator
- [ ] Calculate margins from port extents
- [ ] Calculate margins from port labels
- [ ] Handle all port sides (NORTH, SOUTH, EAST, WEST)

**Current**: Port extents handled in layer spacing, not as node margins.

### 3. Spacing Configuration

#### Spacings Class
- [ ] Node type spacing matrix (NORMAL, LONG_EDGE, etc.)
- [ ] Individual property overrides per node
- [ ] `getIndividualOrDefault()` - hierarchy-aware property lookup
- [ ] Math.max(spacing1, spacing2) - take maximum

**Current**: Fixed `nodeSpacing` and `layerSpacing` values only.

## Test Plan for Full Compliance

### Phase 1: Data Structures (Day 1, ~200 lines)
1. Add `LMargin` to C++ LNode
2. Add `LinearSegment` class with all fields
3. Add priority properties to LNode
4. Test: Verify compilation and basic usage

### Phase 2: Segment Creation (Day 1-2, ~400 lines)
1. Port `fillSegment()` method
2. Port `createDependencyGraphEdges()` method
3. Port topological sort algorithm
4. Test: Verify segment creation on simple graph, log segment structure

### Phase 3: Spacing System (Day 2, ~200 lines)
1. Port `Spacings` class with node type matrix
2. Port `getVerticalSpacing()` method
3. Port `getIndividualOrDefault()`
4. Test: Compare spacing calculations with Java on simple graph

### Phase 4: Unbalanced Placement (Day 2, ~100 lines)
1. Port `createUnbalancedPlacement()` using Spacings
2. Add node margin support in placement
3. Test: Compare node positions with Java on 10-node graph

### Phase 5: Balancing (Day 3, ~300 lines)
1. Port `calcDeflection()` method
2. Port region merging logic
3. Port balancing iterations (pendulum/rubber)
4. Test: Compare final positions with Java on complex graph

### Phase 6: Validation (Day 3-4)
1. Test `up3down5.json` - verify exact match
2. Test all files in `test/digital/`
3. Test all files in `test/analog/`
4. Document discrepancies < 0.1 pixels

**Acceptance**: All test files match within 0.1 pixels

## Current Test Results

### Simple Tests (PASSING ✓)
```
vcc_and_gnd.json (2 nodes):
  JavaScript: vcc=(12,12), gnd=(12,62), size=44x104
  C++:        vcc=(12,12), gnd=(12,62), size=44x104
  Difference: 0 pixels ✓ EXACT MATCH
```

### Complex Tests (NOT TESTED)
```
up3down5.json (31 nodes, 40 edges):
  JavaScript: Graph size = 865.5 x 729
  C++:        NOT YET TESTED
  Status:     Need full LinearSegmentsNodePlacer
```

## Recommendation

Given the 883-line algorithm and 4-phase structure:

**Option 1**: Full systematic port (3-4 days)
- Guarantees 100% compliance
- Can handle any netlistsvg circuit
- Sustainable long-term solution

**Option 2**: Incremental port (1-2 days)
- Port just Spacings + Margins first (~300 lines)
- Test with up3down5.json
- Continue if discrepancies > 5%

**Recommendation**: Option 1 for production use, Option 2 for prototyping.

## Next Steps

1. Review this document
2. Decide on Option 1 or Option 2
3. Begin systematic port with testing at each phase
4. Always reference Java source when implementing
5. Test incrementally, never batch multiple phases without testing

## References

- Java source: `JavaShit/org/eclipse/elk/alg/layered/p4nodes/LinearSegmentsNodePlacer.java` (883 lines)
- Spacing: `JavaShit/org/eclipse/elk/alg/layered/options/Spacings.java` (240 lines)
- Margins: `JavaShit/org/eclipse/elk/alg/layered/intermediate/InnermostNodeMarginCalculator.java` (63 lines)
- Dependencies: `elk-core` node spacing utilities (~500 lines)

**Total estimated port**: ~1700 lines of C++ code
