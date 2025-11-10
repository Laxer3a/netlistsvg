// Eclipse Layout Kernel - C++ Port
// Layered layout algorithm
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../core/layout_provider.h"
#include "../../core/types.h"
#include "lgraph.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace elk {
namespace layered {

// Note: LNode, LPort, LEdge, LLabel, Layer, LGraph are now in lgraph.h

// ============================================================================
// Layered Layout Provider
// ============================================================================

class LayeredLayoutProvider : public ILayoutProvider {
public:
    LayeredLayoutProvider();

    void layout(Node* graph, ProgressCallback progress = nullptr) override;
    std::string getAlgorithmId() const override { return "elk.layered"; }

    // Configuration
    void setDirection(Direction dir) { direction_ = dir; }
    void setNodeSpacing(double spacing) { nodeSpacing_ = spacing; }
    void setLayerSpacing(double spacing) { layerSpacing_ = spacing; }
    void setCrossingMinimization(CrossingMinimizationStrategy strategy) {
        crossingStrategy_ = strategy;
    }
    void setNodePlacement(NodePlacementStrategy strategy) {
        nodeStrategy_ = strategy;
    }

private:
    // Phase 1: Import graph and create internal representation
    void importGraph(Node* graph, std::vector<LNode*>& nodes, std::vector<LEdge*>& edges);

    // Phase 2: Cycle breaking - make graph acyclic
    void breakCycles(std::vector<LNode*>& nodes, std::vector<LEdge*>& edges);

    // Phase 2.5: Calculate node margins (for ports extending beyond bounds)
    void calculateNodeMargins(std::vector<LNode*>& nodes);

    // Phase 3: Layer assignment
    void assignLayers(std::vector<LNode*>& nodes, std::vector<Layer>& layers);
    std::vector<LNode*> assignLayersLongestPath(std::vector<LNode*>& nodes);

    // Phase 4: Add dummy nodes for edges spanning multiple layers
    void insertDummyNodes(std::vector<LNode*>& nodes, std::vector<LEdge*>& edges, std::vector<Layer>& layers);

    // Phase 5: Crossing minimization
    void minimizeCrossings(std::vector<Layer>& layers);
    int countCrossings(const Layer& upperLayer, const Layer& lowerLayer);
    void barycenterHeuristic(Layer& layer, bool useIncoming);

    // Phase 6: Node placement (coordinate assignment)
    void assignCoordinates(std::vector<Layer>& layers);
    void linearSegmentPlacement(std::vector<Layer>& layers);

    // LinearSegmentsNodePlacer algorithm (faithful port from Java)
    std::vector<LinearSegment*> sortLinearSegments(std::vector<Layer>& layers);
    bool fillSegment(LNode* node, LinearSegment* segment);
    void createDependencyGraphEdges(std::vector<Layer>& layers,
                                     std::vector<LinearSegment*>& segmentList,
                                     std::vector<std::vector<LinearSegment*>>& outgoingList,
                                     std::vector<int>& incomingCountList,
                                     int& nextLinearSegmentID);
    void createUnbalancedPlacement(std::vector<Layer>& layers,
                                    const std::vector<LinearSegment*>& linearSegments);

    // Phase 7: Edge routing
    void routeEdges(const std::vector<Layer>& layers, std::vector<LEdge*>& edges);

    // Apply layout back to original graph
    void applyLayout(const std::vector<LNode*>& nodes, const std::vector<LEdge*>& edges);

    // Calculate graph size from node positions
    void calculateGraphSize(Node* graph, const std::vector<LNode*>& nodes);

    // Cleanup
    void cleanup(std::vector<LNode*>& nodes, std::vector<LEdge*>& edges);

    // Configuration
    Direction direction_ = Direction::RIGHT;
    double nodeSpacing_ = 20.0;
    double layerSpacing_ = 50.0;
    CrossingMinimizationStrategy crossingStrategy_ = CrossingMinimizationStrategy::LAYER_SWEEP;
    NodePlacementStrategy nodeStrategy_ = NodePlacementStrategy::LINEAR_SEGMENTS;
};

// ============================================================================
// Utility functions
// ============================================================================

// Topological sort helper
inline void topologicalSortUtil(LNode* node, std::unordered_set<LNode*>& visited,
                                std::vector<LNode*>& stack) {
    visited.insert(node);
    for (LEdge* edge : node->getOutgoingEdges()) {
        if (!edge->reversed) {
            LNode* targetNode = edge->getTarget()->getNode();
            if (targetNode && visited.find(targetNode) == visited.end()) {
                topologicalSortUtil(targetNode, visited, stack);
            }
        }
    }
    stack.push_back(node);
}

} // namespace layered
} // namespace elk
