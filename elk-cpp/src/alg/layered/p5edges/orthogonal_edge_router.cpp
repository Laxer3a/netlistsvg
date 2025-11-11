// Eclipse Layout Kernel - C++ Port
// Orthogonal edge router implementation
// SPDX-License-Identifier: EPL-2.0

#include "../../../../include/elk/alg/layered/p5edges/orthogonal_edge_router.h"
#include "../../../../include/elk/alg/layered/p5edges/orthogonal_routing_generator.h"
#include "../../../../include/elk/alg/layered/p5edges/routing_direction.h"
#include <algorithm>

namespace elk {
namespace layered {
namespace p5edges {

double OrthogonalEdgeRouter::process(std::vector<Layer>& layers,
                                      double nodeNodeSpacing,
                                      double edgeEdgeSpacing,
                                      double edgeNodeSpacing) {
    // Create a temporary LGraph for routing
    static std::mt19937 random(42);  // Fixed seed for reproducibility
    LGraph tempGraph;
    tempGraph.nodeNodeSpacing = nodeNodeSpacing;
    tempGraph.edgeEdgeSpacing = edgeEdgeSpacing;
    tempGraph.edgeNodeSpacing = edgeNodeSpacing;
    tempGraph.random = random;

    // Create the routing generator
    OrthogonalRoutingGenerator routingGenerator(RoutingDirection::WEST_TO_EAST, edgeEdgeSpacing, "");

    double xpos = 0.0;
    const std::vector<LNode*>* leftLayerNodes = nullptr;
    const std::vector<LNode*>* rightLayerNodes = nullptr;
    int leftLayerIndex = -1;
    int rightLayerIndex = -1;

    // Iterate through all layers
    for (size_t layerIdx = 0; layerIdx <= layers.size(); layerIdx++) {
        int slotsCount;

        // Fetch the next layer, if any
        rightLayerNodes = (layerIdx < layers.size()) ? &layers[layerIdx].nodes : nullptr;
        rightLayerIndex = static_cast<int>(layerIdx);

        // Place the left layer's nodes horizontally, if any
        if (leftLayerNodes != nullptr) {
            for (LNode* node : *leftLayerNodes) {
                node->position.x = xpos;
            }

            // Find maximum width in this layer
            double layerWidth = 0.0;
            for (LNode* node : *leftLayerNodes) {
                layerWidth = std::max(layerWidth, node->size.width);
            }
            xpos += layerWidth;
        }

        // Route edges between the two layers
        double startPos = (leftLayerNodes == nullptr) ? xpos : xpos + edgeNodeSpacing;
        slotsCount = routingGenerator.routeEdges(&tempGraph, leftLayerNodes, leftLayerIndex,
                                                 rightLayerNodes, startPos);

        // Check if layers are external (containing only external west/east port dummies)
        bool isLeftLayerExternal = (leftLayerNodes == nullptr) ||
            std::all_of(leftLayerNodes->begin(), leftLayerNodes->end(), [](LNode* node) {
                // Check if node is an external port on west or east side
                return node->type == NodeType::EXTERNAL_PORT &&
                       (node->extPortSide == PortSide::WEST || node->extPortSide == PortSide::EAST);
            });

        bool isRightLayerExternal = (rightLayerNodes == nullptr) ||
            std::all_of(rightLayerNodes->begin(), rightLayerNodes->end(), [](LNode* node) {
                return node->type == NodeType::EXTERNAL_PORT &&
                       (node->extPortSide == PortSide::WEST || node->extPortSide == PortSide::EAST);
            });

        if (slotsCount > 0) {
            // Compute routing area's width based on the number of routing slots
            // This is the KEY FORMULA that creates dynamic layer spacing!
            double routingWidth = (slotsCount - 1) * edgeEdgeSpacing;

            if (leftLayerNodes != nullptr) {
                routingWidth += edgeNodeSpacing;
            }

            if (rightLayerNodes != nullptr) {
                routingWidth += edgeNodeSpacing;
            }

            // If we are between two layers, make sure their minimal spacing is preserved
            if (routingWidth < nodeNodeSpacing && !isLeftLayerExternal && !isRightLayerExternal) {
                routingWidth = nodeNodeSpacing;
            }

            xpos += routingWidth;
        } else if (!isLeftLayerExternal && !isRightLayerExternal) {
            // If all edges are straight, use the usual spacing
            xpos += nodeNodeSpacing;
        }

        // Advance to next layer
        leftLayerNodes = rightLayerNodes;
        leftLayerIndex = rightLayerIndex;
    }

    // Return the final graph width
    return xpos;
}

} // namespace p5edges
} // namespace layered
} // namespace elk
