// Test port separation with multiple ports on same node side
// SPDX-License-Identifier: EPL-2.0

#include <elk/graph/graph.h>
#include <elk/alg/layered/layered_layout.h>
#include <iostream>
#include <fstream>
#include <memory>

using namespace elk;

void generateSVG(const std::string& filename, Node* root, const std::string& title) {
    std::ofstream svg(filename);

    // Calculate bounds
    double maxX = 0, maxY = 0;
    for (const auto& child : root->children) {
        maxX = std::max(maxX, child->position.x + child->size.width);
        maxY = std::max(maxY, child->position.y + child->size.height);
    }

    double width = maxX + 24;
    double height = maxY + 24;

    svg << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << width << "\" height=\"" << height << "\">\n";
    svg << "  <title>" << title << "</title>\n";
    svg << "  <rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";

    // Draw edges
    svg << "  <g id=\"edges\" stroke=\"#666\" stroke-width=\"1.5\" fill=\"none\">\n";
    for (const auto& edge : root->edges) {
        if (!edge->sections.empty()) {
            const auto& section = edge->sections[0];
            svg << "    <path d=\"M " << section.startPoint.x << " " << section.startPoint.y;
            for (const auto& bendPoint : section.bendPoints) {
                svg << " L " << bendPoint.x << " " << bendPoint.y;
            }
            svg << " L " << section.endPoint.x << " " << section.endPoint.y;
            svg << "\" stroke=\"blue\" marker-end=\"url(#arrow)\"/>\n";
        }
    }
    svg << "  </g>\n";

    // Draw nodes and ports
    svg << "  <g id=\"nodes\">\n";
    for (const auto& child : root->children) {
        // Draw node box
        svg << "    <rect x=\"" << child->position.x << "\" y=\"" << child->position.y
            << "\" width=\"" << child->size.width << "\" height=\"" << child->size.height
            << "\" fill=\"#e3f2fd\" stroke=\"#1976d2\" stroke-width=\"2\" rx=\"2\"/>\n";

        // Draw node label
        svg << "    <text x=\"" << (child->position.x + child->size.width/2)
            << "\" y=\"" << (child->position.y + child->size.height/2 + 4)
            << "\" text-anchor=\"middle\" font-family=\"monospace\" font-size=\"10\" fill=\"#000\">"
            << child->id << "</text>\n";

        // Draw ports as small circles
        for (const auto& port : child->ports) {
            double portX = child->position.x + port->position.x;
            double portY = child->position.y + port->position.y;
            svg << "    <circle cx=\"" << portX << "\" cy=\"" << portY
                << "\" r=\"3\" fill=\"red\" stroke=\"darkred\" stroke-width=\"1\"/>\n";
            svg << "    <text x=\"" << (portX + 8) << "\" y=\"" << (portY + 3)
                << "\" font-family=\"monospace\" font-size=\"8\" fill=\"#666\">"
                << port->id << "</text>\n";
        }
    }
    svg << "  </g>\n";

    svg << "</svg>\n";
    svg.close();

    std::cout << "Generated " << filename << " (" << width << "x" << height << ")\n";
}

int main() {
    std::cout << "Testing port separation\n" << std::endl;

    auto root = std::make_unique<Node>("test");

    // Source node with single output
    auto src = root->addChild("src");
    src->size = Size(40, 60);
    auto src_out1 = src->addPort("out1", PortSide::EAST);
    src_out1->position = Point(40, 10);  // Near top
    auto src_out2 = src->addPort("out2", PortSide::EAST);
    src_out2->position = Point(40, 30);  // Middle
    auto src_out3 = src->addPort("out3", PortSide::EAST);
    src_out3->position = Point(40, 50);  // Near bottom

    // Target node with multiple inputs
    auto tgt = root->addChild("tgt");
    tgt->size = Size(40, 60);
    auto tgt_in1 = tgt->addPort("in1", PortSide::WEST);
    tgt_in1->position = Point(0, 10);   // Near top
    auto tgt_in2 = tgt->addPort("in2", PortSide::WEST);
    tgt_in2->position = Point(0, 30);   // Middle
    auto tgt_in3 = tgt->addPort("in3", PortSide::WEST);
    tgt_in3->position = Point(0, 50);   // Near bottom

    // Create edges connecting ports
    auto e1 = root->addEdge("e1");
    e1->sourcePorts.push_back(src_out1);
    e1->targetPorts.push_back(tgt_in1);
    src_out1->outgoingEdges.push_back(e1);
    tgt_in1->incomingEdges.push_back(e1);

    auto e2 = root->addEdge("e2");
    e2->sourcePorts.push_back(src_out2);
    e2->targetPorts.push_back(tgt_in2);
    src_out2->outgoingEdges.push_back(e2);
    tgt_in2->incomingEdges.push_back(e2);

    auto e3 = root->addEdge("e3");
    e3->sourcePorts.push_back(src_out3);
    e3->targetPorts.push_back(tgt_in3);
    src_out3->outgoingEdges.push_back(e3);
    tgt_in3->incomingEdges.push_back(e3);

    // Print port positions BEFORE layout
    std::cout << "Port positions BEFORE layout:\n";
    for (const auto& child : root->children) {
        std::cout << "  " << child->id << ":\n";
        for (const auto& port : child->ports) {
            std::cout << "    " << port->id << ": (" << port->position.x << ", " << port->position.y << ")\n";
        }
    }

    // Run layout
    layered::LayeredLayoutProvider layout;
    layout.setDirection(Direction::RIGHT);
    layout.setNodeSpacing(35.0);
    layout.setLayerSpacing(80.0);
    layout.layout(root.get(), nullptr);

    // Print port positions AFTER layout
    std::cout << "\nPort positions AFTER layout:\n";
    for (const auto& child : root->children) {
        std::cout << "  " << child->id << " at (" << child->position.x << ", " << child->position.y << "):\n";
        for (const auto& port : child->ports) {
            std::cout << "    " << port->id << ": (" << port->position.x << ", " << port->position.y << ")\n";
        }
    }

    // Print edge routing
    std::cout << "\nEdge sections:\n";
    for (const auto& edge : root->edges) {
        std::cout << "  " << edge->id << ":\n";
        if (!edge->sections.empty()) {
            const auto& section = edge->sections[0];
            std::cout << "    start: (" << section.startPoint.x << ", " << section.startPoint.y << ")\n";
            for (size_t i = 0; i < section.bendPoints.size(); i++) {
                std::cout << "    bend" << i << ": (" << section.bendPoints[i].x << ", " << section.bendPoints[i].y << ")\n";
            }
            std::cout << "    end: (" << section.endPoint.x << ", " << section.endPoint.y << ")\n";
        }
    }

    generateSVG("/tmp/port_separation.svg", root.get(), "Port Separation Test");

    return 0;
}
