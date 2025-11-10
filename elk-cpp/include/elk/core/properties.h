// Eclipse Layout Kernel - C++ Port
// Property system for layout options
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <any>
#include <typeindex>

namespace elk {

// Simple property holder using std::any for type-safe storage
class PropertyHolder {
public:
    template<typename T>
    void setProperty(const std::string& key, const T& value) {
        properties_[key] = value;
    }

    template<typename T>
    T getProperty(const std::string& key, const T& defaultValue = T{}) const {
        auto it = properties_.find(key);
        if (it != properties_.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast&) {
                return defaultValue;
            }
        }
        return defaultValue;
    }

    bool hasProperty(const std::string& key) const {
        return properties_.find(key) != properties_.end();
    }

    void removeProperty(const std::string& key) {
        properties_.erase(key);
    }

    void clearProperties() {
        properties_.clear();
    }

private:
    std::unordered_map<std::string, std::any> properties_;
};

// Common property keys
namespace Properties {
    // Core layout properties
    inline const std::string ALGORITHM = "elk.algorithm";
    inline const std::string DIRECTION = "elk.direction";
    inline const std::string SPACING = "elk.spacing.nodeNode";
    inline const std::string EDGE_ROUTING = "elk.edgeRouting";
    inline const std::string PADDING = "elk.padding";
    inline const std::string ASPECT_RATIO = "elk.aspectRatio";

    // Force-directed properties
    inline const std::string FORCE_MODEL = "elk.force.model";
    inline const std::string FORCE_ITERATIONS = "elk.force.iterations";
    inline const std::string FORCE_TEMPERATURE = "elk.force.temperature";
    inline const std::string FORCE_REPULSION = "elk.force.repulsion";

    // Layered layout properties
    inline const std::string LAYERED_SPACING_NODE = "elk.layered.spacing.nodeNodeBetweenLayers";
    inline const std::string LAYERED_SPACING_EDGE = "elk.layered.spacing.edgeNodeBetweenLayers";
    inline const std::string LAYERED_CROSSING_MINIMIZATION = "elk.layered.crossingMinimization.strategy";
    inline const std::string LAYERED_NODE_PLACEMENT = "elk.layered.nodePlacement.strategy";
    inline const std::string LAYERED_CYCLE_BREAKING = "elk.layered.cycleBreaking.strategy";
}

} // namespace elk
