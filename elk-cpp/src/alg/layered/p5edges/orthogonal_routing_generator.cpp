// Eclipse Layout Kernel - C++ Port
// Orthogonal routing generator implementation
// SPDX-License-Identifier: EPL-2.0

#include "../../../../include/elk/alg/layered/p5edges/orthogonal_routing_generator.h"
#include "../../../../include/elk/alg/layered/p5edges/base_routing_direction_strategy.h"
#include "../../../../include/elk/alg/layered/p5edges/hyper_edge_segment_splitter.h"
#include "../../../../include/elk/alg/layered/p5edges/hyper_edge_cycle_detector.h"
#include <algorithm>
#include <limits>
#include <cmath>

namespace elk {
namespace layered {
namespace p5edges {

///////////////////////////////////////////////////////////////////////////////
// External functions for HyperEdgeSegmentSplitter

// These are called from HyperEdgeSegmentSplitter
int countCrossings(const std::list<double>& posis, double start, double end) {
    return OrthogonalRoutingGenerator::countCrossings(posis, start, end);
}

void createDependencyIfNecessary(OrthogonalRoutingGenerator* gen,
                                 HyperEdgeSegment* seg1,
                                 HyperEdgeSegment* seg2) {
    gen->createDependencyIfNecessary(seg1, seg2);
}

///////////////////////////////////////////////////////////////////////////////
// Constructor and Destructor

OrthogonalRoutingGenerator::OrthogonalRoutingGenerator(RoutingDirection direction,
                                                       double edgeSpacing,
                                                       const std::string& debugPrefix)
    : segmentSplitter_(nullptr)
    , routingStrategy_(BaseRoutingDirectionStrategy::forRoutingDirection(direction))
    , edgeSpacing_(edgeSpacing)
    , conflictThreshold_(CONFLICT_THRESHOLD_FACTOR * edgeSpacing)
    , criticalConflictThreshold_(0.0)
    , debugPrefix_(debugPrefix) {
}

OrthogonalRoutingGenerator::~OrthogonalRoutingGenerator() {
    delete segmentSplitter_;
    delete routingStrategy_;
}

///////////////////////////////////////////////////////////////////////////////
// Edge Routing

int OrthogonalRoutingGenerator::routeEdges(LGraph* layeredGraph,
                                            const std::vector<LNode*>* sourceLayerNodes,
                                            int sourceLayerIndex,
                                            const std::vector<LNode*>* targetLayerNodes,
                                            double startPos) {

    // Keep track of our hyperedge segments, and which ports they were created for
    std::map<LPort*, HyperEdgeSegment*> portToEdgeSegmentMap;
    std::vector<HyperEdgeSegment*> edgeSegments;

    // Create hyperedge segments for eastern output ports of the left layer and for western
    // output ports of the right layer
    createHyperEdgeSegments(sourceLayerNodes, routingStrategy_->getSourcePortSide(),
                           edgeSegments, portToEdgeSegmentMap);
    createHyperEdgeSegments(targetLayerNodes, routingStrategy_->getTargetPortSide(),
                           edgeSegments, portToEdgeSegmentMap);

    // Our critical conflict threshold is a fraction of the minimum distance between two
    // horizontal hyperedge segments
    criticalConflictThreshold_ = CRITICAL_CONFLICT_THRESHOLD_FACTOR *
                                 minimumHorizontalSegmentDistance(edgeSegments);

    // Create dependencies for the hyperedge segment ordering graph and note how many critical
    // dependencies have been created
    int criticalDependencyCount = 0;
    for (size_t firstIdx = 0; firstIdx < edgeSegments.size() - 1; firstIdx++) {
        HyperEdgeSegment* firstSegment = edgeSegments[firstIdx];
        for (size_t secondIdx = firstIdx + 1; secondIdx < edgeSegments.size(); secondIdx++) {
            criticalDependencyCount += createDependencyIfNecessary(firstSegment, edgeSegments[secondIdx]);
        }
    }

    // Get random number generator from graph properties
    std::mt19937& random = layeredGraph->random;

    // If there are at least two critical dependencies, there may be critical cycles that need
    // to be broken
    if (criticalDependencyCount >= 2) {
        breakCriticalCycles(edgeSegments, random);
    }

    // Break non-critical cycles
    breakNonCriticalCycles(edgeSegments, random);

    // Assign ranks to the edge segments
    topologicalNumbering(edgeSegments);

    // Set bend points with appropriate coordinates
    int rankCount = -1;
    for (HyperEdgeSegment* node : edgeSegments) {
        // Edges that are just straight lines don't take up a slot and don't need bend points
        if (std::abs(node->getStartCoordinate() - node->getEndCoordinate()) < TOLERANCE) {
            continue;
        }

        rankCount = std::max(rankCount, node->getRoutingSlot());

        routingStrategy_->calculateBendPoints(node, startPos, edgeSpacing_);
    }

    // Release the created resources
    routingStrategy_->clearCreatedJunctionPoints();

    // Clean up hyperedge segments
    for (HyperEdgeSegment* segment : edgeSegments) {
        delete segment;
    }

    return rankCount + 1;
}

double OrthogonalRoutingGenerator::minimumHorizontalSegmentDistance(
        const std::vector<HyperEdgeSegment*>& edgeSegments) {

    std::vector<double> incomingCoords;
    std::vector<double> outgoingCoords;

    for (const auto* segment : edgeSegments) {
        for (double coord : segment->getIncomingConnectionCoordinates()) {
            incomingCoords.push_back(coord);
        }
        for (double coord : segment->getOutgoingConnectionCoordinates()) {
            outgoingCoords.push_back(coord);
        }
    }

    double minIncomingDistance = minimumDifference(incomingCoords);
    double minOutgoingDistance = minimumDifference(outgoingCoords);

    return std::min(minIncomingDistance, minOutgoingDistance);
}

double OrthogonalRoutingGenerator::minimumDifference(std::vector<double>& numbers) {
    // Sort and remove duplicates
    std::sort(numbers.begin(), numbers.end());
    numbers.erase(std::unique(numbers.begin(), numbers.end()), numbers.end());

    double minDifference = std::numeric_limits<double>::max();

    if (numbers.size() >= 2) {
        for (size_t i = 1; i < numbers.size(); i++) {
            minDifference = std::min(minDifference, numbers[i] - numbers[i - 1]);
        }
    }

    return minDifference;
}

///////////////////////////////////////////////////////////////////////////////
// Hyper Edge Graph Creation

void OrthogonalRoutingGenerator::createHyperEdgeSegments(
        const std::vector<LNode*>* nodes,
        PortSide portSide,
        std::vector<HyperEdgeSegment*>& hyperEdges,
        std::map<LPort*, HyperEdgeSegment*>& portToHyperEdgeSegmentMap) {

    if (nodes != nullptr) {
        for (LNode* node : *nodes) {
            for (LPort* port : node->getPorts()) {
                // Filter for OUTPUT ports on the specified side
                if (port->type == PortType::OUTPUT && port->side == portSide) {
                    HyperEdgeSegment* hyperEdge = portToHyperEdgeSegmentMap[port];
                    if (hyperEdge == nullptr) {
                        hyperEdge = new HyperEdgeSegment(routingStrategy_);
                        hyperEdges.push_back(hyperEdge);
                        hyperEdge->addPortPositions(port, portToHyperEdgeSegmentMap);
                    }
                }
            }
        }
    }
}

int OrthogonalRoutingGenerator::createDependencyIfNecessary(HyperEdgeSegment* he1,
                                                             HyperEdgeSegment* he2) {
    // Check if at least one of the two nodes is just a straight line; those don't
    // create dependencies since they don't take up a slot
    if (std::abs(he1->getStartCoordinate() - he1->getEndCoordinate()) < TOLERANCE ||
        std::abs(he2->getStartCoordinate() - he2->getEndCoordinate()) < TOLERANCE) {
        return 0;
    }

    // Compare number of conflicts for both variants
    int conflicts1 = countConflicts(he1->getOutgoingConnectionCoordinates(),
                                    he2->getIncomingConnectionCoordinates());
    int conflicts2 = countConflicts(he2->getOutgoingConnectionCoordinates(),
                                    he1->getIncomingConnectionCoordinates());

    bool criticalConflictsDetected =
        conflicts1 == CRITICAL_CONFLICTS_DETECTED || conflicts2 == CRITICAL_CONFLICTS_DETECTED;
    int criticalDependencyCount = 0;

    if (criticalConflictsDetected) {
        // Check which critical dependencies have to be added
        if (conflicts1 == CRITICAL_CONFLICTS_DETECTED) {
            // hyperedge 1 MUST NOT be left of hyperedge 2, since that would cause critical conflicts
            HyperEdgeSegmentDependency::createAndAddCritical(he2, he1);
            criticalDependencyCount++;
        }

        if (conflicts2 == CRITICAL_CONFLICTS_DETECTED) {
            // hyperedge 2 MUST NOT be left of hyperedge 1, since that would cause critical conflicts
            HyperEdgeSegmentDependency::createAndAddCritical(he1, he2);
            criticalDependencyCount++;
        }

    } else {
        // We did not detect critical conflicts, so count the number of crossings for both variants
        int crossings1 = countCrossings(he1->getOutgoingConnectionCoordinates(),
                                        he2->getStartCoordinate(), he2->getEndCoordinate());
        crossings1 += countCrossings(he2->getIncomingConnectionCoordinates(),
                                     he1->getStartCoordinate(), he1->getEndCoordinate());

        int crossings2 = countCrossings(he2->getOutgoingConnectionCoordinates(),
                                        he1->getStartCoordinate(), he1->getEndCoordinate());
        crossings2 += countCrossings(he1->getIncomingConnectionCoordinates(),
                                     he2->getStartCoordinate(), he2->getEndCoordinate());

        // Compute the penalty; crossings are deemed worse than (non-critical) conflicts
        int depValue1 = CONFLICT_PENALTY * conflicts1 + CROSSING_PENALTY * crossings1;
        int depValue2 = CONFLICT_PENALTY * conflicts2 + CROSSING_PENALTY * crossings2;

        if (depValue1 < depValue2) {
            // hyperedge 1 wants to be left of hyperedge 2
            HyperEdgeSegmentDependency::createAndAddRegular(he1, he2, depValue2 - depValue1);
        } else if (depValue1 > depValue2) {
            // hyperedge 2 wants to be left of hyperedge 1
            HyperEdgeSegmentDependency::createAndAddRegular(he2, he1, depValue1 - depValue2);
        } else if (depValue1 > 0 && depValue2 > 0) {
            // Create two dependencies with zero weight
            HyperEdgeSegmentDependency::createAndAddRegular(he1, he2, 0);
            HyperEdgeSegmentDependency::createAndAddRegular(he2, he1, 0);
        }
    }

    return criticalDependencyCount;
}

int OrthogonalRoutingGenerator::countConflicts(const std::list<double>& posis1,
                                                const std::list<double>& posis2) {
    int conflicts = 0;

    if (!posis1.empty() && !posis2.empty()) {
        auto iter1 = posis1.begin();
        auto iter2 = posis2.begin();
        double pos1 = *iter1;
        double pos2 = *iter2;
        bool hasMore = true;

        do {
            if (pos1 > pos2 - criticalConflictThreshold_ && pos1 < pos2 + criticalConflictThreshold_) {
                // We're done as soon as we find a single critical conflict
                return CRITICAL_CONFLICTS_DETECTED;
            } else if (pos1 > pos2 - conflictThreshold_ && pos1 < pos2 + conflictThreshold_) {
                conflicts++;
            }

            if (pos1 <= pos2 && ++iter1 != posis1.end()) {
                pos1 = *iter1;
            } else if (pos2 <= pos1 && ++iter2 != posis2.end()) {
                pos2 = *iter2;
            } else {
                hasMore = false;
            }
        } while (hasMore);
    }

    return conflicts;
}

int OrthogonalRoutingGenerator::countCrossings(const std::list<double>& posis,
                                                double start,
                                                double end) {
    int crossings = 0;
    for (double pos : posis) {
        if (pos > end) {
            break;
        } else if (pos >= start) {
            crossings++;
        }
    }
    return crossings;
}

///////////////////////////////////////////////////////////////////////////////
// Cycle Breaking

void OrthogonalRoutingGenerator::breakCriticalCycles(std::vector<HyperEdgeSegment*>& edgeSegments,
                                                      std::mt19937& random) {
    std::vector<HyperEdgeSegmentDependency*> cycleDependencies =
        HyperEdgeCycleDetector::detectCycles(edgeSegments, true, random);

    // Lazy initialization
    if (segmentSplitter_ == nullptr) {
        segmentSplitter_ = new HyperEdgeSegmentSplitter(this);
    }

    segmentSplitter_->splitSegments(cycleDependencies, edgeSegments, criticalConflictThreshold_);
}

void OrthogonalRoutingGenerator::breakNonCriticalCycles(std::vector<HyperEdgeSegment*>& edgeSegments,
                                                         std::mt19937& random) {
    std::vector<HyperEdgeSegmentDependency*> cycleDependencies =
        HyperEdgeCycleDetector::detectCycles(edgeSegments, false, random);

    for (HyperEdgeSegmentDependency* cycleDependency : cycleDependencies) {
        if (cycleDependency->getWeight() == 0) {
            // Simply remove this dependency. This assumes that only two-cycles will have dependency weight 0
            cycleDependency->remove();
        } else {
            cycleDependency->reverse();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Topological Ordering

void OrthogonalRoutingGenerator::topologicalNumbering(std::vector<HyperEdgeSegment*>& segments) {
    // Determine sources, targets, incoming count and outgoing count; targets are only
    // added to the list if they only connect westward ports (that is, if all their
    // horizontal segments point to the right)
    std::vector<HyperEdgeSegment*> sources;
    std::vector<HyperEdgeSegment*> rightwardTargets;

    for (HyperEdgeSegment* node : segments) {
        node->setInWeight(static_cast<int>(node->getIncomingSegmentDependencies().size()));
        node->setOutWeight(static_cast<int>(node->getOutgoingSegmentDependencies().size()));

        if (node->getInWeight() == 0) {
            sources.push_back(node);
        }

        if (node->getOutWeight() == 0 && node->getIncomingConnectionCoordinates().empty()) {
            rightwardTargets.push_back(node);
        }
    }

    int maxRank = -1;

    // Assign ranks using topological numbering
    while (!sources.empty()) {
        HyperEdgeSegment* node = sources.front();
        sources.erase(sources.begin());

        for (HyperEdgeSegmentDependency* dep : node->getOutgoingSegmentDependencies()) {
            HyperEdgeSegment* target = dep->getTarget();
            target->setRoutingSlot(std::max(target->getRoutingSlot(), node->getRoutingSlot() + 1));
            maxRank = std::max(maxRank, target->getRoutingSlot());

            target->setInWeight(target->getInWeight() - 1);
            if (target->getInWeight() == 0) {
                sources.push_back(target);
            }
        }
    }

    /* If we stopped here, hyperedge segments that don't have any horizontal segments pointing
     * leftward would be ranked just like every other hyperedge segments. This would move back
     * edges too far away from their target node. To remedy that, we move all hyperedge segments
     * with horizontal segments only pointing rightwards as far right as possible.
     */
    if (maxRank > -1) {
        // Assign all target nodes with horizontal segments pointing to the right the
        // rightmost rank
        for (HyperEdgeSegment* node : rightwardTargets) {
            node->setRoutingSlot(maxRank);
        }

        // Let all other segments with horizontal segments pointing rightwards move as
        // far right as possible
        while (!rightwardTargets.empty()) {
            HyperEdgeSegment* node = rightwardTargets.front();
            rightwardTargets.erase(rightwardTargets.begin());

            // The node only has connections to western ports
            for (HyperEdgeSegmentDependency* dep : node->getIncomingSegmentDependencies()) {
                HyperEdgeSegment* source = dep->getSource();
                if (!source->getIncomingConnectionCoordinates().empty()) {
                    continue;
                }

                source->setRoutingSlot(std::min(source->getRoutingSlot(), node->getRoutingSlot() - 1));

                source->setOutWeight(source->getOutWeight() - 1);
                if (source->getOutWeight() == 0) {
                    rightwardTargets.push_back(source);
                }
            }
        }
    }
}

} // namespace p5edges
} // namespace layered
} // namespace elk
