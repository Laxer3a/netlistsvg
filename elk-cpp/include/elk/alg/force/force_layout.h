// Eclipse Layout Kernel - C++ Port
// Force-directed layout algorithm
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../core/layout_provider.h"
#include "../../core/types.h"
#include <vector>
#include <random>

namespace elk {
namespace force {

// ============================================================================
// Force Model Types
// ============================================================================

enum class ForceModel {
    EADES,                 // Spring embedder (Eades 1984)
    FRUCHTERMAN_REINGOLD  // Fruchterman-Reingold (1991)
};

// ============================================================================
// ForceNode - Internal representation for force calculation
// ============================================================================

struct ForceNode {
    Node* node;              // Reference to original graph node
    Point position;          // Current position
    Point velocity;          // Current velocity
    Point force;             // Accumulated force

    ForceNode(Node* n) : node(n), position(n->position) {}
};

// ============================================================================
// ForceEdge - Internal edge representation
// ============================================================================

struct ForceEdge {
    ForceNode* source;
    ForceNode* target;
    double desiredLength = 50.0;

    ForceEdge(ForceNode* src, ForceNode* tgt)
        : source(src), target(tgt) {}
};

// ============================================================================
// ForceLayoutProvider - Main force-directed layout implementation
// ============================================================================

class ForceLayoutProvider : public ILayoutProvider {
public:
    ForceLayoutProvider();

    void layout(Node* graph, ProgressCallback progress = nullptr) override;
    std::string getAlgorithmId() const override { return "elk.force"; }

    // Configuration
    void setModel(ForceModel model) { model_ = model; }
    void setIterations(int iterations) { iterations_ = iterations; }
    void setRepulsion(double repulsion) { repulsion_ = repulsion; }
    void setSpringLength(double length) { springLength_ = length; }
    void setTemperature(double temp) { temperature_ = temp; }

private:
    // Build internal graph representation
    void importGraph(Node* graph, std::vector<ForceNode>& nodes, std::vector<ForceEdge>& edges);

    // Force calculation methods
    void calculateForces(std::vector<ForceNode>& nodes, const std::vector<ForceEdge>& edges);
    void calculateRepulsiveForces(std::vector<ForceNode>& nodes);
    void calculateAttractiveForces(const std::vector<ForceEdge>& edges);

    // Apply forces and update positions
    void updatePositions(std::vector<ForceNode>& nodes, double temperature);

    // Apply results back to graph
    void applyLayout(const std::vector<ForceNode>& nodes);

    // Center and fit graph
    void centerGraph(std::vector<ForceNode>& nodes);
    void normalizePositions(std::vector<ForceNode>& nodes);

    // Configuration
    ForceModel model_ = ForceModel::FRUCHTERMAN_REINGOLD;
    int iterations_ = 300;
    double repulsion_ = 5.0;
    double springLength_ = 80.0;
    double temperature_ = 100.0;

    std::mt19937 rng_;
};

// ============================================================================
// Force calculation helpers
// ============================================================================

// Eades model forces
inline Point calculateEadesRepulsion(const Point& p1, const Point& p2, double repulsion) {
    Point delta = p1 - p2;
    double dist = delta.length();
    if (dist < EPSILON) {
        // Add small random displacement to avoid singularity
        return {(rand() % 100 - 50) * 0.01, (rand() % 100 - 50) * 0.01};
    }
    // Repulsive force: F = k / d²
    double force = repulsion / (dist * dist);
    return delta.normalized() * force;
}

inline Point calculateEadesAttraction(const Point& p1, const Point& p2, double springConstant, double restLength) {
    Point delta = p2 - p1;
    double dist = delta.length();
    if (dist < EPSILON) return {0, 0};

    // Attractive force: F = k * log(d / l)
    double force = springConstant * std::log(dist / restLength);
    return delta.normalized() * force;
}

// Fruchterman-Reingold model forces
inline Point calculateFRRepulsion(const Point& p1, const Point& p2, double k) {
    Point delta = p1 - p2;
    double dist = delta.length();
    if (dist < EPSILON) {
        return {(rand() % 100 - 50) * 0.01, (rand() % 100 - 50) * 0.01};
    }
    // Repulsive force: F = k² / d
    double force = (k * k) / dist;
    return delta.normalized() * force;
}

inline Point calculateFRAttraction(const Point& p1, const Point& p2, double k) {
    Point delta = p2 - p1;
    double dist = delta.length();
    if (dist < EPSILON) return {0, 0};

    // Attractive force: F = d² / k
    double force = (dist * dist) / k;
    return delta.normalized() * force;
}

} // namespace force
} // namespace elk
