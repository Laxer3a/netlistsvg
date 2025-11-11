// Test ELK C++ with SVG output for visual comparison
// SPDX-License-Identifier: EPL-2.0

#include <elk/graph/graph.h>
#include <elk/alg/layered/layered_layout.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <map>
#include <set>

using namespace elk;

void generateSVG(const std::string& filename, Node* root, const std::string& title) {
    std::ofstream svg(filename);

    // Calculate bounds
    double maxX = 0, maxY = 0;
    for (const auto& child : root->children) {
        maxX = std::max(maxX, child->position.x + child->size.width);
        maxY = std::max(maxY, child->position.y + child->size.height);
    }

    double width = maxX + 24;  // Add padding
    double height = maxY + 24;

    svg << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << width << "\" height=\"" << height << "\">\n";
    svg << "  <title>" << title << "</title>\n";
    svg << "  <defs>\n";
    svg << "    <marker id=\"arrow\" markerWidth=\"10\" markerHeight=\"10\" refX=\"9\" refY=\"3\" orient=\"auto\" markerUnits=\"strokeWidth\">\n";
    svg << "      <path d=\"M0,0 L0,6 L9,3 z\" fill=\"#666\"/>\n";
    svg << "    </marker>\n";
    svg << "  </defs>\n";
    svg << "  <rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";

    // Draw edges first (so they're behind nodes)
    svg << "  <g id=\"edges\" stroke=\"#666\" stroke-width=\"1.5\" fill=\"none\" marker-end=\"url(#arrow)\">\n";
    for (const auto& edge : root->edges) {
        if (!edge->sections.empty()) {
            // Use edge sections with bend points for orthogonal routing
            const auto& section = edge->sections[0];

            // Build path using all bend points
            svg << "    <path d=\"M " << section.startPoint.x << " " << section.startPoint.y;

            // Add all intermediate bend points (creating 90-degree turns)
            for (const auto& bendPoint : section.bendPoints) {
                svg << " L " << bendPoint.x << " " << bendPoint.y;
            }

            // End at target
            svg << " L " << section.endPoint.x << " " << section.endPoint.y;
            svg << "\"/>\n";
        } else if (!edge->sourcePorts.empty() && !edge->targetPorts.empty()) {
            // Fallback to straight line if no sections
            auto* sourcePort = edge->sourcePorts[0];
            auto* targetPort = edge->targetPorts[0];

            Node* sourceNode = sourcePort->parent;
            Node* targetNode = targetPort->parent;

            if (sourceNode && targetNode) {
                double x1 = sourceNode->position.x + sourcePort->position.x;
                double y1 = sourceNode->position.y + sourcePort->position.y;
                double x2 = targetNode->position.x + targetPort->position.x;
                double y2 = targetNode->position.y + targetPort->position.y;

                svg << "    <line x1=\"" << x1 << "\" y1=\"" << y1
                    << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\"/>\n";
            }
        }
    }
    svg << "  </g>\n";

    // Draw junction points (dots where edges merge/split, NOT at simple turns)
    svg << "  <g id=\"junction-points\" fill=\"#666\">\n";

    // First pass: count how many times each coordinate appears
    std::map<std::pair<double, double>, int> pointCount;
    for (const auto& edge : root->edges) {
        if (!edge->sections.empty()) {
            const auto& section = edge->sections[0];
            for (const auto& bendPoint : section.bendPoints) {
                auto key = std::make_pair(bendPoint.x, bendPoint.y);
                pointCount[key]++;
            }
        }
    }

    // Second pass: only draw circles where 2+ edges meet (merge/split points)
    std::set<std::pair<double, double>> drawn;
    for (const auto& entry : pointCount) {
        if (entry.second >= 2 && drawn.find(entry.first) == drawn.end()) {
            svg << "    <circle cx=\"" << entry.first.first << "\" cy=\"" << entry.first.second
                << "\" r=\"2\"/>\n";
            drawn.insert(entry.first);
        }
    }
    svg << "  </g>\n";

    // Draw nodes
    svg << "  <g id=\"nodes\">\n";
    for (const auto& child : root->children) {
        svg << "    <g id=\"" << child->id << "\">\n";
        svg << "      <rect x=\"" << child->position.x << "\" y=\"" << child->position.y
            << "\" width=\"" << child->size.width << "\" height=\"" << child->size.height
            << "\" fill=\"#e3f2fd\" stroke=\"#1976d2\" stroke-width=\"2\" rx=\"2\"/>\n";
        svg << "      <text x=\"" << (child->position.x + child->size.width/2)
            << "\" y=\"" << (child->position.y + child->size.height/2 + 4)
            << "\" text-anchor=\"middle\" font-family=\"monospace\" font-size=\"10\" fill=\"#000\">"
            << child->id << "</text>\n";
        svg << "    </g>\n";
    }
    svg << "  </g>\n";

    // Add title
    svg << "  <text x=\"10\" y=\"15\" font-family=\"Arial\" font-size=\"14\" font-weight=\"bold\" fill=\"#000\">"
        << title << "</text>\n";

    svg << "</svg>\n";
    svg.close();

    std::cout << "Generated " << filename << " (" << width << "x" << height << ")\n";
}

int main() {
    std::cout << "Generating SVG visualization\n" << std::endl;

    // Multi-layer test
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

    // Create edges
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

    generateSVG("/tmp/cpp_layout.svg", root.get(), "C++ ELK Layout");

    std::cout << "\nNode positions:\n";
    for (const auto& child : root->children) {
        std::cout << "  " << child->id << ": (" << child->position.x << ", " << child->position.y << ")\n";
    }

    return 0;
}
