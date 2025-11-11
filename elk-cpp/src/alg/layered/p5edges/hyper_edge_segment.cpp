// Eclipse Layout Kernel - C++ Port
// Hyperedge segment implementation
// SPDX-License-Identifier: EPL-2.0

#include "../../../../include/elk/alg/layered/p5edges/hyper_edge_segment.h"
#include "../../../../include/elk/alg/layered/p5edges/base_routing_direction_strategy.h"
#include <algorithm>
#include <sstream>

namespace elk {
namespace layered {
namespace p5edges {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initialization

HyperEdgeSegment::HyperEdgeSegment(BaseRoutingDirectionStrategy* routingStrategy)
    : routingStrategy_(routingStrategy) {
}

void HyperEdgeSegment::addPortPositions(LPort* port, std::map<LPort*, HyperEdgeSegment*>& hyperEdgeSegmentMap) {
    hyperEdgeSegmentMap[port] = this;
    ports_.push_back(port);
    double portPos = routingStrategy_->getPortPositionOnHyperNode(port);

    // Add the new port position to the respective list
    if (port->side == routingStrategy_->getSourcePortSide()) {
        insertSorted(incomingConnectionCoordinates_, portPos);
    } else {
        insertSorted(outgoingConnectionCoordinates_, portPos);
    }

    // Update start and end coordinates
    recomputeExtent();

    // Add connected ports recursively
    for (LPort* otherPort : port->getConnectedPorts()) {
        if (hyperEdgeSegmentMap.find(otherPort) == hyperEdgeSegmentMap.end()) {
            addPortPositions(otherPort, hyperEdgeSegmentMap);
        }
    }
}

void HyperEdgeSegment::insertSorted(std::list<double>& list, double value) {
    auto it = list.begin();
    while (it != list.end()) {
        double next = *it;
        if (next == value) {
            // An exactly equal value is already present in the list
            return;
        } else if (next > value) {
            break;
        }
        ++it;
    }
    list.insert(it, value);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utilities

void HyperEdgeSegment::recomputeExtent() {
    startPosition_ = NAN;
    endPosition_ = NAN;

    recomputeExtent(incomingConnectionCoordinates_);
    recomputeExtent(outgoingConnectionCoordinates_);
}

void HyperEdgeSegment::recomputeExtent(const std::list<double>& positions) {
    // This code assumes that the positions are sorted ascendingly
    if (!positions.empty()) {
        // Set new start position
        if (std::isnan(startPosition_)) {
            startPosition_ = positions.front();
        } else {
            startPosition_ = std::min(startPosition_, positions.front());
        }

        // Set new end position
        if (std::isnan(endPosition_)) {
            endPosition_ = positions.back();
        } else {
            endPosition_ = std::max(endPosition_, positions.back());
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Splitting

std::pair<HyperEdgeSegment*, HyperEdgeSegment*> HyperEdgeSegment::simulateSplit() {
    HyperEdgeSegment* newSplit = new HyperEdgeSegment(routingStrategy_);
    HyperEdgeSegment* newSplitPartner = new HyperEdgeSegment(routingStrategy_);

    newSplit->incomingConnectionCoordinates_ = incomingConnectionCoordinates_;
    newSplit->splitBy_ = splitBy_;
    newSplit->splitPartner_ = newSplitPartner;
    newSplit->recomputeExtent();

    newSplitPartner->outgoingConnectionCoordinates_ = outgoingConnectionCoordinates_;
    newSplitPartner->splitPartner_ = newSplit;
    newSplitPartner->recomputeExtent();

    return std::make_pair(newSplit, newSplitPartner);
}

HyperEdgeSegment* HyperEdgeSegment::splitAt(double splitPosition) {
    splitPartner_ = new HyperEdgeSegment(routingStrategy_);
    splitPartner_->setSplitPartner(this);

    // Move all target positions over to the new segment
    splitPartner_->outgoingConnectionCoordinates_ = outgoingConnectionCoordinates_;
    outgoingConnectionCoordinates_.clear();

    // Link the two
    outgoingConnectionCoordinates_.push_back(splitPosition);
    splitPartner_->incomingConnectionCoordinates_.push_back(splitPosition);

    // Recompute their outer coordinates
    recomputeExtent();
    splitPartner_->recomputeExtent();

    // Clear dependencies so they can be regenerated later. We could try to be smart about
    // updating them, but that would be more complicated code, so this will do just fine
    while (!incomingSegmentDependencies_.empty()) {
        incomingSegmentDependencies_[0]->remove();
    }

    while (!outgoingSegmentDependencies_.empty()) {
        outgoingSegmentDependencies_[0]->remove();
    }

    return splitPartner_;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String Representation

std::string HyperEdgeSegment::toString() const {
    std::ostringstream builder;
    builder << "{";

    for (size_t i = 0; i < ports_.size(); i++) {
        LPort* port = ports_[i];
        LNode* node = port->getNode();

        // Try to get a meaningful name
        std::string name;
        if (node && node->originalNode) {
            name = node->originalNode->id;
        } else if (node) {
            name = "n" + std::to_string(node->getIndex());
        } else {
            name = "?";
        }

        builder << name;
        if (i < ports_.size() - 1) {
            builder << ',';
        }
    }

    builder << '}';
    return builder.str();
}

} // namespace p5edges
} // namespace layered
} // namespace elk
