// Test port separation with MISALIGNED ports (different Y positions)
// SPDX-License-Identifier: EPL-2.0

#include <elk/graph/graph.h>
#include <elk/alg/layered/layered_layout.h>
#include <iostream>
#include <fstream>
#include <memory>

using namespace elk;

void generateSVG(const std::string& filename, Node* root, const std::string& title) {
    std::ofstream svg(filename);

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

    // Draw edges with bend points
    svg << "  <g id=\"edges\" stroke=\"#666\" stroke-width=\"1.5\" fill=\"none\" marker-end=\"url(#arrow)\">\n";
    for (const auto& edge : root->edges) {
        if (!edge->sections.empty()) {
            const auto& section = edge->sections[0];
            svg << "    <path d=\"M " << section.startPoint.x << " " << section.startPoint.y;
            for (const auto& bendPoint : section.bendPoints) {
                svg << " L " << bendPoint.x << " " << bendPoint.y;
            }
            svg << " L " << section.endPoint.x << " " << section.endPoint.y;
            svg << "\" stroke=\"blue\"/>\n";
        }
    }
    svg << "  </g>\n";

    // Draw nodes and ports
    svg << "  <g id=\"nodes\">\n";
    for (const auto& child : root->children) {
        svg << "    <rect x=\"" << child->position.x << "\" y=\"" << child->position.y
            << "\" width=\"" << child->size.width << "\" height=\"" << child->size.height
            << "\" fill=\"#e3f2fd\" stroke=\"#1976d2\" stroke-width=\"2\" rx=\"2\"/>\n";
        svg << "    <text x=\"" << (child->position.x + child->size.width/2)
            << "\" y=\"" << (child->position.y + child->size.height/2 + 4)
            << "\" text-anchor=\"middle\" font-family=\"monospace\" font-size=\"10\" fill=\"#000\">"
            << child->id << "</text>\n";

        // Draw ports
        for (const auto& port : child->ports) {
            double portX = child->position.x + port->position.x;
            double portY = child->position.y + port->position.y;
            svg << "    <circle cx=\"" << portX << "\" cy=\"" << portY
                << "\" r=\"3\" fill=\"red\" stroke=\"darkred\" stroke-width=\"1\"/>\n";
        }
    }
    svg << "  </g>\n";
    svg << "</svg>\n";
    svg.close();
}

int main() {
    std::cout << "Testing MISALIGNED port routing\n" << std::endl;

    auto root = std::make_unique<Node>("test");

    // Source node
    auto src = root->addChild("src");
    src->size = Size(40, 60);
    auto src_out1 = src->addPort("out1", PortSide::EAST);
    src_out1->position = Point(40, 10);  // Top

    auto src_out2 = src->addPort("out2", PortSide::EAST);
    src_out2->position = Point(40, 50);  // Bottom

    // Target node - INTENTIONALLY MISALIGNED
    auto tgt = root->addChild("tgt");
    tgt->size = Size(40, 60);
    auto tgt_in1 = tgt->addPort("in1", PortSide::WEST);
    tgt_in1->position = Point(0, 50);   // Bottom (connects to src top - CROSSED!)

    auto tgt_in2 = tgt->addPort("in2", PortSide::WEST);
    tgt_in2->position = Point(0, 10);   // Top (connects to src bottom - CROSSED!)

    // Create CROSSED edges
    auto e1 = root->addEdge("e1");
    e1->sourcePorts.push_back(src_out1);  // src top
    e1->targetPorts.push_back(tgt_in1);   // tgt bottom
    src_out1->outgoingEdges.push_back(e1);
    tgt_in1->incomingEdges.push_back(e1);

    auto e2 = root->addEdge("e2");
    e2->sourcePorts.push_back(src_out2);  // src bottom
    e2->targetPorts.push_back(tgt_in2);   // tgt top
    src_out2->outgoingEdges.push_back(e2);
    tgt_in2->incomingEdges.push_back(e2);

    // Run layout
    layered::LayeredLayoutProvider layout;
    layout.setDirection(Direction::RIGHT);
    layout.setNodeSpacing(35.0);
    layout.setLayerSpacing(80.0);
    layout.layout(root.get(), nullptr);

    std::cout << "\nFinal node positions:\n";
    for (const auto& child : root->children) {
        std::cout << "  " << child->id << " at (" << child->position.x << ", " << child->position.y << ")\n";
        for (const auto& port : child->ports) {
            double absX = child->position.x + port->position.x;
            double absY = child->position.y + port->position.y;
            std::cout << "    " << port->id << " at abs (" << absX << ", " << absY << ")\n";
        }
    }

    std::cout << "\nEdge routing:\n";
    for (const auto& edge : root->edges) {
        std::cout << "  " << edge->id << ":\n";
        if (!edge->sections.empty()) {
            const auto& section = edge->sections[0];
            std::cout << "    start: (" << section.startPoint.x << ", " << section.startPoint.y << ")\n";
            for (size_t i = 0; i < section.bendPoints.size(); i++) {
                std::cout << "    bend" << i << ": (" << section.bendPoints[i].x << ", " << section.bendPoints[i].y << ")\n";
            }
            std::cout << "    end: (" << section.endPoint.x << ", " << section.endPoint.y << ")\n";
            std::cout << "    Total bend points: " << section.bendPoints.size() << "\n";
        }
    }

    generateSVG("/tmp/port_misaligned.svg", root.get(), "Misaligned Port Test");

    return 0;
}
