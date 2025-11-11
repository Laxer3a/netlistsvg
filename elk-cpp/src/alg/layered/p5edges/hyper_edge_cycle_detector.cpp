// Eclipse Layout Kernel - C++ Port
// Hyperedge cycle detector implementation
// SPDX-License-Identifier: EPL-2.0

#include "../../../../include/elk/alg/layered/p5edges/hyper_edge_cycle_detector.h"
#include <set>
#include <climits>
#include <random>

namespace elk {
namespace layered {
namespace p5edges {

std::vector<HyperEdgeSegmentDependency*> HyperEdgeCycleDetector::detectCycles(
        const std::vector<HyperEdgeSegment*>& segments,
        bool criticalOnly,
        std::mt19937& random) {

    std::vector<HyperEdgeSegmentDependency*> result;

    std::list<HyperEdgeSegment*> sources;
    std::list<HyperEdgeSegment*> sinks;

    // Initialize values for the algorithm
    initialize(segments, sources, sinks, criticalOnly);

    // Assign marks to all nodes
    computeLinearOrderingMarks(segments, sources, sinks, criticalOnly, random);

    // Process edges that point left: remove those of zero weight, reverse the others
    for (HyperEdgeSegment* source : segments) {
        for (HyperEdgeSegmentDependency* outDependency : source->getOutgoingSegmentDependencies()) {
            // Only consider critical dependencies, if required
            if (!criticalOnly || outDependency->getType() == HyperEdgeSegmentDependency::DependencyType::CRITICAL) {
                if (source->mark > outDependency->getTarget()->mark) {
                    result.push_back(outDependency);
                }
            }
        }
    }

    return result;
}

void HyperEdgeCycleDetector::initialize(
        const std::vector<HyperEdgeSegment*>& segments,
        std::list<HyperEdgeSegment*>& sources,
        std::list<HyperEdgeSegment*>& sinks,
        bool criticalOnly) {

    int nextMark = -1;
    for (HyperEdgeSegment* segment : segments) {
        segment->mark = nextMark--;

        // Sum up the weights of our critical dependencies
        int criticalInWeight = 0;
        for (const auto* dep : segment->getIncomingSegmentDependencies()) {
            if (dep->getType() == HyperEdgeSegmentDependency::DependencyType::CRITICAL) {
                criticalInWeight += dep->getWeight();
            }
        }

        int criticalOutWeight = 0;
        for (const auto* dep : segment->getOutgoingSegmentDependencies()) {
            if (dep->getType() == HyperEdgeSegmentDependency::DependencyType::CRITICAL) {
                criticalOutWeight += dep->getWeight();
            }
        }

        // If we're only considering critical dependencies, we'll ignore the others
        int inWeight = criticalInWeight;
        int outWeight = criticalOutWeight;

        if (!criticalOnly) {
            // Just sum up everything
            inWeight = 0;
            for (const auto* dep : segment->getIncomingSegmentDependencies()) {
                inWeight += dep->getWeight();
            }

            outWeight = 0;
            for (const auto* dep : segment->getOutgoingSegmentDependencies()) {
                outWeight += dep->getWeight();
            }
        }

        // Apply the weight
        segment->setInWeight(inWeight);
        segment->setCriticalInWeight(criticalInWeight);
        segment->setOutWeight(outWeight);
        segment->setCriticalOutWeight(criticalOutWeight);

        // Add the segment to either sources or sinks if the corresponding weight is zero
        if (outWeight == 0) {
            sinks.push_back(segment);
        } else if (inWeight == 0) {
            sources.push_back(segment);
        }
    }
}

void HyperEdgeCycleDetector::computeLinearOrderingMarks(
        const std::vector<HyperEdgeSegment*>& segments,
        std::list<HyperEdgeSegment*>& sources,
        std::list<HyperEdgeSegment*>& sinks,
        bool criticalOnly,
        std::mt19937& random) {

    // Use std::set which keeps elements sorted (like Java's TreeSet)
    std::set<HyperEdgeSegment*> unprocessed(segments.begin(), segments.end());
    std::vector<HyperEdgeSegment*> maxSegments;

    // We'll mark sinks with marks < markBase and sources with marks > markBase. Sink marks will later be offset to
    // be higher than the source marks, but this way, the sink marks will reflect the order in which the sinks were
    // discovered and added.
    int markBase = static_cast<int>(segments.size());
    int nextSinkMark = markBase - 1;
    int nextSourceMark = markBase + 1;

    while (!unprocessed.empty()) {
        while (!sinks.empty()) {
            HyperEdgeSegment* sink = sinks.front();
            sinks.pop_front();
            unprocessed.erase(sink);
            sink->mark = nextSinkMark--;
            updateNeighbors(sink, sources, sinks, criticalOnly);
        }

        while (!sources.empty()) {
            HyperEdgeSegment* source = sources.front();
            sources.pop_front();
            unprocessed.erase(source);
            source->mark = nextSourceMark++;
            updateNeighbors(source, sources, sinks, criticalOnly);
        }

        // If any segments are still unprocessed, they are neither source nor sink. Assemble the list of segments
        // with the highest out flow (out weight - in weight), to be placed among the sources. If we're looking at
        // both, critical and non-critical dependencies, we must be sure that critical dependencies will always
        // point rightwards; we thus stop immediately once we find one
        int maxOutflow = INT_MIN;
        for (HyperEdgeSegment* segment : unprocessed) {
            // If we're not only regarding critical dependencies, we must ensure that critical dependencies will
            // always point to the right to prevent them from being reversed later on. Thus, once we find a single
            // segment that still has an outgoing critical dependency and no incoming ones, we'll take that and
            // leave!
            if (!criticalOnly && segment->getCriticalOutWeight() > 0 && segment->getCriticalInWeight() <= 0) {
                maxSegments.clear();
                maxSegments.push_back(segment);
                break;
            }

            int outflow = segment->getOutWeight() - segment->getInWeight();
            if (outflow >= maxOutflow) {
                if (outflow > maxOutflow) {
                    maxSegments.clear();
                    maxOutflow = outflow;
                }
                maxSegments.push_back(segment);
            }
        }

        // If there are segments with maximal out flow, select one randomly; this might yield new sources and sinks
        if (!maxSegments.empty()) {
            std::uniform_int_distribution<int> dist(0, static_cast<int>(maxSegments.size()) - 1);
            HyperEdgeSegment* maxNode = maxSegments[dist(random)];
            unprocessed.erase(maxNode);
            maxNode->mark = nextSourceMark++;
            updateNeighbors(maxNode, sources, sinks, criticalOnly);
            maxSegments.clear();
        }
    }

    // Shift ranks that are left of the mark base so that sinks now have higher marks than sources
    int shiftBase = static_cast<int>(segments.size()) + 1;
    for (HyperEdgeSegment* node : segments) {
        if (node->mark < markBase) {
            node->mark = node->mark + shiftBase;
        }
    }
}

void HyperEdgeCycleDetector::updateNeighbors(
        HyperEdgeSegment* node,
        std::list<HyperEdgeSegment*>& sources,
        std::list<HyperEdgeSegment*>& sinks,
        bool criticalOnly) {

    // Process following nodes
    for (HyperEdgeSegmentDependency* dep : node->getOutgoingSegmentDependencies()) {
        if (!criticalOnly || dep->getType() == HyperEdgeSegmentDependency::DependencyType::CRITICAL) {
            HyperEdgeSegment* target = dep->getTarget();
            if (target->mark < 0 && dep->getWeight() > 0) {
                // Remove weight (and possibly critical weight) from the target
                target->setInWeight(target->getInWeight() - dep->getWeight());
                if (dep->getType() == HyperEdgeSegmentDependency::DependencyType::CRITICAL) {
                    target->setCriticalInWeight(target->getCriticalInWeight() - dep->getWeight());
                }

                if (target->getInWeight() <= 0 && target->getOutWeight() > 0) {
                    sources.push_back(target);
                }
            }
        }
    }

    // Process preceding nodes
    for (HyperEdgeSegmentDependency* dep : node->getIncomingSegmentDependencies()) {
        if (!criticalOnly || dep->getType() == HyperEdgeSegmentDependency::DependencyType::CRITICAL) {
            HyperEdgeSegment* source = dep->getSource();
            if (source->mark < 0 && dep->getWeight() > 0) {
                // Remove weight (and possibly critical weight) from the source
                source->setOutWeight(source->getOutWeight() - dep->getWeight());
                if (dep->getType() == HyperEdgeSegmentDependency::DependencyType::CRITICAL) {
                    source->setCriticalOutWeight(source->getCriticalOutWeight() - dep->getWeight());
                }

                if (source->getOutWeight() <= 0 && source->getInWeight() > 0) {
                    sinks.push_back(source);
                }
            }
        }
    }
}

} // namespace p5edges
} // namespace layered
} // namespace elk
