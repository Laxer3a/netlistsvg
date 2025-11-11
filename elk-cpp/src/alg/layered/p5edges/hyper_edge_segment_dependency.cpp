// Eclipse Layout Kernel - C++ Port
// Hyperedge segment dependency implementation
// SPDX-License-Identifier: EPL-2.0

#include "../../../../include/elk/alg/layered/p5edges/hyper_edge_segment_dependency.h"
#include "../../../../include/elk/alg/layered/p5edges/hyper_edge_segment.h"
#include <algorithm>

namespace elk {
namespace layered {
namespace p5edges {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private Constructor

HyperEdgeSegmentDependency::HyperEdgeSegmentDependency(DependencyType type, HyperEdgeSegment* source,
                                                       HyperEdgeSegment* target, int weight)
    : type_(type), source_(nullptr), target_(nullptr), weight_(weight) {
    setSource(source);
    setTarget(target);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static Factory Methods

HyperEdgeSegmentDependency* HyperEdgeSegmentDependency::createAndAddRegular(HyperEdgeSegment* source,
                                                                             HyperEdgeSegment* target,
                                                                             int weight) {
    return new HyperEdgeSegmentDependency(DependencyType::REGULAR, source, target, weight);
}

HyperEdgeSegmentDependency* HyperEdgeSegmentDependency::createAndAddCritical(HyperEdgeSegment* source,
                                                                              HyperEdgeSegment* target) {
    return new HyperEdgeSegmentDependency(DependencyType::CRITICAL, source, target, CRITICAL_DEPENDENCY_WEIGHT);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Manipulation

void HyperEdgeSegmentDependency::remove() {
    setSource(nullptr);
    setTarget(nullptr);
}

void HyperEdgeSegmentDependency::reverse() {
    HyperEdgeSegment* oldSource = source_;
    HyperEdgeSegment* oldTarget = target_;

    setSource(oldTarget);
    setTarget(oldSource);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Getters and Setters

void HyperEdgeSegmentDependency::setSource(HyperEdgeSegment* newSource) {
    if (source_ != nullptr) {
        auto& deps = source_->getOutgoingSegmentDependencies();
        deps.erase(std::remove(deps.begin(), deps.end(), this), deps.end());
    }

    source_ = newSource;

    if (source_ != nullptr) {
        source_->getOutgoingSegmentDependencies().push_back(this);
    }
}

void HyperEdgeSegmentDependency::setTarget(HyperEdgeSegment* newTarget) {
    if (target_ != nullptr) {
        auto& deps = target_->getIncomingSegmentDependencies();
        deps.erase(std::remove(deps.begin(), deps.end(), this), deps.end());
    }

    target_ = newTarget;

    if (target_ != nullptr) {
        target_->getIncomingSegmentDependencies().push_back(this);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String Representation

std::string HyperEdgeSegmentDependency::toString() const {
    std::string result = "Dependency(";
    // Note: Will need to implement HyperEdgeSegment::toString() to complete this
    result += (type_ == DependencyType::REGULAR ? "REGULAR" : "CRITICAL");
    result += ")";
    return result;
}

} // namespace p5edges
} // namespace layered
} // namespace elk
