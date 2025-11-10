// Eclipse Layout Kernel - C++ Port
// RectPacking layout algorithm for disconnected components
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../core/layout_provider.h"
#include "../../core/types.h"
#include <vector>
#include <algorithm>

namespace elk {
namespace rectpacking {

// ============================================================================
// Rectangle Packing Types
// ============================================================================

struct PackingRectangle {
    Node* node = nullptr;
    Rect bounds;              // Current bounds (x, y, width, height)
    double area = 0.0;
    int id = 0;
    bool placed = false;
};

// ============================================================================
// RectPacking Configuration
// ============================================================================

enum class PackingStrategy {
    SIMPLE_ROW,           // Simple left-to-right, top-to-bottom row packing
    SIMPLE_COLUMN,        // Simple top-to-bottom, left-to-right column packing
    ASPECT_RATIO,         // Try to maintain aspect ratio
    AREA_BASED            // Pack by area (largest first)
};

enum class PackingAlgorithm {
    SHELF_NEXT_FIT,       // Shelf algorithm: Next Fit - simple horizontal shelves
    SHELF_FIRST_FIT,      // Shelf algorithm: First Fit - find first shelf that fits
    SHELF_BEST_FIT,       // Shelf algorithm: Best Fit - find shelf with least waste
    GUILLOTINE,           // Guillotine algorithm - recursive space subdivision
    MAXRECTS              // MaxRects algorithm - track all free rectangles
};

enum class ExpandDirection {
    DOWN_RIGHT,           // Expand down first, then right
    RIGHT_DOWN,           // Expand right first, then down
    BALANCED              // Try to keep aspect ratio balanced
};

struct RectPackingConfig {
    PackingStrategy strategy = PackingStrategy::AREA_BASED;
    PackingAlgorithm algorithm = PackingAlgorithm::MAXRECTS;
    ExpandDirection expandDirection = ExpandDirection::BALANCED;

    double nodeSpacing = 15.0;        // Spacing between rectangles
    double targetAspectRatio = 1.6;   // Target width/height ratio (golden ratio)
    double aspectRatioTolerance = 0.5; // How much deviation from target is acceptable

    bool sortByArea = true;            // Sort rectangles by area (largest first)
    bool sortByWidth = false;          // Alternative: sort by width
    bool sortByHeight = false;         // Alternative: sort by height
    bool attemptRotation = false;      // Try rotating rectangles 90° for better fit

    bool compactResult = true;         // Run compaction pass after placement
    bool optimizeWhitespace = true;    // Try to minimize whitespace
};

// ============================================================================
// Shelf Packing Structures
// ============================================================================

struct Shelf {
    double y = 0.0;              // Y position of shelf
    double height = 0.0;         // Height of tallest item on shelf
    double usedWidth = 0.0;      // Width currently used
    std::vector<PackingRectangle*> items;
};

// ============================================================================
// Free Rectangle (for MaxRects algorithm)
// ============================================================================

struct FreeRectangle {
    Rect rect;
    int score = 0;

    FreeRectangle(const Rect& r) : rect(r) {}
    FreeRectangle(double x, double y, double w, double h) : rect{x, y, w, h} {}
};

// ============================================================================
// RectPacking Layout Provider
// ============================================================================

class RectPackingLayoutProvider : public ILayoutProvider {
public:
    RectPackingLayoutProvider();

    void layout(Node* graph, ProgressCallback progress = nullptr) override;
    std::string getAlgorithmId() const override { return "elk.rectpacking"; }

    // Configuration
    void setStrategy(PackingStrategy strategy) { config_.strategy = strategy; }
    void setAlgorithm(PackingAlgorithm algorithm) { config_.algorithm = algorithm; }
    void setNodeSpacing(double spacing) { config_.nodeSpacing = spacing; }
    void setTargetAspectRatio(double ratio) { config_.targetAspectRatio = ratio; }
    void setExpandDirection(ExpandDirection dir) { config_.expandDirection = dir; }

private:
    // Phase 1: Extract and prepare rectangles
    std::vector<PackingRectangle> extractRectangles(Node* graph);
    void sortRectangles(std::vector<PackingRectangle>& rects);

    // Phase 2: Pack rectangles using selected algorithm
    void packRectangles(std::vector<PackingRectangle>& rects);

    // Shelf algorithms
    void packShelfNextFit(std::vector<PackingRectangle>& rects);
    void packShelfFirstFit(std::vector<PackingRectangle>& rects);
    void packShelfBestFit(std::vector<PackingRectangle>& rects);

    // Guillotine algorithm
    void packGuillotine(std::vector<PackingRectangle>& rects);
    void guillotineSplit(std::vector<FreeRectangle>& freeRects,
                        const Rect& usedRect, const Rect& freeRect);

    // MaxRects algorithm
    void packMaxRects(std::vector<PackingRectangle>& rects);
    int scoreRectangle(const PackingRectangle& rect, const FreeRectangle& freeRect);
    void splitFreeRectangle(std::vector<FreeRectangle>& freeRects,
                           const Rect& usedRect, const FreeRectangle& freeRect);
    void pruneFreeRectangles(std::vector<FreeRectangle>& freeRects);

    // Simple row/column packing
    void packSimpleRow(std::vector<PackingRectangle>& rects);
    void packSimpleColumn(std::vector<PackingRectangle>& rects);

    // Aspect ratio optimization
    void packAspectRatio(std::vector<PackingRectangle>& rects);
    double estimateWidth(const std::vector<PackingRectangle>& rects, double targetRatio);

    // Phase 3: Apply positions back to graph
    void applyLayout(const std::vector<PackingRectangle>& rects);

    // Phase 4: Optional compaction
    void compactLayout(std::vector<PackingRectangle>& rects);

    // Utilities
    Rect calculateBounds(const std::vector<PackingRectangle>& rects);
    double calculateWaste(const std::vector<PackingRectangle>& rects);
    bool rectanglesOverlap(const Rect& a, const Rect& b);
    bool canFit(const PackingRectangle& rect, const FreeRectangle& freeRect);

    RectPackingConfig config_;
};

// ============================================================================
// Utility Functions
// ============================================================================

// Check if rectangle A contains rectangle B
inline bool contains(const Rect& a, const Rect& b) {
    return b.x >= a.x && b.y >= a.y &&
           b.x + b.width <= a.x + a.width &&
           b.y + b.height <= a.y + a.height;
}

// Check if two rectangles overlap
inline bool overlaps(const Rect& a, const Rect& b) {
    return !(a.x + a.width <= b.x || b.x + b.width <= a.x ||
             a.y + a.height <= b.y || b.y + b.height <= a.y);
}

// Calculate intersection area
inline double intersectionArea(const Rect& a, const Rect& b) {
    double xOverlap = std::max(0.0, std::min(a.x + a.width, b.x + b.width) -
                                    std::max(a.x, b.x));
    double yOverlap = std::max(0.0, std::min(a.y + a.height, b.y + b.height) -
                                    std::max(a.y, b.y));
    return xOverlap * yOverlap;
}

// Sort comparison functions
inline bool compareByArea(const PackingRectangle& a, const PackingRectangle& b) {
    return a.area > b.area;  // Largest first
}

inline bool compareByWidth(const PackingRectangle& a, const PackingRectangle& b) {
    return a.bounds.width > b.bounds.width;
}

inline bool compareByHeight(const PackingRectangle& a, const PackingRectangle& b) {
    return a.bounds.height > b.bounds.height;
}

inline bool compareByPerimeter(const PackingRectangle& a, const PackingRectangle& b) {
    double perimA = 2 * (a.bounds.width + a.bounds.height);
    double perimB = 2 * (b.bounds.width + b.bounds.height);
    return perimA > perimB;
}

// Calculate aspect ratio (width/height)
inline double aspectRatio(const Rect& rect) {
    if (rect.height == 0) return 0.0;
    return rect.width / rect.height;
}

// Calculate aspect ratio difference from target
inline double aspectRatioDiff(double current, double target) {
    return std::abs(current - target);
}

} // namespace rectpacking
} // namespace elk
