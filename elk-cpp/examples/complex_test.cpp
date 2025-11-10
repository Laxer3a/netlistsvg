// Test ELK C++ with a more complex graph
// Based on simplified digital circuit patterns
// SPDX-License-Identifier: EPL-2.0

#include <elk/graph/graph.h>
#include <elk/alg/layered/layered_layout.h>
#include <iostream>
#include <memory>

using namespace elk;

void printResults(const std::string& testName, Node* root) {
    std::cout << "\n=== " << testName << " ===" << std::endl;
    std::cout << "Graph size: " << root->size.width << " x " << root->size.height << std::endl;
    std::cout << "Node positions:" << std::endl;
    for (const auto& child : root->children) {
        std::cout << "  " << child->id << ": (" << child->position.x << ", " << child->position.y << ")" << std::endl;
    }
}

int main() {
    std::cout << "Testing C++ ELK with more complex graphs\n" << std::endl;

    // Test 1: Simple Chain (4 nodes)
    {
        auto root = std::make_unique<Node>("chain");

        Node* nodes[4];
        for (int i = 0; i < 4; i++) {
            nodes[i] = root->addChild("n" + std::to_string(i));
            nodes[i]->size = Size(30, 25);

            auto inPort = nodes[i]->addPort("in", PortSide::WEST);
            inPort->position = Point(0, 12.5);
            inPort->size = Size(0, 0);

            auto outPort = nodes[i]->addPort("out", PortSide::EAST);
            outPort->position = Point(30, 12.5);
            outPort->size = Size(0, 0);

            nodes[i]->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));
        }

        // Chain connection: n0 -> n1 -> n2 -> n3
        for (int i = 0; i < 3; i++) {
            auto edge = root->addEdge("e" + std::to_string(i));
            edge->sourcePorts.push_back(nodes[i]->ports[1].get());
            edge->targetPorts.push_back(nodes[i+1]->ports[0].get());
            nodes[i]->ports[1]->outgoingEdges.push_back(edge);
            nodes[i+1]->ports[0]->incomingEdges.push_back(edge);
        }

        layered::LayeredLayoutProvider layout;
        layout.setDirection(Direction::RIGHT);
        layout.setNodeSpacing(35.0);
        layout.setLayerSpacing(80.0);
        layout.layout(root.get(), nullptr);

        printResults("Test 1: Chain (4 nodes)", root.get());
    }

    // Test 2: DAG (diamond pattern)
    {
        auto root = std::make_unique<Node>("dag");

        auto n0 = root->addChild("n0");
        n0->size = Size(30, 25);
        auto n0out = n0->addPort("out", PortSide::EAST);
        n0out->position = Point(30, 12.5);
        n0->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));

        auto n1 = root->addChild("n1");
        n1->size = Size(30, 25);
        auto n1in = n1->addPort("in", PortSide::WEST);
        n1in->position = Point(0, 12.5);
        auto n1out = n1->addPort("out", PortSide::EAST);
        n1out->position = Point(30, 12.5);
        n1->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));

        auto n2 = root->addChild("n2");
        n2->size = Size(30, 25);
        auto n2in = n2->addPort("in", PortSide::WEST);
        n2in->position = Point(0, 12.5);
        auto n2out = n2->addPort("out", PortSide::EAST);
        n2out->position = Point(30, 12.5);
        n2->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));

        auto n3 = root->addChild("n3");
        n3->size = Size(30, 25);
        auto n3in = n3->addPort("in", PortSide::WEST);
        n3in->position = Point(0, 12.5);
        n3->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));

        // Diamond: n0 -> n1, n0 -> n2, n1 -> n3, n2 -> n3
        auto e0 = root->addEdge("e0");
        e0->sourcePorts.push_back(n0out);
        e0->targetPorts.push_back(n1in);
        n0out->outgoingEdges.push_back(e0);
        n1in->incomingEdges.push_back(e0);

        auto e1 = root->addEdge("e1");
        e1->sourcePorts.push_back(n0out);
        e1->targetPorts.push_back(n2in);
        n0out->outgoingEdges.push_back(e1);
        n2in->incomingEdges.push_back(e1);

        auto e2 = root->addEdge("e2");
        e2->sourcePorts.push_back(n1out);
        e2->targetPorts.push_back(n3in);
        n1out->outgoingEdges.push_back(e2);
        n3in->incomingEdges.push_back(e2);

        auto e3 = root->addEdge("e3");
        e3->sourcePorts.push_back(n2out);
        e3->targetPorts.push_back(n3in);
        n2out->outgoingEdges.push_back(e3);
        n3in->incomingEdges.push_back(e3);

        layered::LayeredLayoutProvider layout;
        layout.setDirection(Direction::RIGHT);
        layout.setNodeSpacing(35.0);
        layout.setLayerSpacing(80.0);
        layout.layout(root.get(), nullptr);

        printResults("Test 2: DAG (diamond)", root.get());
    }

    // Test 3: DOWN direction with port extents
    {
        auto root = std::make_unique<Node>("down_test");

        auto n0 = root->addChild("top");
        n0->size = Size(25, 20);
        auto n0port = n0->addPort("out", PortSide::SOUTH);
        n0port->position = Point(12.5, 25);  // Extends 5 pixels below node
        n0port->size = Size(0, 0);
        n0->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));

        auto n1 = root->addChild("bottom");
        n1->size = Size(25, 20);
        auto n1port = n1->addPort("in", PortSide::NORTH);
        n1port->position = Point(12.5, -10);  // Extends 10 pixels above node
        n1port->size = Size(0, 0);
        n1->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));

        auto edge = root->addEdge("e0");
        edge->sourcePorts.push_back(n0port);
        edge->targetPorts.push_back(n1port);
        n0port->outgoingEdges.push_back(edge);
        n1port->incomingEdges.push_back(edge);

        layered::LayeredLayoutProvider layout;
        layout.setDirection(Direction::DOWN);
        layout.setNodeSpacing(35.0);
        layout.setLayerSpacing(5.0);
        layout.layout(root.get(), nullptr);

        printResults("Test 3: DOWN with port extents", root.get());

        // Expected: gap between port positions should be exactly 5 pixels
        double topPortBottom = n0->position.y + n0port->position.y;
        double bottomPortTop = n1->position.y + n1port->position.y;
        double actualGap = bottomPortTop - topPortBottom;
        std::cout << "  Port gap: " << actualGap << " (expected: 5.0)" << std::endl;

        if (std::abs(actualGap - 5.0) < 1.0) {
            std::cout << "  ✓ PASS" << std::endl;
        } else {
            std::cout << "  ✗ FAIL" << std::endl;
        }
    }

    std::cout << "\nAll tests completed!" << std::endl;
    return 0;
}
