// Eclipse Layout Kernel - C++ Port
// Advanced configuration system
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "types.h"
#include "properties.h"
#include <string>
#include <unordered_map>
#include <functional>

namespace elk {

// ============================================================================
// Configuration Property System
// ============================================================================

class LayoutOption {
public:
    std::string id;
    std::string name;
    std::string description;
    std::any defaultValue;
    std::any value;

    template<typename T>
    T get() const {
        if (value.has_value()) {
            return std::any_cast<T>(value);
        }
        return std::any_cast<T>(defaultValue);
    }

    template<typename T>
    void set(const T& val) {
        value = val;
    }
};

class ConfigurationStore {
public:
    void registerOption(const LayoutOption& option);
    LayoutOption* getOption(const std::string& id);

    template<typename T>
    T getValue(const std::string& id, const T& defaultVal) const {
        auto it = options_.find(id);
        if (it != options_.end()) {
            return it->second.get<T>();
        }
        return defaultVal;
    }

    template<typename T>
    void setValue(const std::string& id, const T& value) {
        auto it = options_.find(id);
        if (it != options_.end()) {
            it->second.set(value);
        }
    }

private:
    std::unordered_map<std::string, LayoutOption> options_;
};

// ============================================================================
// Global Configuration
// ============================================================================

class GlobalConfig {
public:
    static ConfigurationStore& instance();

    // Common options
    static const std::string ALGORITHM;
    static const std::string DIRECTION;
    static const std::string NODE_SPACING;
    static const std::string LAYER_SPACING;
    static const std::string EDGE_ROUTING;

private:
    static ConfigurationStore store_;
};

} // namespace elk
