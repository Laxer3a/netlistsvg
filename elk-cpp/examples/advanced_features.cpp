// ELK C++ - Advanced features demonstration
// SPDX-License-Identifier: EPL-2.0

#include <elk/graph/graph.h>
#include <elk/alg/force/force_layout.h>
#include <elk/alg/layered/layered_layout.h>
#include <elk/alg/common/edge_routing.h>
#include <elk/alg/common/port_constraints.h>
#include <elk/alg/common/label_placement.h>
#include <elk/alg/common/compaction.h>
#include <iostream>
#include <memory>

using namespace elk;

void demonstrateEdgeRouting() {
    std::cout << "=== Advanced Edge Routing ===\n";

    auto root = std::make_unique<Node>("root");

    // Create nodes
    auto n1 = root->addChild("A");
    n1->size = Size(80, 50);
    n1->position = Point(50, 50);

    auto n2 = root->addChild("B");
    n2->size = Size(80, 50);
    n2->position = Point(200, 150);

    // Create edge
    auto edge = connectNodes(n1, n2, root.get());
    edge->sections.push_back(EdgeSection());
    edge->sections[0].startPoint = Point(130, 75);
    edge->sections[0].endPoint = Point(200, 175);

    // Orthogonal routing
    std::cout << "  Orthogonal routing...\n";
    routing::OrthogonalEdgeRouter::RoutingContext ctx;
    ctx.graph = root.get();
    ctx.edgeSpacing = 5.0;
    routing::OrthogonalEdgeRouter::routeEdge(edge, ctx);

    std::cout << "    Bend points: " << edge->sections[0].bendPoints.size() << "\n";

    // Spline routing
    std::cout << "  Spline routing...\n";
    routing::SplineEdgeRouter::SplineConfig cfg;
    cfg.subdivisions = 10;
    cfg.tension = 0.5;
    routing::SplineEdgeRouter::routeEdge(edge, cfg);

    std::cout << "    Smooth points: " << edge->sections[0].bendPoints.size() << "\n";
}

void demonstratePortConstraints() {
    std::cout << "\n=== Port Constraints and Ordering ===\n";

    auto root = std::make_unique<Node>("root");

    auto node = root->addChild("Component");
    node->size = Size(100, 100);
    node->position = Point(100, 100);

    // Add ports
    auto p1 = node->addPort("in1", PortSide::WEST);
    p1->size = Size(8, 8);

    auto p2 = node->addPort("in2", PortSide::WEST);
    p2->size = Size(8, 8);

    auto p3 = node->addPort("out", PortSide::EAST);
    p3->size = Size(8, 8);

    // Set up port constraints
    ports::PortConstraintManager constraints;
    constraints.setPortConstraint(p1, ports::PortConstraint::FIXED_ORDER);
    constraints.setPortConstraint(p2, ports::PortConstraint::FIXED_ORDER);

    // Add order constraint
    ports::PortOrderConstraint orderConstraint(PortSide::WEST, {p1, p2}, true);
    constraints.addOrderConstraint(orderConstraint);

    // Place ports
    ports::PortPlacer::PlacementContext ctx;
    ctx.node = node.get();
    ctx.constraints = &constraints;
    ctx.strategy = ports::PortPlacementStrategy::SIMPLE;
    ctx.distribution.spacing = 15.0;

    ports::PortPlacer::placePorts(ctx);

    std::cout << "  Port 'in1' position: (" << p1->position.x << ", " << p1->position.y << ")\n";
    std::cout << "  Port 'in2' position: (" << p2->position.x << ", " << p2->position.y << ")\n";
    std::cout << "  Port 'out' position: (" << p3->position.x << ", " << p3->position.y << ")\n";
}

void demonstrateLabelPlacement() {
    std::cout << "\n=== Label Placement Optimization ===\n";

    auto root = std::make_unique<Node>("root");

    // Create nodes with labels
    auto n1 = root->addChild("Node1");
    n1->size = Size(80, 50);
    n1->position = Point(50, 50);
    n1->labels.push_back(Label("Input"));

    auto n2 = root->addChild("Node2");
    n2->size = Size(80, 50);
    n2->position = Point(200, 50);
    n2->labels.push_back(Label("Process"));

    // Estimate label sizes
    labels::LabelSizeEstimator::FontMetrics font;
    for (auto& child : root->children) {
        for (auto& label : child->labels) {
            label.size = labels::LabelSizeEstimator::estimateSize(label.text, font);
        }
    }

    // Configure label placement
    labels::LabelPlacementConfig config;
    config.strategy = labels::LabelPlacementStrategy::GREEDY;
    config.nodePlacement = labels::NodeLabelPlacement::OUTSIDE_TOP;
    config.avoidOverlaps = true;

    // Place labels
    labels::LabelPlacer::placeAllLabels(root.get(), config);

    for (const auto& child : root->children) {
        for (const auto& label : child->labels) {
            std::cout << "  Label '" << label.text << "' at ("
                      << label.position.x << ", " << label.position.y << ")\n";
        }
    }
}

void demonstrateCompaction() {
    std::cout << "\n=== Graph Compaction ===\n";

    auto root = std::make_unique<Node>("root");

    // Create scattered nodes
    root->addChild("N1")->position = Point(10, 10);
    root->children[0]->size = Size(50, 40);

    root->addChild("N2")->position = Point(100, 10);
    root->children[1]->size = Size(50, 40);

    root->addChild("N3")->position = Point(10, 80);
    root->children[2]->size = Size(50, 40);

    root->addChild("N4")->position = Point(100, 80);
    root->children[3]->size = Size(50, 40);

    // Calculate initial bounding box
    std::vector<Node*> nodes;
    for (auto& child : root->children) {
        nodes.push_back(child.get());
    }

    Rect beforeBounds = compaction::calculateBoundingBox(nodes);
    std::cout << "  Before compaction: " << beforeBounds.width << " x "
              << beforeBounds.height << "\n";

    // Apply compaction
    compaction::CompactionConfig config;
    config.strategy = compaction::CompactionStrategy::SCANLINE;
    config.direction = compaction::CompactionDirection::BOTH;
    config.minNodeSpacing = 10.0;

    compaction::GraphCompactor::compact(root.get(), config);

    Rect afterBounds = compaction::calculateBoundingBox(nodes);
    std::cout << "  After compaction: " << afterBounds.width << " x "
              << afterBounds.height << "\n";

    double ratio = compaction::calculateCompactionRatio(beforeBounds, afterBounds);
    std::cout << "  Compaction ratio: " << (ratio * 100) << "%\n";
}

void demonstrateCompleteWorkflow() {
    std::cout << "\n=== Complete Workflow Example ===\n";

    auto root = std::make_unique<Node>("root");

    // Create hierarchical graph with ports
    auto comp1 = root->addChild("Component1");
    comp1->size = Size(100, 60);

    auto port1 = comp1->addPort("out");
    port1->size = Size(8, 8);
    port1->labels.push_back(Label("output"));

    auto comp2 = root->addChild("Component2");
    comp2->size = Size(100, 60);

    auto port2 = comp2->addPort("in");
    port2->size = Size(8, 8);
    port2->labels.push_back(Label("input"));

    // Connect ports
    auto edge = connectPorts(port1, port2, root.get());
    edge->labels.push_back(Label("data"));

    // 1. Layout
    std::cout << "  1. Running layered layout...\n";
    layered::LayeredLayoutProvider layout;
    layout.layout(root.get());

    // 2. Assign port sides
    std::cout << "  2. Assigning port sides...\n";
    ports::PortSideAssigner::assignBasedOnEdgeDirection(comp1);
    ports::PortSideAssigner::assignBasedOnEdgeDirection(comp2);

    // 3. Place ports
    std::cout << "  3. Placing ports...\n";
    ports::PortPlacer::PlacementContext portCtx;
    portCtx.node = comp1;
    portCtx.strategy = ports::PortPlacementStrategy::BARYCENTER;
    ports::PortPlacer::placePorts(portCtx);

    portCtx.node = comp2;
    ports::PortPlacer::placePorts(portCtx);

    // 4. Route edges
    std::cout << "  4. Routing edges (orthogonal)...\n";
    routing::OrthogonalEdgeRouter::RoutingContext routeCtx;
    routeCtx.graph = root.get();
    routing::OrthogonalEdgeRouter::routeAllEdges(root.get(), routeCtx);

    // 5. Place labels
    std::cout << "  5. Placing labels...\n";
    labels::LabelPlacementConfig labelCfg;
    labelCfg.strategy = labels::LabelPlacementStrategy::GREEDY;

    // Estimate label sizes
    labels::LabelSizeEstimator::FontMetrics font;
    for (auto& child : root->children) {
        for (auto& port : child->ports) {
            for (auto& label : port->labels) {
                label.size = labels::LabelSizeEstimator::estimateSize(label.text, font);
            }
        }
    }
    for (auto& e : root->edges) {
        for (auto& label : e->labels) {
            label.size = labels::LabelSizeEstimator::estimateSize(label.text, font);
        }
    }

    labels::LabelPlacer::placeAllLabels(root.get(), labelCfg);

    // 6. Compact
    std::cout << "  6. Compacting graph...\n";
    compaction::CompactionConfig compactCfg;
    compactCfg.strategy = compaction::CompactionStrategy::SIMPLE;
    compaction::GraphCompactor::compact(root.get(), compactCfg);

    std::cout << "  Complete!\n";
    std::cout << "  Final positions:\n";
    for (const auto& child : root->children) {
        std::cout << "    " << child->id << ": ("
                  << child->position.x << ", " << child->position.y << ")\n";
    }
}

int main() {
    demonstrateEdgeRouting();
    demonstratePortConstraints();
    demonstrateLabelPlacement();
    demonstrateCompaction();
    demonstrateCompleteWorkflow();

    std::cout << "\nAll advanced features demonstrated!\n";
    return 0;
}
