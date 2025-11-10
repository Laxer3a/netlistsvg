// Eclipse Layout Kernel - C++ Port
// Comment attachment implementation
// SPDX-License-Identifier: EPL-2.0

#include "elk/alg/common/comments.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace elk {
namespace comments {

void CommentProcessor::process(Node* graph, const CommentConfig& config) {
    if (!graph) return;

    auto comments = identifyComments(graph);
    if (comments.empty()) return;

    attachComments(comments, graph);
    placeComments(comments, config);
}

std::vector<Comment> CommentProcessor::identifyComments(Node* graph) {
    std::vector<Comment> comments;

    if (!graph) return comments;

    // Identify comment nodes by checking for special properties or naming conventions
    for (auto& child : graph->children) {
        // Check if this is a comment node
        // Heuristics: nodes with "comment" in ID, or special property
        bool isComment = false;

        // Check ID for comment pattern
        if (child->id.find("comment") != std::string::npos ||
            child->id.find("Comment") != std::string::npos ||
            child->id.find("annotation") != std::string::npos) {
            isComment = true;
        }

        // Check if node has no ports (typical of comments)
        if (child->ports.empty() && !child->labels.empty()) {
            isComment = true;
        }

        if (isComment) {
            Comment comment;
            comment.node = child.get();
            comment.type = CommentAttachment::FREE; // Default to free
            comment.bounds = Rect{
                child->position.x,
                child->position.y,
                child->size.width,
                child->size.height
            };
            comments.push_back(comment);
        }
    }

    return comments;
}

void CommentProcessor::attachComments(std::vector<Comment>& comments, Node* graph) {
    if (!graph) return;

    for (auto& comment : comments) {
        if (!comment.node) continue;

        // Find nearest nodes (potential attachment targets)
        std::vector<std::pair<Node*, double>> distances;

        for (auto& child : graph->children) {
            if (child.get() == comment.node) continue;

            // Calculate center-to-center distance
            double cx1 = comment.node->position.x + comment.node->size.width / 2;
            double cy1 = comment.node->position.y + comment.node->size.height / 2;
            double cx2 = child->position.x + child->size.width / 2;
            double cy2 = child->position.y + child->size.height / 2;

            double dist = std::sqrt(
                std::pow(cx2 - cx1, 2) + std::pow(cy2 - cy1, 2)
            );

            distances.push_back({child.get(), dist});
        }

        // Sort by distance
        std::sort(distances.begin(), distances.end(),
            [](const auto& a, const auto& b) {
                return a.second < b.second;
            });

        // Attach to nearest node(s) within threshold
        const double ATTACHMENT_THRESHOLD = 150.0; // Max distance for attachment

        for (const auto& pair : distances) {
            if (pair.second <= ATTACHMENT_THRESHOLD) {
                comment.attachedNodes.push_back(pair.first);
                comment.type = CommentAttachment::NODES;

                // Limit to 3 attached nodes
                if (comment.attachedNodes.size() >= 3) {
                    break;
                }
            }
        }
    }
}

void CommentProcessor::placeComments(std::vector<Comment>& comments,
                                    const CommentConfig& config) {
    for (auto& comment : comments) {
        if (!comment.node) continue;

        if (comment.type == CommentAttachment::NODES && !comment.attachedNodes.empty()) {
            // Place near attached nodes
            placeNearAttachedNodes(comment, config);
        } else if (comment.type == CommentAttachment::EDGES && !comment.attachedEdges.empty()) {
            // Place near attached edges
            placeNearAttachedEdges(comment, config);
        }
        // Free comments keep their original position

        // Avoid overlaps if configured
        if (config.avoidOverlaps) {
            avoidOverlaps(comment, comments, config);
        }
    }
}

void CommentProcessor::placeNearAttachedNodes(Comment& comment,
                                              const CommentConfig& config) {
    if (comment.attachedNodes.empty()) return;

    // Calculate centroid of attached nodes
    double sumX = 0, sumY = 0;
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    for (Node* node : comment.attachedNodes) {
        double cx = node->position.x + node->size.width / 2;
        double cy = node->position.y + node->size.height / 2;
        sumX += cx;
        sumY += cy;

        minX = std::min(minX, node->position.x);
        maxX = std::max(maxX, node->position.x + node->size.width);
        minY = std::min(minY, node->position.y);
        maxY = std::max(maxY, node->position.y + node->size.height);
    }

    double centroidX = sumX / comment.attachedNodes.size();
    double centroidY = sumY / comment.attachedNodes.size();

    // Place comment based on configuration
    if (config.placeOutside) {
        // Place outside the bounding box of attached nodes
        double width = maxX - minX;
        double height = maxY - minY;

        // Determine best side (prefer top or right)
        bool placeTop = true;
        bool placeRight = width > height;

        if (placeTop && !placeRight) {
            // Top
            comment.node->position.x = centroidX - comment.node->size.width / 2;
            comment.node->position.y = minY - comment.node->size.height - config.commentSpacing;
        } else if (!placeTop && placeRight) {
            // Right
            comment.node->position.x = maxX + config.commentSpacing;
            comment.node->position.y = centroidY - comment.node->size.height / 2;
        } else if (placeRight) {
            // Right (prefer this)
            comment.node->position.x = maxX + config.commentSpacing;
            comment.node->position.y = centroidY - comment.node->size.height / 2;
        } else {
            // Top (fallback)
            comment.node->position.x = centroidX - comment.node->size.width / 2;
            comment.node->position.y = minY - comment.node->size.height - config.commentSpacing;
        }
    } else {
        // Place at centroid
        comment.node->position.x = centroidX - comment.node->size.width / 2;
        comment.node->position.y = centroidY - comment.node->size.height / 2;
    }

    // Update bounds
    comment.bounds = Rect{
        comment.node->position.x,
        comment.node->position.y,
        comment.node->size.width,
        comment.node->size.height
    };
}

void CommentProcessor::placeNearAttachedEdges(Comment& comment,
                                              const CommentConfig& config) {
    // Place near midpoint of attached edges
    if (comment.attachedEdges.empty()) return;

    // Simple implementation: place at midpoint of first edge
    Edge* edge = comment.attachedEdges[0];

    if (!edge->sections.empty() && !edge->sections[0].bendPoints.empty()) {
        const auto& points = edge->sections[0].bendPoints;
        size_t midIdx = points.size() / 2;
        const Point& midPoint = points[midIdx];

        comment.node->position.x = midPoint.x - comment.node->size.width / 2;
        comment.node->position.y = midPoint.y - comment.node->size.height / 2;
    }

    comment.bounds = Rect{
        comment.node->position.x,
        comment.node->position.y,
        comment.node->size.width,
        comment.node->size.height
    };
}

void CommentProcessor::avoidOverlaps(Comment& comment,
                                     const std::vector<Comment>& allComments,
                                     const CommentConfig& config) {
    const int MAX_ITERATIONS = 10;
    const double MOVE_STEP = 10.0;

    for (int iter = 0; iter < MAX_ITERATIONS; ++iter) {
        bool hasOverlap = false;

        for (const auto& other : allComments) {
            if (&other == &comment) continue;
            if (!other.node) continue;

            // Check for overlap
            if (rectsOverlap(comment.bounds, other.bounds)) {
                hasOverlap = true;

                // Move comment away from overlap
                double dx = comment.bounds.x + comment.bounds.width / 2 -
                           (other.bounds.x + other.bounds.width / 2);
                double dy = comment.bounds.y + comment.bounds.height / 2 -
                           (other.bounds.y + other.bounds.height / 2);

                double dist = std::sqrt(dx * dx + dy * dy);
                if (dist > 0) {
                    dx /= dist;
                    dy /= dist;

                    comment.node->position.x += dx * MOVE_STEP;
                    comment.node->position.y += dy * MOVE_STEP;

                    comment.bounds.x = comment.node->position.x;
                    comment.bounds.y = comment.node->position.y;
                }
            }
        }

        if (!hasOverlap) break;
    }
}

bool CommentProcessor::rectsOverlap(const Rect& a, const Rect& b) {
    return !(a.x + a.width <= b.x || b.x + b.width <= a.x ||
             a.y + a.height <= b.y || b.y + b.height <= a.y);
}

} // namespace comments
} // namespace elk
