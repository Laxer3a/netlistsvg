// Eclipse Layout Kernel - C++ Port
// Hyperedge segment splitter implementation
// SPDX-License-Identifier: EPL-2.0

#include "../../../../include/elk/alg/layered/p5edges/hyper_edge_segment_splitter.h"
#include <algorithm>
#include <vector>
#include <set>

// Forward declaration - OrthogonalRoutingGenerator will be defined later
namespace elk {
namespace layered {
namespace p5edges {
class OrthogonalRoutingGenerator;
}
}
}

namespace elk {
namespace layered {
namespace p5edges {

// Forward declarations of methods from OrthogonalRoutingGenerator that we'll need
// These will be properly linked when OrthogonalRoutingGenerator is implemented
extern int countCrossings(const std::list<double>& connections, double start, double end);
extern void createDependencyIfNecessary(OrthogonalRoutingGenerator* gen,
                                        HyperEdgeSegment* seg1,
                                        HyperEdgeSegment* seg2);

HyperEdgeSegmentSplitter::HyperEdgeSegmentSplitter(OrthogonalRoutingGenerator* routingGenerator)
    : routingGenerator_(routingGenerator) {
}

void HyperEdgeSegmentSplitter::splitSegments(
        const std::vector<HyperEdgeSegmentDependency*>& dependenciesToResolve,
        std::vector<HyperEdgeSegment*>& segments,
        double criticalConflictThreshold) {

    // Only start preparations if there's going to be things to do
    if (dependenciesToResolve.empty()) {
        return;
    }

    // Collect all relevant spaces between horizontal segments that are large enough to house
    // another horizontal segment without causing additional conflicts
    std::vector<FreeArea> freeAreas = findFreeAreas(segments, criticalConflictThreshold);

    // For each dependency, choose which segment to split
    std::vector<HyperEdgeSegment*> segmentsToSplit = decideWhichSegmentsToSplit(dependenciesToResolve);

    // Split the segments in order from smallest to largest. The smallest ones need to be split
    // first since they have fewer options for where to put their horizontal connecting segments.
    std::sort(segmentsToSplit.begin(), segmentsToSplit.end(),
              [](HyperEdgeSegment* hes1, HyperEdgeSegment* hes2) {
                  return hes1->getLength() < hes2->getLength();
              });

    for (HyperEdgeSegment* segment : segmentsToSplit) {
        split(segment, segments, freeAreas, criticalConflictThreshold);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Finding Space

std::vector<HyperEdgeSegmentSplitter::FreeArea> HyperEdgeSegmentSplitter::findFreeAreas(
        const std::vector<HyperEdgeSegment*>& segments,
        double criticalConflictThreshold) {

    std::vector<FreeArea> freeAreas;

    // Retrieve all positions where hyperedge segments connect to ports, and sort them
    std::vector<double> allCoordinates;

    for (const auto* segment : segments) {
        for (double coord : segment->getIncomingConnectionCoordinates()) {
            allCoordinates.push_back(coord);
        }
        for (double coord : segment->getOutgoingConnectionCoordinates()) {
            allCoordinates.push_back(coord);
        }
    }

    std::sort(allCoordinates.begin(), allCoordinates.end());

    // Go through each pair of coordinates and create free areas for those that are at least
    // twice the critical threshold
    for (size_t i = 1; i < allCoordinates.size(); i++) {
        if (allCoordinates[i] - allCoordinates[i - 1] >= 2 * criticalConflictThreshold) {
            freeAreas.emplace_back(
                allCoordinates[i - 1] + criticalConflictThreshold,
                allCoordinates[i] - criticalConflictThreshold);
        }
    }

    return freeAreas;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Split Segment Decisions

std::vector<HyperEdgeSegment*> HyperEdgeSegmentSplitter::decideWhichSegmentsToSplit(
        const std::vector<HyperEdgeSegmentDependency*>& dependencies) {

    // Use vector to maintain insertion order (like Java's LinkedHashSet)
    std::vector<HyperEdgeSegment*> segmentsToSplit;

    for (HyperEdgeSegmentDependency* dependency : dependencies) {
        HyperEdgeSegment* sourceSegment = dependency->getSource();
        HyperEdgeSegment* targetSegment = dependency->getTarget();

        // If either of the involved segments were already selected for splitting because of
        // another dependency, that's sufficient
        auto contains = [&](HyperEdgeSegment* seg) {
            return std::find(segmentsToSplit.begin(), segmentsToSplit.end(), seg) != segmentsToSplit.end();
        };

        if (contains(sourceSegment) || contains(targetSegment)) {
            continue;
        }

        // One segment will be split, and the other one will be remembered to have caused the split.
        // The latter will have to stay between the two segments the former will be split into,
        // or else we'll have overlaps again
        HyperEdgeSegment* segmentToSplit = sourceSegment;
        HyperEdgeSegment* segmentCausingSplit = targetSegment;

        // This is perfectly fine unless the segment to split represents a hyperedge and the other
        // does not. We prefer splitting regular edges since hyperedges have a higher chance of
        // causing additional crossings.
        if (sourceSegment->representsHyperedge() && !targetSegment->representsHyperedge()) {
            segmentToSplit = targetSegment;
            segmentCausingSplit = sourceSegment;
        }

        segmentsToSplit.push_back(segmentToSplit);
        segmentToSplit->setSplitBy(segmentCausingSplit);
    }

    return segmentsToSplit;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Actual Splitting

void HyperEdgeSegmentSplitter::split(HyperEdgeSegment* segment,
                                      std::vector<HyperEdgeSegment*>& segments,
                                      std::vector<FreeArea>& freeAreas,
                                      double criticalConflictThreshold) {

    // Split the segment at the best position and add the new segment to our list
    double splitPosition = computePositionToSplitAndUpdateFreeAreas(segment, freeAreas, criticalConflictThreshold);
    segments.push_back(segment->splitAt(splitPosition));

    // Update the dependencies to reflect the new situation
    updateDependencies(segment, segments);
}

void HyperEdgeSegmentSplitter::updateDependencies(HyperEdgeSegment* segment,
                                                   const std::vector<HyperEdgeSegment*>& segments) {
    HyperEdgeSegment* splitCausingSegment = segment->getSplitBy();
    HyperEdgeSegment* splitPartner = segment->getSplitPartner();

    // The segment currently has no dependencies at all. We first need for the segments to be
    // ordered like this: segment ---> split-causing segment ---> split partner
    HyperEdgeSegmentDependency::createAndAddCritical(segment, splitCausingSegment);
    HyperEdgeSegmentDependency::createAndAddCritical(splitCausingSegment, splitPartner);

    // Now we just need to re-introduce dependencies to other segments
    for (HyperEdgeSegment* otherSegment : segments) {
        // We already have our dependencies between our three segments involved in the conflict settled
        if (otherSegment != splitCausingSegment && otherSegment != segment && otherSegment != splitPartner) {
            createDependencyIfNecessary(routingGenerator_, otherSegment, segment);
            createDependencyIfNecessary(routingGenerator_, otherSegment, splitPartner);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Split Position Computation

double HyperEdgeSegmentSplitter::computePositionToSplitAndUpdateFreeAreas(
        HyperEdgeSegment* segment,
        std::vector<FreeArea>& freeAreas,
        double criticalConflictThreshold) {

    // We shall iterate over the available areas (which are sorted by ascending positions) and find
    // the index of the first and the last area we could use since they are in our segment's reach
    int firstPossibleAreaIndex = -1;
    int lastPossibleAreaIndex = -1;

    for (int i = 0; i < static_cast<int>(freeAreas.size()); i++) {
        const FreeArea& currArea = freeAreas[i];

        if (currArea.startPosition > segment->getEndCoordinate()) {
            // We're past the possible areas, so stop
            break;
        } else if (currArea.endPosition >= segment->getStartCoordinate()) {
            // We've found a possible area; it might be the first
            if (firstPossibleAreaIndex < 0) {
                firstPossibleAreaIndex = i;
            }

            lastPossibleAreaIndex = i;
        }
    }

    // Determine the position to split the segment at
    double splitPosition = center(segment);

    if (firstPossibleAreaIndex >= 0) {
        // There are areas we can use
        int bestAreaIndex = chooseBestAreaIndex(segment, freeAreas, firstPossibleAreaIndex, lastPossibleAreaIndex);

        // We'll use the best area's centre and update the area list
        splitPosition = center(freeAreas[bestAreaIndex]);
        useArea(freeAreas, bestAreaIndex, criticalConflictThreshold);
    }

    return splitPosition;
}

int HyperEdgeSegmentSplitter::chooseBestAreaIndex(HyperEdgeSegment* segment,
                                                    const std::vector<FreeArea>& freeAreas,
                                                    int fromIndex,
                                                    int toIndex) {

    int bestAreaIndex = fromIndex;

    if (fromIndex < toIndex) {
        // We have more areas to choose from, so rate them and find the best one. We need to simulate
        // splitting the segment so that we can count potential crossings
        auto splitSegments = segment->simulateSplit();
        HyperEdgeSegment* splitSegment = splitSegments.first;
        HyperEdgeSegment* splitPartner = splitSegments.second;

        const FreeArea& bestArea = freeAreas[bestAreaIndex];
        AreaRating bestRating = rateArea(segment, splitSegment, splitPartner, bestArea);

        for (int i = fromIndex + 1; i <= toIndex; i++) {
            // Determine how good our current area is
            const FreeArea& currArea = freeAreas[i];
            AreaRating currRating = rateArea(segment, splitSegment, splitPartner, currArea);

            if (isBetter(currArea, currRating, bestArea, bestRating)) {
                bestRating = currRating;
                bestAreaIndex = i;
            }
        }

        // Clean up simulated segments
        delete splitSegment;
        delete splitPartner;
    }

    return bestAreaIndex;
}

HyperEdgeSegmentSplitter::AreaRating HyperEdgeSegmentSplitter::rateArea(
        HyperEdgeSegment* segment,
        HyperEdgeSegment* splitSegment,
        HyperEdgeSegment* splitPartner,
        const FreeArea& area) {

    // The area's centre would be used to link the two split segments, so we need to add that
    // to their incident connections
    double areaCentre = center(area);

    splitSegment->getOutgoingConnectionCoordinates().clear();
    splitSegment->getOutgoingConnectionCoordinates().push_back(areaCentre);

    splitPartner->getIncomingConnectionCoordinates().clear();
    splitPartner->getIncomingConnectionCoordinates().push_back(areaCentre);

    // We need to count the dependencies and crossings that the split partners would cause with
    // the original segments incident dependencies
    AreaRating rating(0, 0);

    for (HyperEdgeSegmentDependency* dependency : segment->getIncomingSegmentDependencies()) {
        HyperEdgeSegment* otherSegment = dependency->getSource();

        updateConsideringBothOrderings(rating, splitSegment, otherSegment);
        updateConsideringBothOrderings(rating, splitPartner, otherSegment);
    }

    for (HyperEdgeSegmentDependency* dependency : segment->getOutgoingSegmentDependencies()) {
        HyperEdgeSegment* otherSegment = dependency->getTarget();

        updateConsideringBothOrderings(rating, splitSegment, otherSegment);
        updateConsideringBothOrderings(rating, splitPartner, otherSegment);
    }

    // There will be two additional dependencies: splitSegment --> splitBySegment --> splitPartner.
    // The order between the three will not change, so we only have to count crossings for this concrete order
    rating.dependencies += 2;

    rating.crossings += countCrossingsForSingleOrdering(splitSegment, segment->getSplitBy());
    rating.crossings += countCrossingsForSingleOrdering(segment->getSplitBy(), splitPartner);

    return rating;
}

void HyperEdgeSegmentSplitter::updateConsideringBothOrderings(AreaRating& rating,
                                                               HyperEdgeSegment* s1,
                                                               HyperEdgeSegment* s2) {

    int crossingsS1LeftOfS2 = countCrossingsForSingleOrdering(s1, s2);
    int crossingsS2LeftOfS1 = countCrossingsForSingleOrdering(s2, s1);

    if (crossingsS1LeftOfS2 == crossingsS2LeftOfS1) {
        // If the crossings are the same, we're only interested if there are more than 0
        if (crossingsS1LeftOfS2 > 0) {
            // Both orderings generate the same number of crossings (more than 0), so we have a two-cycle
            rating.dependencies += 2;
            rating.crossings += crossingsS1LeftOfS2;
        }
    } else {
        // One order is better than the other, so there will be a single dependency
        rating.dependencies += 1;
        rating.crossings += std::min(crossingsS1LeftOfS2, crossingsS2LeftOfS1);
    }
}

int HyperEdgeSegmentSplitter::countCrossingsForSingleOrdering(HyperEdgeSegment* left,
                                                               HyperEdgeSegment* right) {
    return countCrossings(left->getOutgoingConnectionCoordinates(), right->getStartCoordinate(), right->getEndCoordinate())
         + countCrossings(right->getIncomingConnectionCoordinates(), left->getStartCoordinate(), left->getEndCoordinate());
}

bool HyperEdgeSegmentSplitter::isBetter(const FreeArea& currArea, const AreaRating& currRating,
                                         const FreeArea& bestArea, const AreaRating& bestRating) {

    if (currRating.crossings < bestRating.crossings) {
        // First criterion: number of crossings
        return true;

    } else if (currRating.crossings == bestRating.crossings) {
        if (currRating.dependencies < bestRating.dependencies) {
            // Second criterion: number of dependencies
            return true;

        } else if (currRating.dependencies == bestRating.dependencies) {
            if (currArea.size > bestArea.size) {
                // Third criterion: size
                return true;
            }
        }
    }

    return false;
}

void HyperEdgeSegmentSplitter::useArea(std::vector<FreeArea>& freeAreas,
                                        int usedAreaIndex,
                                        double criticalConflictThreshold) {

    FreeArea oldArea = freeAreas[usedAreaIndex];
    freeAreas.erase(freeAreas.begin() + usedAreaIndex);

    if (oldArea.size / 2 >= criticalConflictThreshold) {
        // We will probably insert new areas. Keep track of where to insert them
        int insertIndex = usedAreaIndex;

        // This area is large enough to split and still have enough space to the position we're
        // now using for our latest segment
        double oldAreaCentre = center(oldArea);

        // Create the two new areas (and be doubly sure that double precision does not bite us)
        double newEnd1 = oldAreaCentre - criticalConflictThreshold;
        if (oldArea.startPosition <= oldAreaCentre - criticalConflictThreshold) {
            freeAreas.insert(freeAreas.begin() + insertIndex++, FreeArea(oldArea.startPosition, newEnd1));
        }

        double newStart2 = oldAreaCentre + criticalConflictThreshold;
        if (newStart2 <= oldArea.endPosition) {
            freeAreas.insert(freeAreas.begin() + insertIndex, FreeArea(newStart2, oldArea.endPosition));
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility Methods

double HyperEdgeSegmentSplitter::center(HyperEdgeSegment* s) {
    return center(s->getStartCoordinate(), s->getEndCoordinate());
}

double HyperEdgeSegmentSplitter::center(const FreeArea& a) {
    return center(a.startPosition, a.endPosition);
}

double HyperEdgeSegmentSplitter::center(double p1, double p2) {
    return (p1 + p2) / 2;
}

} // namespace p5edges
} // namespace layered
} // namespace elk
