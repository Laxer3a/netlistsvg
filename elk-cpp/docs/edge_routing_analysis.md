# Orthogonal Edge Routing - Complete Analysis and Port Plan

## Executive Summary

**Total Scope:**
- **11 classes** to port
- **~2,716 lines** of Java code
- **71 methods** (public + private)
- **37 fields** across all classes

**Estimated Complexity:** MEDIUM-HIGH
- Core algorithm is well-defined (Sander 2004 paper)
- Multiple interdependent classes
- Requires understanding of hyperedge segment graph concept

---

## Class Hierarchy and Dependencies

```
OrthogonalEdgeRouter (entry point)
  └─> OrthogonalRoutingGenerator (main algorithm)
        ├─> HyperEdgeSegment (data structure for edge groups)
        ├─> HyperEdgeSegmentDependency (conflict relationships)
        ├─> HyperEdgeSegmentSplitter (resolve cycles)
        ├─> HyperEdgeCycleDetector (detect dependency cycles)
        └─> BaseRoutingDirectionStrategy (routing direction abstraction)
              ├─> WestToEastRoutingStrategy (left-to-right)
              ├─> NorthToSouthRoutingStrategy (top-to-bottom)
              └─> SouthToNorthRoutingStrategy (bottom-to-top)
```

---

## Detailed Class Breakdown

### 1. OrthogonalEdgeRouter.java (Main Entry Point)
**Lines:** 292 | **Methods:** 2 public | **Fields:** 0

**Purpose:** Phase 5 processor - routes all edges in the graph

**Key Methods:**
- `process(LGraph)` - Main entry point, iterates through layer pairs
- Calculates routing area width based on slot count
- Places nodes horizontally after routing

**Dependencies:**
- OrthogonalRoutingGenerator
- LGraphUtil.placeNodesHorizontally()

**Complexity:** LOW - Mostly orchestration

---

### 2. OrthogonalRoutingGenerator.java (Core Algorithm)
**Lines:** 559 | **Methods:** 3 public, 6 private | **Fields:** 11

**Purpose:** Generates orthogonal routing for edges between two layers

**Key Methods:**
- `routeEdges()` - Main routing algorithm, returns slot count
- `createHyperEdges()` - Groups edges into hyperedge segments
- `calculateHyperEdgePositions()` - Computes vertical positions
- `computeSlotAssignment()` - Assigns segments to routing slots
- `createBendPoints()` - Generates final bend points

**Algorithm Steps:**
1. Create hyperedge segments (group edges with shared vertical runs)
2. Calculate positions and detect conflicts
3. Build dependency graph
4. Assign slots using topological sort
5. Generate orthogonal bend points

**Dependencies:**
- HyperEdgeSegment
- HyperEdgeSegmentDependency
- HyperEdgeSegmentSplitter
- HyperEdgeCycleDetector
- BaseRoutingDirectionStrategy

**Complexity:** HIGH - Core algorithm implementation

---

### 3. HyperEdgeSegment.java (Data Structure)
**Lines:** 468 | **Methods:** 33 public, 7 private | **Fields:** 15

**Purpose:** Represents a vertical segment shared by multiple edges

**Key Fields:**
- `List<LEdge> edges` - Edges using this segment
- `LPort sourcePorts, targetPorts` - Connected ports
- `double routingSlot` - Assigned horizontal position
- `List<HyperEdgeSegmentDependency> outgoingDependencies`

**Key Methods:**
- `getSourcePortPosition()` - Get vertical position at source
- `getTargetPortPosition()` - Get vertical position at target
- `getRepresentative()` - For union-find grouping
- `union()` - Merge segments

**Complexity:** MEDIUM - Complex data structure with many accessors

---

### 4. HyperEdgeSegmentDependency.java (Conflict Tracking)
**Lines:** 179 | **Methods:** 11 public, 1 private | **Fields:** 4

**Purpose:** Represents a dependency between two hyperedge segments

**Key Fields:**
- `HyperEdgeSegment source, target`
- `int weight` - Penalty for conflict/crossing
- `Type type` - CONFLICT or CROSS

**Key Methods:**
- `getWeight()` - Get conflict penalty
- `getType()` - Get dependency type
- Getters/setters for source/target

**Complexity:** LOW - Simple data structure

---

### 5. HyperEdgeSegmentSplitter.java (Cycle Resolution)
**Lines:** 461 | **Methods:** 2 public, 16 private | **Fields:** 6

**Purpose:** Resolves cyclic dependencies by splitting segments

**Algorithm:**
When dependency graph has cycles (can't assign slots):
1. Find best segment to split
2. Split into two vertical segments
3. Rebuild dependency graph
4. Retry slot assignment

**Key Methods:**
- `splitIfNecessary()` - Main entry point
- `findBestSplit()` - Choose which segment to split
- `splitSegment()` - Perform the split

**Complexity:** MEDIUM-HIGH - Complex graph manipulation

---

### 6. HyperEdgeCycleDetector.java (Cycle Detection)
**Lines:** 270 | **Methods:** 1 public, 4 private | **Fields:** 0

**Purpose:** Detects cycles in hyperedge segment dependency graph

**Algorithm:** Modified DFS with strongly connected components

**Key Methods:**
- `hasCycles()` - Returns true if graph has cycles
- DFS helper methods

**Complexity:** MEDIUM - Standard graph algorithm

---

### 7. BaseRoutingDirectionStrategy.java (Direction Abstraction)
**Lines:** 186 | **Methods:** 7 public, 1 private | **Fields:** 1

**Purpose:** Abstract base for different routing directions

**Key Methods:**
- `getPortPositionOnHyperNode()` - Get port position (direction-dependent)
- `getPortPosition()` - Get absolute port position
- `setTargetPortPosition()` - Set calculated position

**Concrete Implementations:**
- WestToEastRoutingStrategy (92 lines) - Left-to-right (DEFAULT)
- NorthToSouthRoutingStrategy (91 lines) - Top-to-bottom
- SouthToNorthRoutingStrategy (92 lines) - Bottom-to-top

**Complexity:** MEDIUM - Coordinate transformation logic

---

### 8. RoutingDirection.java (Enum)
**Lines:** 26 | **Methods:** 0 | **Fields:** 0

**Purpose:** Enum for routing directions (WEST_TO_EAST, etc.)

**Complexity:** TRIVIAL - Just an enum

---

## Port Plan: Step-by-Step

### Phase 1: Data Structures (Foundation)
**Order:** Port these first as they're used by everything else

1. **RoutingDirection** (enum) - 10 minutes
2. **HyperEdgeSegmentDependency** - 30 minutes
3. **HyperEdgeSegment** - 2 hours (complex data structure)

### Phase 2: Utilities (Support Functions)
4. **HyperEdgeCycleDetector** - 1 hour
5. **BaseRoutingDirectionStrategy** + WestToEastRoutingStrategy - 1.5 hours
   (Only port WestToEast initially, others can wait)

### Phase 3: Core Algorithm
6. **HyperEdgeSegmentSplitter** - 2 hours
7. **OrthogonalRoutingGenerator** - 4 hours (CORE ALGORITHM)

### Phase 4: Integration
8. **OrthogonalEdgeRouter** - 1 hour
9. **Integration & Testing** - 2 hours

**Total Estimated Time:** ~14 hours of focused work

---

## Key Concepts to Understand

### Hyperedge Segments
Instead of routing each edge individually, edges are grouped into "hyperedge segments":
```
Source Layer          Target Layer
[Node A] ─┐           ┌─ [Node C]
          ├─ SEGMENT ─┤
[Node B] ─┘           └─ [Node D]
```

Multiple edges that share a vertical segment are combined into one HyperEdgeSegment.

### Routing Slots
Horizontal positions between layers are divided into discrete "slots":
```
Layer 0   |slot0|slot1|slot2|   Layer 1
[Node]    | --- |     | --- |   [Node]
          |     | --- |     |
```

Each hyperedge segment is assigned to a slot.

### Dependencies
Two segments have a dependency if they would conflict:
- **CONFLICT**: Segments too close vertically (would overlap)
- **CROSS**: One segment crosses over another

### Slot Assignment
Use topological sort on dependency graph to assign segments to slots without conflicts.

### Cycle Breaking
If dependency graph has cycles, split one segment into two to break the cycle.

---

## Testing Strategy

1. **Unit test each class** with simple inputs
2. **Test with up3down5.json** (31 nodes, 40 edges)
3. **Verify width** matches JavaScript (~957px)
4. **Verify orthogonal routing** (no diagonal lines)
5. **Check for edge overlaps** (none should occur)

---

## Success Criteria

✓ Width increases from 532px to ~950px
✓ Edges have proper orthogonal bend points
✓ No edge overlaps or conflicts
✓ Layer spacing varies based on edge density
✓ Matches JavaScript netlistsvg output visually

---

## Risks and Mitigations

**Risk:** Complex algorithm might have subtle bugs
**Mitigation:** Port incrementally, test each class, compare with Java execution

**Risk:** Coordinate system confusion (different directions)
**Mitigation:** Start with WestToEast only, add others later

**Risk:** Performance issues with large graphs
**Mitigation:** Profile and optimize after correctness is verified

---

## Notes

- We only need **ORTHOGONAL** routing, not polyline or spline
- Focus on **WEST_TO_EAST** direction first (standard left-to-right)
- Port methods ONE BY ONE in the order they're called
- Add extensive debug logging to verify correctness
- Compare intermediate results with Java at each step
