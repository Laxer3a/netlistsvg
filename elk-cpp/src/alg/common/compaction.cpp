// Eclipse Layout Kernel - C++ Port
// Graph compaction implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/alg/common/compaction.h"
#include <algorithm>
#include <limits>
#include <cmath>

namespace elk {
namespace compaction {

// ============================================================================
// GraphCompactor Implementation
// ============================================================================

void GraphCompactor::compact(Node* graph, const CompactionConfig& config) {
    if (!graph) return;

    if (config.direction == CompactionDirection::HORIZONTAL ||
        config.direction == CompactionDirection::BOTH) {
        compactHorizontal(graph, config);
    }

    if (config.direction == CompactionDirection::VERTICAL ||
        config.direction == CompactionDirection::BOTH) {
        compactVertical(graph, config);
    }
}

void GraphCompactor::compactHorizontal(Node* graph, const CompactionConfig& config) {
    if (!graph || graph->children.empty()) return;

    if (config.strategy == CompactionStrategy::SCANLINE) {
        ScanlineCompactor::compact(graph, CompactionDirection::HORIZONTAL, config);
    } else if (config.strategy == CompactionStrategy::QUADRATIC) {
        QuadraticCompactor::compact(graph, config);
    } else {
        // Simple gap removal
        removeGaps(graph, config.minNodeSpacing);
    }
}

void GraphCompactor::compactVertical(Node* graph, const CompactionConfig& config) {
    if (!graph || graph->children.empty()) return;

    if (config.strategy == CompactionStrategy::SCANLINE) {
        ScanlineCompactor::compact(graph, CompactionDirection::VERTICAL, config);
    } else {
        removeGaps(graph, config.minNodeSpacing);
    }
}

void GraphCompactor::removeGaps(Node* graph, double minSpacing) {
    if (!graph || graph->children.empty()) return;

    // Horizontal compaction
    auto nodesX = sortNodesByPosition(graph, true);

    for (size_t i = 1; i < nodesX.size(); ++i) {
        Node* current = nodesX[i];
        double targetX = 0;

        // Find rightmost blocking node
        for (size_t j = 0; j < i; ++j) {
            Node* other = nodesX[j];

            // Check if nodes overlap vertically
            if (other->position.y + other->size.height > current->position.y &&
                other->position.y < current->position.y + current->size.height) {

                double blockingX = other->position.x + other->size.width + minSpacing;
                targetX = std::max(targetX, blockingX);
            }
        }

        if (targetX < current->position.x) {
            current->position.x = targetX;
        }
    }

    // Vertical compaction
    auto nodesY = sortNodesByPosition(graph, false);

    for (size_t i = 1; i < nodesY.size(); ++i) {
        Node* current = nodesY[i];
        double targetY = 0;

        // Find lowest blocking node
        for (size_t j = 0; j < i; ++j) {
            Node* other = nodesY[j];

            // Check if nodes overlap horizontally
            if (other->position.x + other->size.width > current->position.x &&
                other->position.x < current->position.x + current->size.width) {

                double blockingY = other->position.y + other->size.height + minSpacing;
                targetY = std::max(targetY, blockingY);
            }
        }

        if (targetY < current->position.y) {
            current->position.y = targetY;
        }
    }
}

std::vector<Node*> GraphCompactor::sortNodesByPosition(Node* graph, bool horizontal) {
    std::vector<Node*> nodes;
    for (auto& child : graph->children) {
        nodes.push_back(child.get());
    }

    if (horizontal) {
        std::sort(nodes.begin(), nodes.end(),
                  [](Node* a, Node* b) { return a->position.x < b->position.x; });
    } else {
        std::sort(nodes.begin(), nodes.end(),
                  [](Node* a, Node* b) { return a->position.y < b->position.y; });
    }

    return nodes;
}

// ============================================================================
// ScanlineCompactor Implementation
// ============================================================================

void ScanlineCompactor::compact(
    Node* graph,
    CompactionDirection direction,
    const CompactionConfig& config) {

    if (!graph || graph->children.empty()) return;

    bool horizontal = (direction == CompactionDirection::HORIZONTAL);

    // Generate events
    auto events = generateEvents(graph, horizontal);
    if (events.empty()) return;

    // Sort events by position
    std::sort(events.begin(), events.end());

    // Process scanline
    processScanline(events, config, horizontal);
}

std::vector<ScanlineCompactor::Event> ScanlineCompactor::generateEvents(
    Node* graph,
    bool horizontal) {

    std::vector<Event> events;

    for (auto& child : graph->children) {
        Node* node = child.get();

        Event startEvent;
        startEvent.type = Event::START;
        startEvent.node = node;

        Event endEvent;
        endEvent.type = Event::END;
        endEvent.node = node;

        if (horizontal) {
            startEvent.position = node->position.x;
            endEvent.position = node->position.x + node->size.width;
        } else {
            startEvent.position = node->position.y;
            endEvent.position = node->position.y + node->size.height;
        }

        events.push_back(startEvent);
        events.push_back(endEvent);
    }

    return events;
}

void ScanlineCompactor::processScanline(
    const std::vector<Event>& events,
    const CompactionConfig& config,
    bool horizontal) {

    std::vector<Node*> activeNodes;
    double compactionOffset = 0.0;

    for (size_t i = 0; i < events.size(); ++i) {
        const Event& event = events[i];

        if (event.type == Event::START) {
            // Node starts - check for compaction opportunity
            double currentPos = horizontal ? event.node->position.x : event.node->position.y;
            double targetPos = currentPos - compactionOffset;

            // Ensure minimum spacing from active nodes
            for (Node* active : activeNodes) {
                double activeEnd = horizontal ?
                    (active->position.x + active->size.width) :
                    (active->position.y + active->size.height);

                // Check if nodes overlap in other dimension
                bool overlaps = false;
                if (horizontal) {
                    overlaps = !(active->position.y + active->size.height <= event.node->position.y ||
                                active->position.y >= event.node->position.y + event.node->size.height);
                } else {
                    overlaps = !(active->position.x + active->size.width <= event.node->position.x ||
                                active->position.x >= event.node->position.x + event.node->size.width);
                }

                if (overlaps) {
                    targetPos = std::max(targetPos, activeEnd + config.minNodeSpacing);
                }
            }

            // Apply compaction
            double actualOffset = currentPos - targetPos;
            if (actualOffset > 0) {
                if (horizontal) {
                    event.node->position.x = targetPos;
                } else {
                    event.node->position.y = targetPos;
                }
                compactionOffset = actualOffset;
            }

            activeNodes.push_back(event.node);

        } else {
            // Node ends - remove from active set
            auto it = std::find(activeNodes.begin(), activeNodes.end(), event.node);
            if (it != activeNodes.end()) {
                activeNodes.erase(it);
            }
        }
    }
}

// ============================================================================
// QuadraticCompactor Implementation
// ============================================================================

void QuadraticCompactor::compact(Node* graph, const CompactionConfig& config) {
    if (!graph || graph->children.empty()) return;

    std::vector<Node*> nodes;
    for (auto& child : graph->children) {
        nodes.push_back(child.get());
    }

    // Generate constraints
    auto constraints = generateConstraints(graph, config.minNodeSpacing);

    // Solve for horizontal positions
    iterativeSolve(nodes, constraints, config.maxIterations, true);

    // Solve for vertical positions
    iterativeSolve(nodes, constraints, config.maxIterations, false);
}

std::vector<QuadraticCompactor::Constraint> QuadraticCompactor::generateConstraints(
    Node* graph,
    double minSpacing) {

    std::vector<Constraint> constraints;

    for (auto& child1 : graph->children) {
        for (auto& child2 : graph->children) {
            if (child1 == child2) continue;

            Node* n1 = child1.get();
            Node* n2 = child2.get();

            // Check if nodes need spacing constraint
            Rect r1 = n1->getBounds();
            Rect r2 = n2->getBounds();

            // Horizontal constraint
            if (r1.intersects(r2) || rectanglesOverlap(r1, r2, minSpacing)) {
                Constraint c;
                c.node1 = n1;
                c.node2 = n2;
                c.minDistance = minSpacing + n1->size.width;
                c.horizontal = true;
                constraints.push_back(c);
            }

            // Vertical constraint
            if (r1.intersects(r2) || rectanglesOverlap(r1, r2, minSpacing)) {
                Constraint c;
                c.node1 = n1;
                c.node2 = n2;
                c.minDistance = minSpacing + n1->size.height;
                c.horizontal = false;
                constraints.push_back(c);
            }
        }
    }

    return constraints;
}

void QuadraticCompactor::iterativeSolve(
    const std::vector<Node*>& nodes,
    const std::vector<Constraint>& constraints,
    int maxIterations,
    bool horizontal) {

    // Simple iterative constraint satisfaction
    for (int iter = 0; iter < maxIterations; ++iter) {
        bool changed = false;

        for (const auto& c : constraints) {
            if (c.horizontal != horizontal) continue;

            double pos1 = horizontal ? c.node1->position.x : c.node1->position.y;
            double pos2 = horizontal ? c.node2->position.x : c.node2->position.y;

            if (pos2 < pos1 + c.minDistance) {
                double newPos = pos1 + c.minDistance;
                if (horizontal) {
                    c.node2->position.x = newPos;
                } else {
                    c.node2->position.y = newPos;
                }
                changed = true;
            }
        }

        if (!changed) break;
    }
}

// ============================================================================
// RectanglePacker Implementation
// ============================================================================

void RectanglePacker::pack(
    std::vector<Rectangle>& rectangles,
    double containerWidth,
    double spacing) {

    // Use shelf packing by default
    shelfPack(rectangles, containerWidth, spacing);
}

void RectanglePacker::shelfPack(
    std::vector<Rectangle>& rectangles,
    double containerWidth,
    double spacing) {

    // Sort by height (descending)
    std::sort(rectangles.begin(), rectangles.end(),
              [](const Rectangle& a, const Rectangle& b) {
                  return a.bounds.height > b.bounds.height;
              });

    double currentX = 0;
    double currentY = 0;
    double shelfHeight = 0;

    for (auto& rect : rectangles) {
        // Check if rectangle fits on current shelf
        if (currentX + rect.bounds.width > containerWidth && currentX > 0) {
            // Move to next shelf
            currentX = 0;
            currentY += shelfHeight + spacing;
            shelfHeight = 0;
        }

        // Place rectangle
        rect.bounds.x = currentX;
        rect.bounds.y = currentY;

        if (rect.node) {
            rect.node->position.x = currentX;
            rect.node->position.y = currentY;
        }

        currentX += rect.bounds.width + spacing;
        shelfHeight = std::max(shelfHeight, rect.bounds.height);
    }
}

void RectanglePacker::guillotinePack(
    std::vector<Rectangle>& rectangles,
    double containerWidth,
    double spacing) {

    // Simple guillotine packing
    std::vector<Rectangle> placed;

    for (auto& rect : rectangles) {
        Point pos = findBestPosition(rect, placed, containerWidth, spacing);
        rect.bounds.x = pos.x;
        rect.bounds.y = pos.y;

        if (rect.node) {
            rect.node->position = pos;
        }

        placed.push_back(rect);
    }

    rectangles = placed;
}

Point RectanglePacker::findBestPosition(
    const Rectangle& rect,
    const std::vector<Rectangle>& placed,
    double containerWidth,
    double spacing) {

    Point bestPos(0, 0);
    double bestScore = std::numeric_limits<double>::max();

    // Try positions along existing rectangles
    std::vector<Point> candidates = {{0, 0}};

    for (const auto& p : placed) {
        candidates.push_back({p.bounds.right() + spacing, p.bounds.y});
        candidates.push_back({p.bounds.x, p.bounds.bottom() + spacing});
    }

    for (const Point& pos : candidates) {
        Rect testBounds = rect.bounds;
        testBounds.x = pos.x;
        testBounds.y = pos.y;

        if (testBounds.right() > containerWidth) continue;
        if (!isValidPosition(testBounds, placed, spacing)) continue;

        // Score: prefer positions closer to origin
        double score = pos.x + pos.y;
        if (score < bestScore) {
            bestScore = score;
            bestPos = pos;
        }
    }

    return bestPos;
}

bool RectanglePacker::isValidPosition(
    const Rect& bounds,
    const std::vector<Rectangle>& placed,
    double spacing) {

    for (const auto& rect : placed) {
        if (rectanglesOverlap(bounds, rect.bounds, spacing)) {
            return false;
        }
    }
    return true;
}

} // namespace compaction
} // namespace elk
