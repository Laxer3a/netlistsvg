// Eclipse Layout Kernel - C++ Port
// RectPacking implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/alg/rectpacking/rectpacking_layout.h"
#include <algorithm>
#include <limits>
#include <cmath>

namespace elk {
namespace rectpacking {

RectPackingLayoutProvider::RectPackingLayoutProvider() {}

void RectPackingLayoutProvider::layout(Node* graph, ProgressCallback progress) {
    if (!graph) return;

    if (progress) progress("Extracting rectangles", 0.1);
    auto rects = extractRectangles(graph);

    if (rects.empty()) return;

    if (progress) progress("Sorting rectangles", 0.2);
    sortRectangles(rects);

    if (progress) progress("Packing rectangles", 0.4);
    packRectangles(rects);

    if (config_.compactResult && progress) progress("Compacting layout", 0.8);
    if (config_.compactResult) {
        compactLayout(rects);
    }

    if (progress) progress("Applying layout", 0.9);
    applyLayout(rects);

    if (progress) progress("Complete", 1.0);
}

std::vector<PackingRectangle> RectPackingLayoutProvider::extractRectangles(Node* graph) {
    std::vector<PackingRectangle> rects;
    int id = 0;

    for (auto& child : graph->children) {
        PackingRectangle rect;
        rect.node = child.get();
        rect.bounds.x = 0;
        rect.bounds.y = 0;
        rect.bounds.width = child->size.width;
        rect.bounds.height = child->size.height;
        rect.area = rect.bounds.width * rect.bounds.height;
        rect.id = id++;
        rect.placed = false;

        rects.push_back(rect);
    }

    return rects;
}

void RectPackingLayoutProvider::sortRectangles(std::vector<PackingRectangle>& rects) {
    if (config_.sortByArea) {
        std::sort(rects.begin(), rects.end(), compareByArea);
    } else if (config_.sortByWidth) {
        std::sort(rects.begin(), rects.end(), compareByWidth);
    } else if (config_.sortByHeight) {
        std::sort(rects.begin(), rects.end(), compareByHeight);
    } else {
        std::sort(rects.begin(), rects.end(), compareByPerimeter);
    }
}

void RectPackingLayoutProvider::packRectangles(std::vector<PackingRectangle>& rects) {
    switch (config_.algorithm) {
        case PackingAlgorithm::SHELF_NEXT_FIT:
            packShelfNextFit(rects);
            break;
        case PackingAlgorithm::SHELF_FIRST_FIT:
            packShelfFirstFit(rects);
            break;
        case PackingAlgorithm::SHELF_BEST_FIT:
            packShelfBestFit(rects);
            break;
        case PackingAlgorithm::GUILLOTINE:
            packGuillotine(rects);
            break;
        case PackingAlgorithm::MAXRECTS:
            packMaxRects(rects);
            break;
    }

    // Handle strategy-based simple packing
    if (config_.strategy == PackingStrategy::SIMPLE_ROW) {
        packSimpleRow(rects);
    } else if (config_.strategy == PackingStrategy::SIMPLE_COLUMN) {
        packSimpleColumn(rects);
    } else if (config_.strategy == PackingStrategy::ASPECT_RATIO) {
        packAspectRatio(rects);
    }
}

void RectPackingLayoutProvider::packShelfNextFit(std::vector<PackingRectangle>& rects) {
    std::vector<Shelf> shelves;
    Shelf currentShelf;
    currentShelf.y = 0;
    currentShelf.height = 0;
    currentShelf.usedWidth = 0;

    for (auto& rect : rects) {
        double rectWidth = rect.bounds.width + config_.nodeSpacing;
        double rectHeight = rect.bounds.height + config_.nodeSpacing;

        // If doesn't fit on current shelf, create new shelf
        if (currentShelf.items.size() > 0 && rectHeight > currentShelf.height) {
            shelves.push_back(currentShelf);
            currentShelf.y += currentShelf.height;
            currentShelf.height = 0;
            currentShelf.usedWidth = 0;
            currentShelf.items.clear();
        }

        // Place on current shelf
        rect.bounds.x = currentShelf.usedWidth;
        rect.bounds.y = currentShelf.y;
        rect.placed = true;

        currentShelf.usedWidth += rectWidth;
        currentShelf.height = std::max(currentShelf.height, rectHeight);
        currentShelf.items.push_back(&rect);
    }

    if (!currentShelf.items.empty()) {
        shelves.push_back(currentShelf);
    }
}

void RectPackingLayoutProvider::packShelfFirstFit(std::vector<PackingRectangle>& rects) {
    std::vector<Shelf> shelves;

    for (auto& rect : rects) {
        double rectWidth = rect.bounds.width + config_.nodeSpacing;
        double rectHeight = rect.bounds.height + config_.nodeSpacing;

        // Find first shelf that fits
        bool placed = false;
        for (auto& shelf : shelves) {
            if (rectHeight <= shelf.height) {
                rect.bounds.x = shelf.usedWidth;
                rect.bounds.y = shelf.y;
                rect.placed = true;
                placed = true;

                shelf.usedWidth += rectWidth;
                shelf.items.push_back(&rect);
                break;
            }
        }

        // Create new shelf if not placed
        if (!placed) {
            Shelf newShelf;
            newShelf.y = shelves.empty() ? 0 : shelves.back().y + shelves.back().height;
            newShelf.height = rectHeight;
            newShelf.usedWidth = rectWidth;

            rect.bounds.x = 0;
            rect.bounds.y = newShelf.y;
            rect.placed = true;

            newShelf.items.push_back(&rect);
            shelves.push_back(newShelf);
        }
    }
}

void RectPackingLayoutProvider::packShelfBestFit(std::vector<PackingRectangle>& rects) {
    std::vector<Shelf> shelves;

    for (auto& rect : rects) {
        double rectWidth = rect.bounds.width + config_.nodeSpacing;
        double rectHeight = rect.bounds.height + config_.nodeSpacing;

        // Find best fitting shelf (least waste)
        Shelf* bestShelf = nullptr;
        double bestWaste = std::numeric_limits<double>::max();

        for (auto& shelf : shelves) {
            if (rectHeight <= shelf.height) {
                double waste = shelf.height - rectHeight;
                if (waste < bestWaste) {
                    bestWaste = waste;
                    bestShelf = &shelf;
                }
            }
        }

        if (bestShelf) {
            rect.bounds.x = bestShelf->usedWidth;
            rect.bounds.y = bestShelf->y;
            rect.placed = true;

            bestShelf->usedWidth += rectWidth;
            bestShelf->items.push_back(&rect);
        } else {
            // Create new shelf
            Shelf newShelf;
            newShelf.y = shelves.empty() ? 0 : shelves.back().y + shelves.back().height;
            newShelf.height = rectHeight;
            newShelf.usedWidth = rectWidth;

            rect.bounds.x = 0;
            rect.bounds.y = newShelf.y;
            rect.placed = true;

            newShelf.items.push_back(&rect);
            shelves.push_back(newShelf);
        }
    }
}

void RectPackingLayoutProvider::packGuillotine(std::vector<PackingRectangle>& rects) {
    // Start with one large free rectangle
    double totalArea = 0;
    for (const auto& rect : rects) {
        totalArea += rect.area;
    }

    // Estimate initial bounds
    double initialSize = std::sqrt(totalArea) * 1.5;
    std::vector<FreeRectangle> freeRects;
    freeRects.push_back(FreeRectangle(0, 0, initialSize, initialSize));

    for (auto& rect : rects) {
        // Find best free rectangle
        int bestIdx = -1;
        double bestScore = std::numeric_limits<double>::max();

        for (size_t i = 0; i < freeRects.size(); ++i) {
            if (canFit(rect, freeRects[i])) {
                double waste = freeRects[i].rect.width * freeRects[i].rect.height - rect.area;
                if (waste < bestScore) {
                    bestScore = waste;
                    bestIdx = i;
                }
            }
        }

        if (bestIdx >= 0) {
            // Place rectangle
            FreeRectangle& freeRect = freeRects[bestIdx];
            rect.bounds.x = freeRect.rect.x;
            rect.bounds.y = freeRect.rect.y;
            rect.placed = true;

            Rect usedRect = rect.bounds;
            usedRect.width += config_.nodeSpacing;
            usedRect.height += config_.nodeSpacing;

            // Split the free rectangle
            guillotineSplit(freeRects, usedRect, freeRect.rect);
            freeRects.erase(freeRects.begin() + bestIdx);
        }
    }
}

void RectPackingLayoutProvider::guillotineSplit(std::vector<FreeRectangle>& freeRects,
                                                const Rect& usedRect, const Rect& freeRect) {
    // Horizontal split
    if (usedRect.x + usedRect.width < freeRect.x + freeRect.width) {
        FreeRectangle rightRect(
            usedRect.x + usedRect.width,
            freeRect.y,
            freeRect.x + freeRect.width - (usedRect.x + usedRect.width),
            freeRect.height
        );
        freeRects.push_back(rightRect);
    }

    // Vertical split
    if (usedRect.y + usedRect.height < freeRect.y + freeRect.height) {
        FreeRectangle bottomRect(
            freeRect.x,
            usedRect.y + usedRect.height,
            freeRect.width,
            freeRect.y + freeRect.height - (usedRect.y + usedRect.height)
        );
        freeRects.push_back(bottomRect);
    }
}

void RectPackingLayoutProvider::packMaxRects(std::vector<PackingRectangle>& rects) {
    // Estimate initial bounds
    double totalArea = 0;
    for (const auto& rect : rects) {
        totalArea += rect.area;
    }

    double initialSize = std::sqrt(totalArea / config_.targetAspectRatio) * 1.5;
    double initialWidth = initialSize * config_.targetAspectRatio;
    double initialHeight = initialSize;

    std::vector<FreeRectangle> freeRects;
    freeRects.push_back(FreeRectangle(0, 0, initialWidth, initialHeight));

    for (auto& rect : rects) {
        // Find best position using scoring
        int bestIdx = -1;
        int bestScore = std::numeric_limits<int>::min();

        for (size_t i = 0; i < freeRects.size(); ++i) {
            if (canFit(rect, freeRects[i])) {
                int score = scoreRectangle(rect, freeRects[i]);
                if (score > bestScore) {
                    bestScore = score;
                    bestIdx = i;
                }
            }
        }

        if (bestIdx >= 0) {
            // Place rectangle
            FreeRectangle& freeRect = freeRects[bestIdx];
            rect.bounds.x = freeRect.rect.x;
            rect.bounds.y = freeRect.rect.y;
            rect.placed = true;

            Rect usedRect = rect.bounds;
            usedRect.width += config_.nodeSpacing;
            usedRect.height += config_.nodeSpacing;

            // Split all intersecting free rectangles
            std::vector<FreeRectangle> newFreeRects;
            for (size_t i = 0; i < freeRects.size(); ++i) {
                if (overlaps(usedRect, freeRects[i].rect)) {
                    splitFreeRectangle(newFreeRects, usedRect, freeRects[i]);
                } else {
                    newFreeRects.push_back(freeRects[i]);
                }
            }

            freeRects = newFreeRects;
            pruneFreeRectangles(freeRects);
        }
    }
}

int RectPackingLayoutProvider::scoreRectangle(const PackingRectangle& rect,
                                              const FreeRectangle& freeRect) {
    // Score based on Best Short Side Fit (BSSF)
    double leftoverHoriz = freeRect.rect.width - rect.bounds.width;
    double leftoverVert = freeRect.rect.height - rect.bounds.height;
    double shortSideFit = std::min(leftoverHoriz, leftoverVert);
    double longSideFit = std::max(leftoverHoriz, leftoverVert);

    return -static_cast<int>(shortSideFit * 1000 + longSideFit);
}

void RectPackingLayoutProvider::splitFreeRectangle(std::vector<FreeRectangle>& freeRects,
                                                   const Rect& usedRect,
                                                   const FreeRectangle& freeRect) {
    // Create new free rectangles from split
    if (usedRect.x > freeRect.rect.x) {
        // Left side
        FreeRectangle left(
            freeRect.rect.x,
            freeRect.rect.y,
            usedRect.x - freeRect.rect.x,
            freeRect.rect.height
        );
        freeRects.push_back(left);
    }

    if (usedRect.x + usedRect.width < freeRect.rect.x + freeRect.rect.width) {
        // Right side
        FreeRectangle right(
            usedRect.x + usedRect.width,
            freeRect.rect.y,
            freeRect.rect.x + freeRect.rect.width - (usedRect.x + usedRect.width),
            freeRect.rect.height
        );
        freeRects.push_back(right);
    }

    if (usedRect.y > freeRect.rect.y) {
        // Top side
        FreeRectangle top(
            freeRect.rect.x,
            freeRect.rect.y,
            freeRect.rect.width,
            usedRect.y - freeRect.rect.y
        );
        freeRects.push_back(top);
    }

    if (usedRect.y + usedRect.height < freeRect.rect.y + freeRect.rect.height) {
        // Bottom side
        FreeRectangle bottom(
            freeRect.rect.x,
            usedRect.y + usedRect.height,
            freeRect.rect.width,
            freeRect.rect.y + freeRect.rect.height - (usedRect.y + usedRect.height)
        );
        freeRects.push_back(bottom);
    }
}

void RectPackingLayoutProvider::pruneFreeRectangles(std::vector<FreeRectangle>& freeRects) {
    // Remove rectangles that are contained in other rectangles
    for (size_t i = 0; i < freeRects.size(); ++i) {
        for (size_t j = i + 1; j < freeRects.size();) {
            if (contains(freeRects[i].rect, freeRects[j].rect)) {
                freeRects.erase(freeRects.begin() + j);
            } else if (contains(freeRects[j].rect, freeRects[i].rect)) {
                freeRects.erase(freeRects.begin() + i);
                --i;
                break;
            } else {
                ++j;
            }
        }
    }
}

void RectPackingLayoutProvider::packSimpleRow(std::vector<PackingRectangle>& rects) {
    double x = 0;
    double y = 0;
    double rowHeight = 0;
    double maxWidth = estimateWidth(rects, config_.targetAspectRatio);

    for (auto& rect : rects) {
        double rectWidth = rect.bounds.width + config_.nodeSpacing;
        double rectHeight = rect.bounds.height + config_.nodeSpacing;

        // Start new row if doesn't fit
        if (x > 0 && x + rectWidth > maxWidth) {
            x = 0;
            y += rowHeight;
            rowHeight = 0;
        }

        rect.bounds.x = x;
        rect.bounds.y = y;
        rect.placed = true;

        x += rectWidth;
        rowHeight = std::max(rowHeight, rectHeight);
    }
}

void RectPackingLayoutProvider::packSimpleColumn(std::vector<PackingRectangle>& rects) {
    double x = 0;
    double y = 0;
    double colWidth = 0;
    double maxHeight = estimateWidth(rects, 1.0 / config_.targetAspectRatio);

    for (auto& rect : rects) {
        double rectWidth = rect.bounds.width + config_.nodeSpacing;
        double rectHeight = rect.bounds.height + config_.nodeSpacing;

        // Start new column if doesn't fit
        if (y > 0 && y + rectHeight > maxHeight) {
            y = 0;
            x += colWidth;
            colWidth = 0;
        }

        rect.bounds.x = x;
        rect.bounds.y = y;
        rect.placed = true;

        y += rectHeight;
        colWidth = std::max(colWidth, rectWidth);
    }
}

void RectPackingLayoutProvider::packAspectRatio(std::vector<PackingRectangle>& rects) {
    // Use row packing with aspect ratio optimization
    packSimpleRow(rects);
}

double RectPackingLayoutProvider::estimateWidth(const std::vector<PackingRectangle>& rects,
                                                double targetRatio) {
    double totalArea = 0;
    for (const auto& rect : rects) {
        totalArea += rect.area;
    }

    // width = sqrt(totalArea * targetRatio)
    return std::sqrt(totalArea * targetRatio);
}

void RectPackingLayoutProvider::applyLayout(const std::vector<PackingRectangle>& rects) {
    for (const auto& rect : rects) {
        if (rect.node && rect.placed) {
            rect.node->position.x = rect.bounds.x;
            rect.node->position.y = rect.bounds.y;
        }
    }
}

void RectPackingLayoutProvider::compactLayout(std::vector<PackingRectangle>& rects) {
    if (!config_.optimizeWhitespace) return;

    // Simple compaction: move rectangles left and up as much as possible
    bool changed = true;
    int maxIterations = 5;
    int iteration = 0;

    while (changed && iteration < maxIterations) {
        changed = false;
        ++iteration;

        // Try moving each rectangle left
        for (auto& rect : rects) {
            double minX = 0;
            bool canMove = true;

            // Check for overlap with other rectangles
            for (auto& other : rects) {
                if (&rect == &other) continue;

                Rect testRect = rect.bounds;
                testRect.x = minX;

                if (rectanglesOverlap(testRect, other.bounds)) {
                    minX = other.bounds.x + other.bounds.width + config_.nodeSpacing;
                    canMove = false;
                }
            }

            if (rect.bounds.x > minX) {
                rect.bounds.x = minX;
                changed = true;
            }
        }

        // Try moving each rectangle up
        for (auto& rect : rects) {
            double minY = 0;

            for (auto& other : rects) {
                if (&rect == &other) continue;

                Rect testRect = rect.bounds;
                testRect.y = minY;

                if (rectanglesOverlap(testRect, other.bounds)) {
                    minY = other.bounds.y + other.bounds.height + config_.nodeSpacing;
                }
            }

            if (rect.bounds.y > minY) {
                rect.bounds.y = minY;
                changed = true;
            }
        }
    }
}

Rect RectPackingLayoutProvider::calculateBounds(const std::vector<PackingRectangle>& rects) {
    if (rects.empty()) return Rect{0, 0, 0, 0};

    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double maxY = std::numeric_limits<double>::lowest();

    for (const auto& rect : rects) {
        minX = std::min(minX, rect.bounds.x);
        minY = std::min(minY, rect.bounds.y);
        maxX = std::max(maxX, rect.bounds.x + rect.bounds.width);
        maxY = std::max(maxY, rect.bounds.y + rect.bounds.height);
    }

    return Rect{minX, minY, maxX - minX, maxY - minY};
}

double RectPackingLayoutProvider::calculateWaste(const std::vector<PackingRectangle>& rects) {
    Rect bounds = calculateBounds(rects);
    double boundingArea = bounds.width * bounds.height;

    double usedArea = 0;
    for (const auto& rect : rects) {
        usedArea += rect.area;
    }

    return boundingArea - usedArea;
}

bool RectPackingLayoutProvider::rectanglesOverlap(const Rect& a, const Rect& b) {
    return overlaps(a, b);
}

bool RectPackingLayoutProvider::canFit(const PackingRectangle& rect,
                                      const FreeRectangle& freeRect) {
    return rect.bounds.width + config_.nodeSpacing <= freeRect.rect.width &&
           rect.bounds.height + config_.nodeSpacing <= freeRect.rect.height;
}

} // namespace rectpacking
} // namespace elk
