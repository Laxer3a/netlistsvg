// Eclipse Layout Kernel - C++ Port
// Configuration implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/core/configuration.h"

namespace elk {

void ConfigurationStore::registerOption(const LayoutOption& option) {
    options_[option.id] = option;
}

LayoutOption* ConfigurationStore::getOption(const std::string& id) {
    auto it = options_.find(id);
    if (it != options_.end()) {
        return &it->second;
    }
    return nullptr;
}

// Global configuration
ConfigurationStore GlobalConfig::store_;

const std::string GlobalConfig::ALGORITHM = "elk.algorithm";
const std::string GlobalConfig::DIRECTION = "elk.direction";
const std::string GlobalConfig::NODE_SPACING = "elk.spacing.node";
const std::string GlobalConfig::LAYER_SPACING = "elk.spacing.layer";
const std::string GlobalConfig::EDGE_ROUTING = "elk.edgeRouting";

ConfigurationStore& GlobalConfig::instance() {
    return store_;
}

} // namespace elk
