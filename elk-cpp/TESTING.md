# ELK C++ Testing Status

## Tests Implemented

### 1. Simple 2-Node Test (netlistsvg_test.cpp)
- **Status**: ✓ PASS
- **Graph**: 2 nodes (vcc, gnd) with ports, 1 edge
- **Direction**: DOWN
- **Result**: Exact match with JavaScript elkjs output
  - vcc at (12, 12)
  - gnd at (12, 62)
  - Graph size: 44 x 104

### 2. Complex Synthetic Tests (complex_test.cpp)
- **Test 1 - Chain**: ✓ PASS
  - 4 nodes in linear chain
  - Direction: RIGHT

- **Test 2 - DAG**: ✓ PASS
  - 4 nodes in diamond pattern
  - Direction: RIGHT

- **Test 3 - Port Extents**: ✓ PASS
  - 2 nodes with ports extending beyond bounds
  - Direction: DOWN
  - Port-to-port gap: 5 pixels (as expected)

## Key Features Verified

### ✓ Direction Handling
- DOWN direction properly swaps X/Y coordinate calculations
- UP direction support (untested but implemented)
- RIGHT direction works correctly
- LEFT direction support (implemented)

### ✓ Port Extent Spacing
- Two-pass algorithm:
  1. Pre-calculate port extents for all layers
  2. Apply spacing = layerSpacing + portExtentBelow(current) + portExtentAbove(next)
- Correctly handles ports extending beyond node boundaries

### ✓ Graph Padding
- 12px default padding applied to all sides
- Matches ELK Java/JavaScript behavior

## Known Limitations

### Algorithm Simplifications

The C++ implementation uses a simplified linear placement algorithm. The Java ELK uses a more sophisticated approach:

1. **Java ELK** (LinearSegmentsNodePlacer):
   - Linear segments (groups of nodes)
   - Unbalanced placement phase
   - Balancing with pendulum/rubber iterations
   - Post-processing phase
   - Node margins calculated from ports/labels

2. **C++ Implementation** (current):
   - Simple layer-by-layer placement
   - Direct port extent calculation during placement
   - No balancing or optimization phases

This means:
- ✓ Simple graphs with few crossings will match
- ✗ Complex graphs may have different node ordering within layers
- ✗ Complex graphs may have slightly different Y-positions

### Not Yet Tested

- Real netlistsvg circuits (up3down5.json, generics.json, etc.)
- Graphs with >10 nodes
- Graphs with complex port configurations
- Edge routing quality
- Label placement
- Self-loops
- Hierarchical graphs

## Next Steps for Production Use

To make this suitable for real netlistsvg usage:

1. **Test with real circuits**: Run all test/digital/*.json and test/analog/*.json files
2. **Implement linear segments**: Port the full LinearSegmentsNodePlacer algorithm
3. **Add node margins**: Implement InnermostNodeMarginCalculator
4. **Crossing minimization**: Ensure proper layer ordering
5. **JSON parser**: Add nlohmann/json or similar to parse netlistsvg graphs directly

## References

- Java source: `/JavaShit/org/eclipse/elk/alg/layered/`
- Key files:
  - `p4nodes/LinearSegmentsNodePlacer.java` - Node placement
  - `options/Spacings.java` - Spacing calculation
  - `intermediate/InnermostNodeMarginCalculator.java` - Margin calculation
