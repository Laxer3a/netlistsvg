// Test ELK C++ with multi-layer graph
// Simulates a more complex digital circuit pattern
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
    std::cout << "Testing C++ ELK with multi-layer graph\n" << std::endl;

    // Multi-layer test: 3 layers with multiple nodes per layer
    // Layer 0: 2 source nodes (s0, s1)
    // Layer 1: 4 processing nodes (p0, p1, p2, p3)
    // Layer 2: 2 sink nodes (t0, t1)
    {
        auto root = std::make_unique<Node>("multilayer");

        // Create source nodes
        auto s0 = root->addChild("s0");
        s0->size = Size(30, 25);
        auto s0_out = s0->addPort("out", PortSide::EAST);
        s0_out->position = Point(30, 12.5);
        s0->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));

        auto s1 = root->addChild("s1");
        s1->size = Size(30, 25);
        auto s1_out = s1->addPort("out", PortSide::EAST);
        s1_out->position = Point(30, 12.5);
        s1->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));

        // Create processing nodes
        auto p0 = root->addChild("p0");
        p0->size = Size(30, 25);
        auto p0_in = p0->addPort("in", PortSide::WEST);
        p0_in->position = Point(0, 12.5);
        auto p0_out = p0->addPort("out", PortSide::EAST);
        p0_out->position = Point(30, 12.5);
        p0->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));

        auto p1 = root->addChild("p1");
        p1->size = Size(30, 25);
        auto p1_in = p1->addPort("in", PortSide::WEST);
        p1_in->position = Point(0, 12.5);
        auto p1_out = p1->addPort("out", PortSide::EAST);
        p1_out->position = Point(30, 12.5);
        p1->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));

        auto p2 = root->addChild("p2");
        p2->size = Size(30, 25);
        auto p2_in = p2->addPort("in", PortSide::WEST);
        p2_in->position = Point(0, 12.5);
        auto p2_out = p2->addPort("out", PortSide::EAST);
        p2_out->position = Point(30, 12.5);
        p2->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));

        auto p3 = root->addChild("p3");
        p3->size = Size(30, 25);
        auto p3_in = p3->addPort("in", PortSide::WEST);
        p3_in->position = Point(0, 12.5);
        auto p3_out = p3->addPort("out", PortSide::EAST);
        p3_out->position = Point(30, 12.5);
        p3->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));

        // Create sink nodes
        auto t0 = root->addChild("t0");
        t0->size = Size(30, 25);
        auto t0_in = t0->addPort("in", PortSide::WEST);
        t0_in->position = Point(0, 12.5);
        t0->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));

        auto t1 = root->addChild("t1");
        t1->size = Size(30, 25);
        auto t1_in = t1->addPort("in", PortSide::WEST);
        t1_in->position = Point(0, 12.5);
        t1->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));

        // Create edges - fan-out then fan-in pattern
        // s0 -> p0, p1
        auto e0 = root->addEdge("e0");
        e0->sourcePorts.push_back(s0_out);
        e0->targetPorts.push_back(p0_in);
        s0_out->outgoingEdges.push_back(e0);
        p0_in->incomingEdges.push_back(e0);

        auto e1 = root->addEdge("e1");
        e1->sourcePorts.push_back(s0_out);
        e1->targetPorts.push_back(p1_in);
        s0_out->outgoingEdges.push_back(e1);
        p1_in->incomingEdges.push_back(e1);

        // s1 -> p2, p3
        auto e2 = root->addEdge("e2");
        e2->sourcePorts.push_back(s1_out);
        e2->targetPorts.push_back(p2_in);
        s1_out->outgoingEdges.push_back(e2);
        p2_in->incomingEdges.push_back(e2);

        auto e3 = root->addEdge("e3");
        e3->sourcePorts.push_back(s1_out);
        e3->targetPorts.push_back(p3_in);
        s1_out->outgoingEdges.push_back(e3);
        p3_in->incomingEdges.push_back(e3);

        // p0, p1 -> t0
        auto e4 = root->addEdge("e4");
        e4->sourcePorts.push_back(p0_out);
        e4->targetPorts.push_back(t0_in);
        p0_out->outgoingEdges.push_back(e4);
        t0_in->incomingEdges.push_back(e4);

        auto e5 = root->addEdge("e5");
        e5->sourcePorts.push_back(p1_out);
        e5->targetPorts.push_back(t0_in);
        p1_out->outgoingEdges.push_back(e5);
        t0_in->incomingEdges.push_back(e5);

        // p2, p3 -> t1
        auto e6 = root->addEdge("e6");
        e6->sourcePorts.push_back(p2_out);
        e6->targetPorts.push_back(t1_in);
        p2_out->outgoingEdges.push_back(e6);
        t1_in->incomingEdges.push_back(e6);

        auto e7 = root->addEdge("e7");
        e7->sourcePorts.push_back(p3_out);
        e7->targetPorts.push_back(t1_in);
        p3_out->outgoingEdges.push_back(e7);
        t1_in->incomingEdges.push_back(e7);

        layered::LayeredLayoutProvider layout;
        layout.setDirection(Direction::RIGHT);
        layout.setNodeSpacing(35.0);
        layout.setLayerSpacing(80.0);
        layout.layout(root.get(), nullptr);

        printResults("Multi-layer test (8 nodes, 8 edges)", root.get());
    }

    std::cout << "\nTest completed!" << std::endl;
    return 0;
}
