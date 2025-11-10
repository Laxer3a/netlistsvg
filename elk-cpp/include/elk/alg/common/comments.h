// Eclipse Layout Kernel - C++ Port
// Comment attachment system
// SPDX-License-Identifier: EPL-2.0

#pragma once

#include "../../core/types.h"
#include "../../graph/graph.h"
#include <vector>
#include <unordered_map>

namespace elk {
namespace comments {

// ============================================================================
// Comment Types
// ============================================================================

enum class CommentAttachment {
    NODES,        // Attached to specific nodes
    EDGES,        // Attached to edges
    FREE          // Free-floating comment
};

struct Comment {
    Node* node = nullptr;          // The comment node itself
    CommentAttachment type = CommentAttachment::FREE;
    std::vector<Node*> attachedNodes;
    std::vector<Edge*> attachedEdges;
    Rect bounds;
};

struct CommentConfig {
    double commentSpacing = 10.0;
    double minDistance = 5.0;
    bool placeOutside = true;      // Place outside attached nodes
    bool avoidOverlaps = true;
};

// ============================================================================
// Comment Processor
// ============================================================================

class CommentProcessor {
public:
    static void process(Node* graph, const CommentConfig& config);

private:
    static std::vector<Comment> identifyComments(Node* graph);
    static void attachComments(std::vector<Comment>& comments, Node* graph);
    static void placeComments(std::vector<Comment>& comments, const CommentConfig& config);

    static void placeNearAttachedNodes(Comment& comment, const CommentConfig& config);
    static void placeNearAttachedEdges(Comment& comment, const CommentConfig& config);
    static void avoidOverlaps(Comment& comment, const std::vector<Comment>& allComments,
                             const CommentConfig& config);
    static bool rectsOverlap(const Rect& a, const Rect& b);
};

} // namespace comments
} // namespace elk
