// Eclipse Layout Kernel - C++ Port
// Brandes-Köpf implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/alg/layered/placement/brandes_koepf.h"
#include <algorithm>
#include <limits>
#include <cmath>

namespace elk {
namespace layered {
namespace placement {

void BrandesKoepfPlacer::place(std::vector<Layer>& layers,
                               const BrandesKoepfConfig& config) {
    if (layers.size() < 2) return;

    // Phase 1: Vertical alignment in all 4 directions
    std::vector<BKAlignment> alignments(4);
    alignments[0] = verticalAlignment(layers, BKDirection::UP_LEFT, config);
    alignments[1] = verticalAlignment(layers, BKDirection::UP_RIGHT, config);
    alignments[2] = verticalAlignment(layers, BKDirection::DOWN_LEFT, config);
    alignments[3] = verticalAlignment(layers, BKDirection::DOWN_RIGHT, config);

    // Phase 2: Horizontal compaction for each alignment
    horizontalCompaction(alignments[0], layers, BKDirection::UP_LEFT, config);
    horizontalCompaction(alignments[1], layers, BKDirection::UP_RIGHT, config);
    horizontalCompaction(alignments[2], layers, BKDirection::DOWN_LEFT, config);
    horizontalCompaction(alignments[3], layers, BKDirection::DOWN_RIGHT, config);

    // Phase 3: Compute final positions
    computePositions(layers, alignments, config);
}

BKAlignment BrandesKoepfPlacer::verticalAlignment(
    const std::vector<Layer>& layers,
    BKDirection direction,
    const BrandesKoepfConfig& config) {

    BKAlignment alignment;

    // Initialize: each node is its own root
    for (const auto& layer : layers) {
        for (LNode* node : layer.nodes) {
            alignment.root[node] = node;
            alignment.align[node] = node;
        }
    }

    // Detect type-1 conflicts
    std::unordered_map<LNode*, std::vector<LNode*>> conflicts;
    markType1Conflicts(layers, conflicts);

    // Process layers in specified direction
    int start = isUpward(direction) ? 1 : layers.size() - 2;
    int end = isUpward(direction) ? layers.size() : -1;
    int step = isUpward(direction) ? 1 : -1;

    for (int i = start; i != end; i += step) {
        int r = isLeftward(direction) ? layers[i].nodes.size() - 1 : 0;
        int rEnd = isLeftward(direction) ? -1 : layers[i].nodes.size();
        int rStep = isLeftward(direction) ? -1 : 1;

        for (int j = r; j != rEnd; j += rStep) {
            LNode* v = layers[i].nodes[j];
            auto neighbors = getNeighbors(v, layers, direction);

            if (!neighbors.empty()) {
                LNode* m = median(neighbors);

                if (m && alignment.align[v] == v && canAlign(m, v, conflicts)) {
                    alignment.align[m] = v;
                    alignment.root[v] = alignment.root[m];
                    alignment.align[v] = alignment.root[v];
                }
            }
        }
    }

    return alignment;
}

void BrandesKoepfPlacer::horizontalCompaction(
    BKAlignment& alignment,
    const std::vector<Layer>& layers,
    BKDirection direction,
    const BrandesKoepfConfig& config) {

    // Build blocks
    auto blocks = BlockBuilder::build(alignment, layers);

    // Place blocks
    std::unordered_map<BKBlock*, double> sink;
    std::unordered_map<BKBlock*, double> shift;

    for (auto& block : blocks) {
        placeBlock(&block, layers, sink, shift, config);
    }

    // Apply positions to nodes
    for (auto& block : blocks) {
        for (LNode* node : block.nodes) {
            node->position.x = block.position;
        }
    }
}

void BrandesKoepfPlacer::computePositions(
    std::vector<Layer>& layers,
    const std::vector<BKAlignment>& alignments,
    const BrandesKoepfConfig& config) {

    // For each node, collect positions from all 4 alignments
    for (auto& layer : layers) {
        for (LNode* node : layer.nodes) {
            std::vector<double> positions;

            // Collect positions (simplified - would need to track positions per alignment)
            positions.push_back(node->position.x);

            // Balance positions
            if (config.balanceBlocks && !positions.empty()) {
                node->position.x = balancePosition(positions);
            }
        }
    }
}

void BrandesKoepfPlacer::markType1Conflicts(
    const std::vector<Layer>& layers,
    std::unordered_map<LNode*, std::vector<LNode*>>& conflicts) {

    ConflictDetector::detectType1Conflicts(layers, conflicts);
}

LNode* BrandesKoepfPlacer::median(const std::vector<LNode*>& neighbors) {
    return getMedianNode(neighbors);
}

void BrandesKoepfPlacer::placeBlock(
    BKBlock* block,
    const std::vector<Layer>& layers,
    std::unordered_map<BKBlock*, double>& sink,
    std::unordered_map<BKBlock*, double>& shift,
    const BrandesKoepfConfig& config) {

    if (!block || block->nodes.empty()) return;

    // Simple placement: use minimum position considering spacing
    double minPos = 0.0;

    for (LNode* node : block->nodes) {
        minPos = std::max(minPos, node->position.x);
    }

    block->position = minPos;
}

double BrandesKoepfPlacer::separation(BKBlock* leftBlock, BKBlock* rightBlock,
                                     const BrandesKoepfConfig& config) {
    return config.nodeSpacing;
}

double BrandesKoepfPlacer::balancePosition(const std::vector<double>& positions) {
    return medianPosition(const_cast<std::vector<double>&>(positions));
}

std::vector<LNode*> BrandesKoepfPlacer::getNeighbors(
    LNode* node,
    const std::vector<Layer>& layers,
    BKDirection direction) {

    std::vector<LNode*> neighbors;

    bool useIncoming = isUpward(direction);

    if (useIncoming) {
        for (LPort* port : node->ports) {
            for (LEdge* edge : port->incomingEdges) {
                if (edge->source && edge->source->node) {
                    neighbors.push_back(edge->source->node);
                }
            }
        }
    } else {
        for (LPort* port : node->ports) {
            for (LEdge* edge : port->outgoingEdges) {
                if (edge->target && edge->target->node) {
                    neighbors.push_back(edge->target->node);
                }
            }
        }
    }

    // Sort neighbors by position
    std::sort(neighbors.begin(), neighbors.end(),
        [&layers](LNode* a, LNode* b) {
            int layerA = getLayerIndex(a, layers);
            int layerB = getLayerIndex(b, layers);

            if (layerA != layerB) return layerA < layerB;

            if (layerA >= 0 && layerA < layers.size()) {
                int posA = getNodeIndex(a, layers[layerA]);
                int posB = getNodeIndex(b, layers[layerB]);
                return posA < posB;
            }

            return false;
        });

    return neighbors;
}

bool BrandesKoepfPlacer::canAlign(
    LNode* u, LNode* v,
    const std::unordered_map<LNode*, std::vector<LNode*>>& conflicts) {

    // Check if alignment would create a conflict
    auto it = conflicts.find(u);
    if (it != conflicts.end()) {
        for (LNode* conflicting : it->second) {
            if (conflicting == v) {
                return false;
            }
        }
    }

    return true;
}

// BlockBuilder implementation

std::vector<BKBlock> BlockBuilder::build(
    const BKAlignment& alignment,
    const std::vector<Layer>& layers) {

    std::vector<BKBlock> blocks;
    auto roots = findRoots(alignment);

    for (LNode* root : roots) {
        BKBlock block;
        block.root = root;
        block.nodes = collectBlockNodes(root, alignment);
        blocks.push_back(block);
    }

    return blocks;
}

std::vector<LNode*> BlockBuilder::findRoots(const BKAlignment& alignment) {
    std::vector<LNode*> roots;

    for (const auto& pair : alignment.root) {
        LNode* node = pair.first;
        LNode* root = pair.second;

        if (node == root) {
            roots.push_back(root);
        }
    }

    return roots;
}

std::vector<LNode*> BlockBuilder::collectBlockNodes(
    LNode* root,
    const BKAlignment& alignment) {

    std::vector<LNode*> nodes;

    for (const auto& pair : alignment.root) {
        if (pair.second == root) {
            nodes.push_back(pair.first);
        }
    }

    return nodes;
}

double BlockBuilder::calculateBlockWidth(
    const std::vector<LNode*>& nodes,
    const BrandesKoepfConfig& config) {

    double maxWidth = 0.0;

    for (LNode* node : nodes) {
        maxWidth = std::max(maxWidth, node->size.width);
    }

    return maxWidth;
}

// ConflictDetector implementation

void ConflictDetector::detectType1Conflicts(
    const std::vector<Layer>& layers,
    std::unordered_map<LNode*, std::vector<LNode*>>& conflicts) {

    for (size_t i = 0; i < layers.size() - 1; ++i) {
        const Layer& layer1 = layers[i];
        const Layer& layer2 = layers[i + 1];

        for (size_t j = 0; j < layer1.nodes.size(); ++j) {
            for (size_t k = j + 1; k < layer1.nodes.size(); ++k) {
                LNode* u1 = layer1.nodes[j];
                LNode* u2 = layer1.nodes[k];

                // Check all edge pairs for crossings
                for (LPort* port1 : u1->ports) {
                    for (LEdge* edge1 : port1->outgoingEdges) {
                        LNode* v1 = edge1->target ? edge1->target->node : nullptr;

                        for (LPort* port2 : u2->ports) {
                            for (LEdge* edge2 : port2->outgoingEdges) {
                                LNode* v2 = edge2->target ? edge2->target->node : nullptr;

                                if (v1 && v2 && edgesCross(u1, v1, u2, v2, layers)) {
                                    conflicts[u1].push_back(v1);
                                    conflicts[u2].push_back(v2);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

bool ConflictDetector::edgesCross(LNode* u1, LNode* v1, LNode* u2, LNode* v2,
                                  const std::vector<Layer>& layers) {
    int layerU = getLayerIndex(u1, layers);
    int layerV = getLayerIndex(v1, layers);

    if (layerU < 0 || layerV < 0 || layerU >= layers.size() || layerV >= layers.size()) {
        return false;
    }

    int posU1 = getNodeIndex(u1, layers[layerU]);
    int posU2 = getNodeIndex(u2, layers[layerU]);
    int posV1 = getNodeIndex(v1, layers[layerV]);
    int posV2 = getNodeIndex(v2, layers[layerV]);

    // Edges cross if they have opposite order
    return (posU1 < posU2 && posV1 > posV2) || (posU1 > posU2 && posV1 < posV2);
}

int ConflictDetector::getPosition(LNode* node, const Layer& layer) {
    return getNodeIndex(node, layer);
}

} // namespace placement
} // namespace layered
} // namespace elk
