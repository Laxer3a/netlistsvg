// Eclipse Layout Kernel - C++ Port
// Hyperedge segment dependency
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include <memory>
#include <string>

namespace elk {
namespace layered {
namespace p5edges {

// Forward declaration
class HyperEdgeSegment;

/**
 * A dependency between two HyperEdgeSegments. The dependency is to be interpreted like this:
 * the source segment wants to be in lower routing slot than the target segment. Otherwise,
 * bad things might ensue. What this means depends on the dependency's type:
 *
 * - For REGULAR dependencies, ignoring it will cause the result to deteriorate by the
 *   dependency's weight (which is, for example, the number of additional edge crossings
 *   caused by not honoring this dependency).
 * - For CRITICAL dependencies, ignoring it will cause edge overlaps, which should be
 *   avoided at all cost.
 */
class HyperEdgeSegmentDependency {
public:
    /**
     * Possible types of dependencies between HyperEdgeSegments.
     */
    enum class DependencyType {
        /** Regular dependencies are ones that, if ignored, may cause additional crossings. */
        REGULAR,
        /** Critical dependencies are ones that, if ignored, result in edge overlaps. */
        CRITICAL
    };

    /** Non-zero weight used for critical dependencies. */
    static constexpr int CRITICAL_DEPENDENCY_WEIGHT = 1;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Creation

    /**
     * Creates a regular dependency between the given segments with the given weight,
     * and adds it to the segments' list of incident dependencies.
     */
    static HyperEdgeSegmentDependency* createAndAddRegular(HyperEdgeSegment* source,
                                                           HyperEdgeSegment* target,
                                                           int weight);

    /**
     * Creates a critical dependency between the given segments with a weight of
     * CRITICAL_DEPENDENCY_WEIGHT, and adds it to the segments' list of incident dependencies.
     */
    static HyperEdgeSegmentDependency* createAndAddCritical(HyperEdgeSegment* source,
                                                             HyperEdgeSegment* target);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Manipulation

    /**
     * Removes this dependency from its source and target segments.
     */
    void remove();

    /**
     * Reverses this dependency.
     */
    void reverse();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Getters and Setters

    /**
     * Returns the dependency's type.
     */
    DependencyType getType() const { return type_; }

    /**
     * Return the source segment.
     */
    HyperEdgeSegment* getSource() const { return source_; }

    /**
     * Sets the source segment and updates that segment's list of outgoing dependencies.
     */
    void setSource(HyperEdgeSegment* newSource);

    /**
     * Return the target segment.
     */
    HyperEdgeSegment* getTarget() const { return target_; }

    /**
     * Sets the target segment and updates that segment's list of incoming dependencies.
     */
    void setTarget(HyperEdgeSegment* newTarget);

    /**
     * Returns the weight of this dependency.
     */
    int getWeight() const { return weight_; }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // String representation

    std::string toString() const;

private:
    /**
     * Private constructor - use static factory methods instead.
     */
    HyperEdgeSegmentDependency(DependencyType type, HyperEdgeSegment* source,
                               HyperEdgeSegment* target, int weight);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties

    /** The dependency's type. */
    DependencyType type_;
    /** The source hypernode of this dependency. */
    HyperEdgeSegment* source_;
    /** The target hypernode of this dependency. */
    HyperEdgeSegment* target_;
    /** The weight of this dependency. */
    int weight_;
};

} // namespace p5edges
} // namespace layered
} // namespace elk
