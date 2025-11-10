// Eclipse Layout Kernel - C++ Port
// Self-loops processor implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/alg/layered/intermediate/self_loops.h"
#include "elk/alg/layered/layered_layout.h"
#include <algorithm>
#include <cmath>

namespace elk {
namespace layered {

void SelfLoopProcessor::process(std::vector<LNode*>& nodes,
                               std::vector<LEdge*>& edges,
                               const SelfLoopConfig& config) {
    // Phase 1: Identify self-loops
    auto groups = identifySelfLoops(nodes, edges);

    if (groups.empty()) return;

    // Phase 2: Determine placement
    determinePlacement(groups, config);

    // Phase 3: Calculate geometry
    calculateGeometry(groups, config);

    // Phase 4: Route loops
    routeLoops(groups, config);

    // Phase 5: Apply to graph
    applyToGraph(groups);
}

std::vector<SelfLoopGroup> SelfLoopProcessor::identifySelfLoops(
    const std::vector<LNode*>& nodes,
    const std::vector<LEdge*>& edges) {

    std::unordered_map<LNode*, SelfLoopGroup> groupMap;

    for (LEdge* edge : edges) {
        if (isSelfLoop(edge)) {
            LNode* node = edge->source;

            if (groupMap.find(node) == groupMap.end()) {
                SelfLoopGroup group;
                group.node = node;
                groupMap[node] = group;
            }

            SelfLoopEdge loopEdge;
            loopEdge.edge = edge;
            loopEdge.node = node;
            loopEdge.sourcePort = edge->sourcePort;
            loopEdge.targetPort = edge->targetPort;

            groupMap[node].loops.push_back(loopEdge);
        }
    }

    std::vector<SelfLoopGroup> groups;
    for (auto& pair : groupMap) {
        groups.push_back(pair.second);
    }

    return groups;
}

void SelfLoopProcessor::determinePlacement(std::vector<SelfLoopGroup>& groups,
                                          const SelfLoopConfig& config) {
    for (auto& group : groups) {
        if (config.optimizePlacement) {
            group.preferredSide = chooseBestSide(group, config);
        } else {
            // Use default placement
            switch (config.placement) {
                case SelfLoopPlacement::NORTH:
                    group.preferredSide = PortSide::NORTH;
                    break;
                case SelfLoopPlacement::SOUTH:
                    group.preferredSide = PortSide::SOUTH;
                    break;
                case SelfLoopPlacement::EAST:
                    group.preferredSide = PortSide::EAST;
                    break;
                case SelfLoopPlacement::WEST:
                    group.preferredSide = PortSide::WEST;
                    break;
                case SelfLoopPlacement::STACKED:
                    // Find side with fewest edges
                    group.preferredSide = chooseBestSide(group, config);
                    break;
            }
        }

        // Assign side to all loops in group
        for (auto& loop : group.loops) {
            loop.placementSide = group.preferredSide;
        }
    }
}

PortSide SelfLoopProcessor::chooseBestSide(const SelfLoopGroup& group,
                                          const SelfLoopConfig& config) {
    LNode* node = group.node;

    // Count edges on each side
    int northCount = countEdgesOnSide(node, PortSide::NORTH);
    int southCount = countEdgesOnSide(node, PortSide::SOUTH);
    int eastCount = countEdgesOnSide(node, PortSide::EAST);
    int westCount = countEdgesOnSide(node, PortSide::WEST);

    // Find side with fewest edges
    int minCount = std::min({northCount, southCount, eastCount, westCount});

    if (northCount == minCount) return PortSide::NORTH;
    if (southCount == minCount) return PortSide::SOUTH;
    if (eastCount == minCount) return PortSide::EAST;
    return PortSide::WEST;
}

int SelfLoopProcessor::countEdgesOnSide(LNode* node, PortSide side) {
    int count = 0;

    // Count incoming edges
    for (LEdge* edge : node->incomingEdges) {
        if (edge->targetPort && edge->targetPort->side == side) {
            count++;
        }
    }

    // Count outgoing edges
    for (LEdge* edge : node->outgoingEdges) {
        if (edge->sourcePort && edge->sourcePort->side == side) {
            count++;
        }
    }

    return count;
}

void SelfLoopProcessor::calculateGeometry(std::vector<SelfLoopGroup>& groups,
                                         const SelfLoopConfig& config) {
    for (auto& group : groups) {
        // Calculate individual loop sizes
        for (auto& loop : group.loops) {
            calculateSingleLoopSize(loop, config);
        }

        // Stack loops
        stackLoops(group, config);
    }
}

void SelfLoopProcessor::calculateSingleLoopSize(SelfLoopEdge& loop,
                                               const SelfLoopConfig& config) {
    // Default size
    loop.width = std::max(config.loopSize, config.minLoopWidth);
    loop.height = std::max(config.loopSize, config.minLoopHeight);

    // Adjust based on port positions if needed
    if (config.respectPortSides && loop.sourcePort && loop.targetPort) {
        Point sourcePos = getPortPosition(loop.sourcePort, loop.node);
        Point targetPos = getPortPosition(loop.targetPort, loop.node);

        double distance = std::sqrt(
            std::pow(targetPos.x - sourcePos.x, 2) +
            std::pow(targetPos.y - sourcePos.y, 2)
        );

        loop.width = std::max(loop.width, distance * 1.5);
        loop.height = std::max(loop.height, distance * 1.5);
    }
}

void SelfLoopProcessor::stackLoops(SelfLoopGroup& group,
                                  const SelfLoopConfig& config) {
    if (group.loops.empty()) return;

    double currentOffset = 0;

    for (size_t i = 0; i < group.loops.size(); ++i) {
        auto& loop = group.loops[i];
        loop.stackIndex = i;

        if (group.preferredSide == PortSide::NORTH ||
            group.preferredSide == PortSide::SOUTH) {
            // Stack vertically
            currentOffset += loop.height + (i > 0 ? config.loopSpacing : 0);
        } else {
            // Stack horizontally
            currentOffset += loop.width + (i > 0 ? config.loopSpacing : 0);
        }
    }

    group.totalWidth = currentOffset;
    group.totalHeight = currentOffset;
}

void SelfLoopProcessor::routeLoops(std::vector<SelfLoopGroup>& groups,
                                  const SelfLoopConfig& config) {
    for (auto& group : groups) {
        for (auto& loop : group.loops) {
            switch (config.routing) {
                case SelfLoopRouting::ORTHOGONAL:
                    routeOrthogonal(loop, config);
                    break;
                case SelfLoopRouting::ROUNDED:
                    routeRounded(loop, config);
                    break;
                case SelfLoopRouting::SPLINE:
                    routeSpline(loop, config);
                    break;
            }
        }
    }
}

void SelfLoopProcessor::routeOrthogonal(SelfLoopEdge& loop,
                                       const SelfLoopConfig& config) {
    Rect nodeBounds = getNodeBounds(loop.node);
    double offset = (loop.stackIndex + 1) * (config.loopSize + config.loopSpacing);

    Point start = loop.sourcePort ?
                 getPortPosition(loop.sourcePort, loop.node) :
                 Point{nodeBounds.x, nodeBounds.y};

    Point end = loop.targetPort ?
               getPortPosition(loop.targetPort, loop.node) :
               Point{nodeBounds.x + nodeBounds.width, nodeBounds.y};

    loop.bendPoints.clear();
    loop.bendPoints.push_back(start);

    // Create rectangular path
    switch (loop.placementSide) {
        case PortSide::NORTH: {
            double topY = nodeBounds.y - offset;
            loop.bendPoints.push_back(Point{start.x, topY});
            loop.bendPoints.push_back(Point{end.x, topY});
            break;
        }
        case PortSide::SOUTH: {
            double bottomY = nodeBounds.y + nodeBounds.height + offset;
            loop.bendPoints.push_back(Point{start.x, bottomY});
            loop.bendPoints.push_back(Point{end.x, bottomY});
            break;
        }
        case PortSide::EAST: {
            double rightX = nodeBounds.x + nodeBounds.width + offset;
            loop.bendPoints.push_back(Point{rightX, start.y});
            loop.bendPoints.push_back(Point{rightX, end.y});
            break;
        }
        case PortSide::WEST: {
            double leftX = nodeBounds.x - offset;
            loop.bendPoints.push_back(Point{leftX, start.y});
            loop.bendPoints.push_back(Point{leftX, end.y});
            break;
        }
        default:
            break;
    }

    loop.bendPoints.push_back(end);
}

void SelfLoopProcessor::routeRounded(SelfLoopEdge& loop,
                                    const SelfLoopConfig& config) {
    // Start with orthogonal routing
    routeOrthogonal(loop, config);

    // The actual rounding would be handled by the edge routing system
    // We just mark it as needing rounded corners
}

void SelfLoopProcessor::routeSpline(SelfLoopEdge& loop,
                                   const SelfLoopConfig& config) {
    Rect nodeBounds = getNodeBounds(loop.node);
    double offset = (loop.stackIndex + 1) * (config.loopSize + config.loopSpacing);

    Point start = loop.sourcePort ?
                 getPortPosition(loop.sourcePort, loop.node) :
                 Point{nodeBounds.x, nodeBounds.y};

    Point end = loop.targetPort ?
               getPortPosition(loop.targetPort, loop.node) :
               Point{nodeBounds.x + nodeBounds.width, nodeBounds.y};

    loop.bendPoints.clear();
    loop.bendPoints.push_back(start);

    // Create control points for spline
    Point corner = getCornerPoint(nodeBounds, loop.placementSide, offset);

    switch (loop.placementSide) {
        case PortSide::NORTH:
        case PortSide::SOUTH: {
            double midX = (start.x + end.x) / 2;
            loop.bendPoints.push_back(Point{start.x, corner.y});
            loop.bendPoints.push_back(Point{midX, corner.y});
            loop.bendPoints.push_back(Point{end.x, corner.y});
            break;
        }
        case PortSide::EAST:
        case PortSide::WEST: {
            double midY = (start.y + end.y) / 2;
            loop.bendPoints.push_back(Point{corner.x, start.y});
            loop.bendPoints.push_back(Point{corner.x, midY});
            loop.bendPoints.push_back(Point{corner.x, end.y});
            break;
        }
        default:
            break;
    }

    loop.bendPoints.push_back(end);
}

void SelfLoopProcessor::applyToGraph(const std::vector<SelfLoopGroup>& groups) {
    for (const auto& group : groups) {
        for (const auto& loop : group.loops) {
            if (loop.edge && loop.edge->originalEdge) {
                Edge* originalEdge = loop.edge->originalEdge;

                // Create edge section with bend points
                if (originalEdge->sections.empty()) {
                    originalEdge->sections.emplace_back();
                }

                EdgeSection& section = originalEdge->sections[0];
                section.bendPoints = loop.bendPoints;

                if (!loop.bendPoints.empty()) {
                    section.startPoint = loop.bendPoints.front();
                    section.endPoint = loop.bendPoints.back();
                }
            }
        }
    }
}

bool SelfLoopProcessor::isSelfLoop(const LEdge* edge) {
    return edge && edge->source == edge->target;
}

Point SelfLoopProcessor::getPortPosition(LPort* port, LNode* node) {
    if (!port || !node) return Point(0, 0);

    // Port position - if original port exists, use its position
    // Otherwise estimate based on side
    if (port->originalPort) {
        return Point(
            node->position.x + port->originalPort->position.x,
            node->position.y + port->originalPort->position.y
        );
    }

    // Default position based on side
    switch (port->side) {
        case PortSide::NORTH:
            return Point(node->position.x + node->size.width / 2, node->position.y);
        case PortSide::SOUTH:
            return Point(node->position.x + node->size.width / 2, node->position.y + node->size.height);
        case PortSide::EAST:
            return Point(node->position.x + node->size.width, node->position.y + node->size.height / 2);
        case PortSide::WEST:
            return Point(node->position.x, node->position.y + node->size.height / 2);
        default:
            return Point(node->position.x, node->position.y);
    }
}

Rect SelfLoopProcessor::getNodeBounds(LNode* node) {
    if (!node) return Rect{0, 0, 0, 0};

    return Rect{
        node->position.x,
        node->position.y,
        node->size.width,
        node->size.height
    };
}

} // namespace layered
} // namespace elk
