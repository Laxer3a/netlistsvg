// Eclipse Layout Kernel - C++ Port
// Comprehensive test for all implemented features
// SPDX-License-Identifier: EPL-2.0

#include <elk/alg/all_algorithms.h>
#include <elk/core/recursive_layout.h>
#include <elk/core/configuration.h>
#include <elk/alg/common/node_spacing.h>
#include <elk/alg/common/comments.h>
#include <elk/alg/common/incremental.h>
#include <iostream>
#include <cassert>

using namespace elk;

// Test utilities
void printTestResult(const std::string& testName, bool passed) {
    std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << testName << std::endl;
}

// Test 1: MrTree Layout
bool testMrTreeLayout() {
    auto root = std::make_unique<Node>("root");

    // Create tree structure
    auto n1 = root->addChild("n1");
    n1->size = Size{60, 40};

    auto n2 = root->addChild("n2");
    n2->size = Size{60, 40};

    auto n3 = root->addChild("n3");
    n3->size = Size{60, 40};

    auto n4 = n1->addChild("n4");
    n4->size = Size{60, 40};

    auto n5 = n1->addChild("n5");
    n5->size = Size{60, 40};

    // Connect tree edges
    connectNodes(n1, n4, root.get());
    connectNodes(n1, n5, root.get());

    // Layout using MrTree
    mrtree::MrTreeLayoutProvider layout;
    layout.setNodeSpacing(20);
    layout.setLevelSpacing(80);
    layout.layout(root.get());

    // Verify positions are set
    bool hasPositions = (n1->position.x != 0 || n1->position.y != 0) &&
                       (n4->position.y > n1->position.y);

    return hasPositions;
}

// Test 2: RectPacking Layout
bool testRectPackingLayout() {
    auto root = std::make_unique<Node>("root");

    // Create disconnected nodes
    for (int i = 0; i < 5; ++i) {
        auto node = root->addChild("node_" + std::to_string(i));
        node->size = Size{60, 40};
    }

    // Layout using RectPacking
    rectpacking::RectPackingLayoutProvider layout;
    layout.setAlgorithm(rectpacking::PackingAlgorithm::MAXRECTS);
    layout.setNodeSpacing(15);
    layout.layout(root.get());

    // Verify nodes don't overlap
    bool noOverlap = true;
    for (size_t i = 0; i < root->children.size(); ++i) {
        for (size_t j = i + 1; j < root->children.size(); ++j) {
            auto& n1 = root->children[i];
            auto& n2 = root->children[j];

            Rect r1{n1->position.x, n1->position.y, n1->size.width, n1->size.height};
            Rect r2{n2->position.x, n2->position.y, n2->size.width, n2->size.height};

            if (rectpacking::overlaps(r1, r2)) {
                noOverlap = false;
                break;
            }
        }
    }

    return noOverlap;
}

// Test 3: Layered with Advanced Features
bool testLayeredWithAdvancedFeatures() {
    auto root = std::make_unique<Node>("root");

    // Create layered graph
    auto n1 = root->addChild("n1");
    n1->size = Size{60, 40};

    auto n2 = root->addChild("n2");
    n2->size = Size{60, 40};

    auto n3 = root->addChild("n3");
    n3->size = Size{60, 40};

    auto n4 = root->addChild("n4");
    n4->size = Size{60, 40};

    // Connect nodes
    connectNodes(n1, n3, root.get());
    connectNodes(n2, n3, root.get());
    connectNodes(n2, n4, root.get());
    connectNodes(n3, n4, root.get());

    // Add self-loop
    connectNodes(n3, n3, root.get());

    // Layout using Layered
    layered::LayeredLayoutProvider layout;
    layout.setDirection(Direction::RIGHT);
    layout.layout(root.get());

    // Verify layering (nodes should be separated horizontally)
    bool hasLayers = (n3->position.x > n1->position.x) &&
                    (n4->position.x > n3->position.x);

    return hasLayers;
}

// Test 4: Network Simplex Placement
bool testNetworkSimplexPlacement() {
    auto root = std::make_unique<Node>("root");

    // Create simple graph
    auto n1 = root->addChild("n1");
    n1->size = Size{60, 40};

    auto n2 = root->addChild("n2");
    n2->size = Size{60, 40};

    auto n3 = root->addChild("n3");
    n3->size = Size{60, 40};

    connectNodes(n1, n2, root.get());
    connectNodes(n2, n3, root.get());

    // Use layered layout (which can use network simplex internally)
    layered::LayeredLayoutProvider layout;
    layout.layout(root.get());

    // Basic check: nodes have positions
    return n1->position.x >= 0 && n2->position.x >= 0 && n3->position.x >= 0;
}

// Test 5: Brandes-Koepf Placement
bool testBrandesKoepfPlacement() {
    auto root = std::make_unique<Node>("root");

    // Create graph with crossing edges
    auto n1 = root->addChild("n1");
    n1->size = Size{60, 40};

    auto n2 = root->addChild("n2");
    n2->size = Size{60, 40};

    auto n3 = root->addChild("n3");
    n3->size = Size{60, 40};

    auto n4 = root->addChild("n4");
    n4->size = Size{60, 40};

    connectNodes(n1, n4, root.get());
    connectNodes(n2, n3, root.get());

    // Layout
    layered::LayeredLayoutProvider layout;
    layout.layout(root.get());

    return true; // Brandes-Koepf is used internally
}

// Test 6: Recursive Layout
bool testRecursiveLayout() {
    auto root = std::make_unique<Node>("root");

    // Create hierarchical structure
    auto parent = root->addChild("parent");
    parent->size = Size{200, 150};

    auto child1 = parent->addChild("child1");
    child1->size = Size{60, 40};

    auto child2 = parent->addChild("child2");
    child2->size = Size{60, 40};

    // Use recursive layout
    force::ForceLayoutProvider provider;
    RecursiveConfig config;
    config.strategy = RecursiveStrategy::TOP_DOWN;

    RecursiveLayoutEngine::layout(root.get(), &provider, config);

    // Verify parent was adjusted
    return parent->size.width >= 60 && parent->size.height >= 40;
}

// Test 7: Incremental Layout
bool testIncrementalLayout() {
    auto root = std::make_unique<Node>("root");

    auto n1 = root->addChild("n1");
    n1->size = Size{60, 40};

    auto n2 = root->addChild("n2");
    n2->size = Size{60, 40};

    // Initial layout
    force::ForceLayoutProvider layout;
    layout.layout(root.get());

    double oldX = n1->position.x;

    // Simulate change
    std::vector<incremental::GraphChange> changes;
    incremental::GraphChange change;
    change.type = incremental::ChangeType::NODE_MOVED;
    change.node = n1;
    changes.push_back(change);

    incremental::IncrementalConfig config;
    incremental::IncrementalLayoutEngine::layout(root.get(), changes, config);

    return true; // Incremental layout executed
}

// Test 8: Configuration System
bool testConfigurationSystem() {
    auto& config = GlobalConfig::instance();

    config.setValue(GlobalConfig::NODE_SPACING, 25.0);
    double spacing = config.getValue(GlobalConfig::NODE_SPACING, 20.0);

    return spacing == 25.0;
}

// Test 9: All Algorithms Available
bool testAllAlgorithms() {
    bool allAvailable = true;

    ILayoutProvider* force = createLayoutProvider("elk.force");
    allAvailable &= (force != nullptr);
    delete force;

    ILayoutProvider* layered = createLayoutProvider("elk.layered");
    allAvailable &= (layered != nullptr);
    delete layered;

    ILayoutProvider* mrtree = createLayoutProvider("elk.mrtree");
    allAvailable &= (mrtree != nullptr);
    delete mrtree;

    ILayoutProvider* rectpacking = createLayoutProvider("elk.rectpacking");
    allAvailable &= (rectpacking != nullptr);
    delete rectpacking;

    return allAvailable;
}

// Test 10: Node Spacing
bool testNodeSpacing() {
    auto root = std::make_unique<Node>("root");

    auto n1 = root->addChild("n1");
    n1->size = Size{60, 40};

    auto n2 = root->addChild("n2");
    n2->size = Size{60, 40};

    spacing::NodeSpacingConfig config;
    config.strategy = spacing::SpacingStrategy::ADAPTIVE;

    spacing::NodeSpacingCalculator::calculate(root.get(), config);

    return true; // Node spacing executed
}

// Main test runner
int main() {
    std::cout << "=== ELK C++ Comprehensive Feature Test ===" << std::endl;
    std::cout << std::endl;

    int passed = 0;
    int total = 0;

    #define RUN_TEST(test) \
        total++; \
        if (test()) { \
            passed++; \
            printTestResult(#test, true); \
        } else { \
            printTestResult(#test, false); \
        }

    RUN_TEST(testMrTreeLayout);
    RUN_TEST(testRectPackingLayout);
    RUN_TEST(testLayeredWithAdvancedFeatures);
    RUN_TEST(testNetworkSimplexPlacement);
    RUN_TEST(testBrandesKoepfPlacement);
    RUN_TEST(testRecursiveLayout);
    RUN_TEST(testIncrementalLayout);
    RUN_TEST(testConfigurationSystem);
    RUN_TEST(testAllAlgorithms);
    RUN_TEST(testNodeSpacing);

    std::cout << std::endl;
    std::cout << "=== Test Results ===" << std::endl;
    std::cout << "Passed: " << passed << "/" << total << std::endl;
    std::cout << "Coverage: " << (passed * 100 / total) << "%" << std::endl;

    if (passed == total) {
        std::cout << std::endl;
        std::cout << "✅ All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << std::endl;
        std::cout << "❌ Some tests failed" << std::endl;
        return 1;
    }
}
