// Test to match netlistsvg elkjs behavior
// Replicates test/analog/vcc_and_gnd.json
// SPDX-License-Identifier: EPL-2.0

#include <elk/graph/graph.h>
#include <elk/alg/layered/layered_layout.h>
#include <elk/core/properties.h>
#include <iostream>
#include <iomanip>
#include <memory>

using namespace elk;

void printInputGraph(Node* root) {
    std::cout << "\n========== ELK INPUT GRAPH ==========" << std::endl;
    std::cout << "Number of nodes (children): " << root->children.size() << std::endl;
    std::cout << "Number of edges: " << root->edges.size() << std::endl;

    std::cout << "\nNodes:" << std::endl;
    for (const auto& child : root->children) {
        std::cout << "  " << child->id << ":" << std::endl;
        std::cout << "    size=(" << child->size.width << ", " << child->size.height << ")" << std::endl;
        std::cout << "    ports:" << std::endl;
        for (const auto& port : child->ports) {
            std::cout << "      " << port->id << ": pos=(" << port->position.x << ", " << port->position.y << ")" << std::endl;
        }
    }

    std::cout << "\nEdges:" << std::endl;
    for (const auto& edge : root->edges) {
        std::cout << "  " << edge->id << ":" << std::endl;
        if (!edge->sourcePorts.empty() && !edge->targetPorts.empty()) {
            std::cout << "    source port: " << edge->sourcePorts[0]->id << std::endl;
            std::cout << "    target port: " << edge->targetPorts[0]->id << std::endl;
        }
    }
}

void printOutputGraph(Node* root) {
    std::cout << "\n========== ELK OUTPUT GRAPH ==========" << std::endl;
    std::cout << "Graph dimensions: " << root->size.width << " x " << root->size.height << std::endl;

    std::cout << "\nNode positions:" << std::endl;
    for (const auto& child : root->children) {
        std::cout << "  " << child->id << ": pos=("
                  << child->position.x << ", " << child->position.y << "), size=("
                  << child->size.width << ", " << child->size.height << ")" << std::endl;

        for (const auto& port : child->ports) {
            std::cout << "    port " << port->id << ": pos=("
                      << port->position.x << ", " << port->position.y << ")" << std::endl;
        }
    }

    std::cout << "\nEdge routing:" << std::endl;
    for (const auto& edge : root->edges) {
        std::string sourceId = edge->sourcePorts.empty() ? "none" : edge->sourcePorts[0]->id;
        std::string targetId = edge->targetPorts.empty() ? "none" : edge->targetPorts[0]->id;
        std::cout << "  " << edge->id << ": " << sourceId << " -> " << targetId << std::endl;

        for (size_t i = 0; i < edge->sections.size(); ++i) {
            const auto& section = edge->sections[i];
            std::cout << "    section " << i << ": start=("
                      << section.startPoint.x << ", " << section.startPoint.y
                      << "), end=(" << section.endPoint.x << ", " << section.endPoint.y << ")" << std::endl;

            if (!section.bendPoints.empty()) {
                std::cout << "      bendPoints: ";
                for (const auto& bp : section.bendPoints) {
                    std::cout << "(" << bp.x << ", " << bp.y << ") ";
                }
                std::cout << std::endl;
            }
        }
    }
    std::cout << "========================================\n" << std::endl;
}

int main() {
    // Create root graph matching the netlistsvg "test" module
    auto root = std::make_unique<Node>("test");

    // Create VCC node
    auto vcc = root->addChild("vcc");
    vcc->size = Size(20, 30);

    // Add port to VCC with fixed position
    auto vccPort = vcc->addPort("vcc.A", PortSide::SOUTH);
    vccPort->position = Point(10, 30);  // Bottom center of the VCC component
    vccPort->size = Size(0, 0);

    // Set port constraints to FIXED_POS (matching the JSON)
    vcc->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));

    // Create GND node
    auto gnd = root->addChild("gnd");
    gnd->size = Size(20, 30);

    // Add port to GND with fixed position
    auto gndPort = gnd->addPort("gnd.A", PortSide::NORTH);
    gndPort->position = Point(10, -15);  // Top center of the GND component (negative y extends above)
    gndPort->size = Size(0, 0);

    // Set port constraints to FIXED_POS
    gnd->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));

    // Create edge from VCC to GND
    auto edge = root->addEdge("e0");
    edge->sourcePorts.push_back(vccPort);
    edge->targetPorts.push_back(gndPort);

    // Set edge properties matching the JSON
    edge->setProperty("org.eclipse.elk.layered.priority.direction", 10);
    edge->setProperty("org.eclipse.elk.edge.thickness", 1);

    // Update port connectivity
    vccPort->outgoingEdges.push_back(edge);
    gndPort->incomingEdges.push_back(edge);

    // Print input graph
    printInputGraph(root.get());

    // Configure layout options matching the analog.svg skin
    std::cout << "\nLayout Options:" << std::endl;
    std::cout << "  org.eclipse.elk.layered.spacing.nodeNodeBetweenLayers: 5" << std::endl;
    std::cout << "  org.eclipse.elk.layered.compaction.postCompaction.strategy: 4" << std::endl;
    std::cout << "  org.eclipse.elk.spacing.nodeNode: 35" << std::endl;
    std::cout << "  org.eclipse.elk.direction: DOWN" << std::endl;

    // Run layered layout with DOWN direction
    layered::LayeredLayoutProvider layout;
    layout.setDirection(Direction::DOWN);
    layout.setNodeSpacing(35.0);  // org.eclipse.elk.spacing.nodeNode
    layout.setLayerSpacing(5.0);   // org.eclipse.elk.layered.spacing.nodeNodeBetweenLayers

    // Note: compaction strategy would need to be implemented separately

    layout.layout(root.get(), [](const std::string& task, double progress) {
        // Optional: print progress
        // std::cout << task << ": " << (progress * 100) << "%\n";
    });

    // Print output graph
    printOutputGraph(root.get());

    // Compare with expected JavaScript output:
    // vcc: pos=(12, 12), size=(20, 30)
    // gnd: pos=(12, 62), size=(20, 30)
    // Graph dimensions: 44 x 104
    std::cout << "\n========== COMPARISON ==========" << std::endl;
    std::cout << "Expected from JavaScript:" << std::endl;
    std::cout << "  vcc: pos=(12, 12)" << std::endl;
    std::cout << "  gnd: pos=(12, 62)" << std::endl;
    std::cout << "  Graph: 44 x 104" << std::endl;
    std::cout << "\nActual from C++:" << std::endl;
    std::cout << "  vcc: pos=(" << vcc->position.x << ", " << vcc->position.y << ")" << std::endl;
    std::cout << "  gnd: pos=(" << gnd->position.x << ", " << gnd->position.y << ")" << std::endl;
    std::cout << "  Graph: " << root->size.width << " x " << root->size.height << std::endl;

    // Calculate differences
    double vcc_x_diff = std::abs(vcc->position.x - 12.0);
    double vcc_y_diff = std::abs(vcc->position.y - 12.0);
    double gnd_x_diff = std::abs(gnd->position.x - 12.0);
    double gnd_y_diff = std::abs(gnd->position.y - 62.0);
    double width_diff = std::abs(root->size.width - 44.0);
    double height_diff = std::abs(root->size.height - 104.0);

    std::cout << "\nDifferences:" << std::endl;
    std::cout << "  vcc x: " << vcc_x_diff << std::endl;
    std::cout << "  vcc y: " << vcc_y_diff << std::endl;
    std::cout << "  gnd x: " << gnd_x_diff << std::endl;
    std::cout << "  gnd y: " << gnd_y_diff << std::endl;
    std::cout << "  width: " << width_diff << std::endl;
    std::cout << "  height: " << height_diff << std::endl;

    double tolerance = 1.0;  // Allow 1 pixel difference
    bool match = (vcc_x_diff < tolerance && vcc_y_diff < tolerance &&
                  gnd_x_diff < tolerance && gnd_y_diff < tolerance &&
                  width_diff < tolerance && height_diff < tolerance);

    std::cout << "\nResult: " << (match ? "✓ MATCH" : "✗ MISMATCH") << std::endl;
    std::cout << "================================\n" << std::endl;

    return match ? 0 : 1;
}
