// ELK C++ - Simple usage example
// SPDX-License-Identifier: EPL-2.0

#include <elk/graph/graph.h>
#include <elk/alg/force/force_layout.h>
#include <elk/alg/layered/layered_layout.h>
#include <iostream>
#include <memory>

using namespace elk;

void printNodePositions(Node* root) {
    std::cout << "\nNode positions:\n";
    for (const auto& child : root->children) {
        std::cout << "  " << child->id << ": ("
                  << child->position.x << ", "
                  << child->position.y << ")\n";
    }
}

void forceLayoutExample() {
    std::cout << "=== Force-Directed Layout Example ===\n";

    // Create root graph
    auto root = std::make_unique<Node>("root");

    // Add nodes
    auto n1 = root->addChild("node1");
    n1->size = Size(60, 40);

    auto n2 = root->addChild("node2");
    n2->size = Size(60, 40);

    auto n3 = root->addChild("node3");
    n3->size = Size(60, 40);

    auto n4 = root->addChild("node4");
    n4->size = Size(60, 40);

    // Add edges
    connectNodes(n1, n2, root.get());
    connectNodes(n2, n3, root.get());
    connectNodes(n3, n4, root.get());
    connectNodes(n4, n1, root.get());

    // Run force layout
    force::ForceLayoutProvider layout;
    layout.setIterations(200);
    layout.setModel(force::ForceModel::FRUCHTERMAN_REINGOLD);

    layout.layout(root.get(), [](const std::string& task, double progress) {
        std::cout << task << ": " << (progress * 100) << "%\n";
    });

    printNodePositions(root.get());
}

void layeredLayoutExample() {
    std::cout << "\n=== Layered Layout Example ===\n";

    // Create root graph
    auto root = std::make_unique<Node>("root");

    // Add nodes
    auto n1 = root->addChild("A");
    n1->size = Size(60, 40);

    auto n2 = root->addChild("B");
    n2->size = Size(60, 40);

    auto n3 = root->addChild("C");
    n3->size = Size(60, 40);

    auto n4 = root->addChild("D");
    n4->size = Size(60, 40);

    auto n5 = root->addChild("E");
    n5->size = Size(60, 40);

    // Create a DAG
    connectNodes(n1, n2, root.get());
    connectNodes(n1, n3, root.get());
    connectNodes(n2, n4, root.get());
    connectNodes(n3, n4, root.get());
    connectNodes(n4, n5, root.get());

    // Run layered layout
    layered::LayeredLayoutProvider layout;
    layout.setDirection(Direction::RIGHT);
    layout.setNodeSpacing(20.0);
    layout.setLayerSpacing(80.0);

    layout.layout(root.get(), [](const std::string& task, double progress) {
        std::cout << task << ": " << (progress * 100) << "%\n";
    });

    printNodePositions(root.get());
}

void hierarchicalExample() {
    std::cout << "\n=== Hierarchical Graph Example ===\n";

    // Create root
    auto root = std::make_unique<Node>("root");

    // Add container node
    auto container = root->addChild("container");
    container->size = Size(300, 200);
    container->padding = Padding(20);

    // Add children inside container
    auto child1 = container->addChild("child1");
    child1->size = Size(50, 30);

    auto child2 = container->addChild("child2");
    child2->size = Size(50, 30);

    connectNodes(child1, child2, container);

    // Layout the container's children
    force::ForceLayoutProvider layout;
    layout.setIterations(100);
    layout.layout(container, nullptr);

    std::cout << "Container child positions:\n";
    for (const auto& child : container->children) {
        std::cout << "  " << child->id << ": ("
                  << child->position.x << ", "
                  << child->position.y << ")\n";
    }
}

int main() {
    forceLayoutExample();
    layeredLayoutExample();
    hierarchicalExample();

    std::cout << "\nDone!\n";
    return 0;
}
