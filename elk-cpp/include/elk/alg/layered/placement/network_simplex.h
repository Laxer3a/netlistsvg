// Eclipse Layout Kernel - C++ Port
// Network Simplex algorithm for node placement
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../../core/types.h"
#include "../layered_layout.h"
#include <vector>
#include <limits>

namespace elk {
namespace layered {
namespace placement {

// ============================================================================
// Network Simplex Types
// ============================================================================

struct SimplexEdge {
    int source = -1;
    int target = -1;
    double weight = 1.0;
    double minLength = 1.0;    // Minimum separation
    int cutValue = 0;          // Cut value for simplex algorithm
    bool inTree = false;       // Whether edge is in spanning tree

    LEdge* originalEdge = nullptr;
};

struct SimplexNode {
    LNode* node = nullptr;
    int id = -1;
    double position = 0.0;     // Calculated position
    int depth = 0;             // Depth in spanning tree
    SimplexNode* parent = nullptr;
    std::vector<SimplexNode*> children;
};

// ============================================================================
// Network Simplex Configuration
// ============================================================================

struct NetworkSimplexConfig {
    double nodeSpacing = 20.0;         // Minimum spacing between nodes
    double edgeSpacing = 10.0;         // Spacing for parallel edges
    int maxIterations = 100;           // Maximum simplex iterations

    bool balanceNodes = true;          // Try to balance node positions
    bool straightenEdges = true;       // Prefer straight edges
    double straightEdgeWeight = 2.0;   // Weight for straight edges

    bool debug = false;                // Enable debug output
};

// ============================================================================
// Network Simplex Placer
// ============================================================================

class NetworkSimplexPlacer {
public:
    static void place(std::vector<Layer>& layers,
                     const NetworkSimplexConfig& config);

private:
    // Phase 1: Build constraint graph
    static std::vector<SimplexNode> buildGraph(
        const std::vector<Layer>& layers,
        std::vector<SimplexEdge>& edges,
        const NetworkSimplexConfig& config);

    // Phase 2: Create initial feasible tree
    static void createFeasibleTree(
        std::vector<SimplexNode>& nodes,
        std::vector<SimplexEdge>& edges);

    // Phase 3: Run network simplex
    static void runSimplex(
        std::vector<SimplexNode>& nodes,
        std::vector<SimplexEdge>& edges,
        const NetworkSimplexConfig& config);

    // Phase 4: Extract positions
    static void extractPositions(
        const std::vector<SimplexNode>& nodes,
        std::vector<Layer>& layers);

    // Simplex operations
    static SimplexEdge* findEnteringEdge(
        std::vector<SimplexEdge>& edges);

    static SimplexEdge* findLeavingEdge(
        std::vector<SimplexNode>& nodes,
        std::vector<SimplexEdge>& edges,
        SimplexEdge* enteringEdge);

    static void updateTree(
        std::vector<SimplexNode>& nodes,
        std::vector<SimplexEdge>& edges,
        SimplexEdge* enteringEdge,
        SimplexEdge* leavingEdge);

    // Tree operations
    static void calculateCutValues(
        std::vector<SimplexNode>& nodes,
        std::vector<SimplexEdge>& edges);

    static int calculateCutValue(
        SimplexEdge* edge,
        const std::vector<SimplexNode>& nodes,
        const std::vector<SimplexEdge>& edges);

    // Position calculation
    static void calculatePositions(
        std::vector<SimplexNode>& nodes,
        const std::vector<SimplexEdge>& edges);

    static void normalizePositions(std::vector<SimplexNode>& nodes);

    // Utilities
    static SimplexNode* findNode(std::vector<SimplexNode>& nodes, int id);
    static bool formsLoop(SimplexNode* source, SimplexNode* target);
};

// ============================================================================
// Constraint Graph Builder
// ============================================================================

class ConstraintGraphBuilder {
public:
    // Build constraint graph from layers
    static void build(const std::vector<Layer>& layers,
                     std::vector<SimplexNode>& nodes,
                     std::vector<SimplexEdge>& edges,
                     const NetworkSimplexConfig& config);

private:
    // Add node ordering constraints (within layer)
    static void addOrderingConstraints(
        const Layer& layer,
        std::vector<SimplexNode>& nodes,
        std::vector<SimplexEdge>& edges,
        const NetworkSimplexConfig& config);

    // Add edge straightness constraints (between layers)
    static void addEdgeConstraints(
        const std::vector<Layer>& layers,
        std::vector<SimplexNode>& nodes,
        std::vector<SimplexEdge>& edges,
        const NetworkSimplexConfig& config);

    // Add balance constraints
    static void addBalanceConstraints(
        std::vector<SimplexNode>& nodes,
        std::vector<SimplexEdge>& edges,
        const NetworkSimplexConfig& config);
};

// ============================================================================
// Spanning Tree
// ============================================================================

class SpanningTreeBuilder {
public:
    // Build initial spanning tree
    static void build(std::vector<SimplexNode>& nodes,
                     std::vector<SimplexEdge>& edges);

private:
    // Create tight tree (edges with slack = 0)
    static void createTightTree(std::vector<SimplexNode>& nodes,
                               std::vector<SimplexEdge>& edges);

    // Make tree feasible
    static void makeFeasible(std::vector<SimplexNode>& nodes,
                            std::vector<SimplexEdge>& edges);

    // DFS to build tree structure
    static void buildTreeStructure(std::vector<SimplexNode>& nodes,
                                   const std::vector<SimplexEdge>& edges);
};

// ============================================================================
// Utility Functions
// ============================================================================

// Calculate slack for edge
inline double calculateSlack(const SimplexEdge& edge,
                            const std::vector<SimplexNode>& nodes) {
    if (edge.source < 0 || edge.target < 0 ||
        edge.source >= nodes.size() || edge.target >= nodes.size()) {
        return 0.0;
    }

    double actualLength = nodes[edge.target].position - nodes[edge.source].position;
    return actualLength - edge.minLength;
}

// Check if edge is tight (slack = 0)
inline bool isTight(const SimplexEdge& edge,
                   const std::vector<SimplexNode>& nodes) {
    return std::abs(calculateSlack(edge, nodes)) < 1e-6;
}

// Find path between nodes in tree
inline std::vector<SimplexNode*> findPath(SimplexNode* from, SimplexNode* to) {
    std::vector<SimplexNode*> path;

    // Simple implementation: go up to root then down
    // (more efficient implementation would use lowest common ancestor)

    SimplexNode* current = from;
    while (current) {
        path.push_back(current);
        current = current->parent;
    }

    return path;
}

// Calculate node rank (for layering)
inline int calculateRank(const SimplexNode& node,
                        const std::vector<SimplexEdge>& edges) {
    int rank = 0;

    for (const auto& edge : edges) {
        if (edge.target == node.id && edge.inTree) {
            // This is a tree edge pointing to this node
            ++rank;
        }
    }

    return rank;
}

// Get edges incident to node
inline std::vector<SimplexEdge*> getIncidentEdges(
    int nodeId, std::vector<SimplexEdge>& edges) {

    std::vector<SimplexEdge*> incident;

    for (auto& edge : edges) {
        if (edge.source == nodeId || edge.target == nodeId) {
            incident.push_back(&edge);
        }
    }

    return incident;
}

// Get tree edges only
inline std::vector<SimplexEdge*> getTreeEdges(std::vector<SimplexEdge>& edges) {
    std::vector<SimplexEdge*> treeEdges;

    for (auto& edge : edges) {
        if (edge.inTree) {
            treeEdges.push_back(&edge);
        }
    }

    return treeEdges;
}

} // namespace placement
} // namespace layered
} // namespace elk
