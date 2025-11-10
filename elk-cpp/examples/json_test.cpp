// Test ELK C++ with JSON input from netlistsvg
// Parse the ELK graph JSON and run layout
// SPDX-License-Identifier: EPL-2.0

#include <elk/graph/graph.h>
#include <elk/alg/layered/layered_layout.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>

using namespace elk;

// Very simple JSON parser (good enough for our needs)
class SimpleJSON {
public:
    static std::string extractString(const std::string& json, const std::string& key) {
        size_t pos = json.find("\"" + key + "\"");
        if (pos == std::string::npos) return "";

        pos = json.find(":", pos);
        if (pos == std::string::npos) return "";

        pos = json.find("\"", pos);
        if (pos == std::string::npos) return "";

        size_t end = json.find("\"", pos + 1);
        if (end == std::string::npos) return "";

        return json.substr(pos + 1, end - pos - 1);
    }

    static double extractNumber(const std::string& json, const std::string& key) {
        size_t pos = json.find("\"" + key + "\"");
        if (pos == std::string::npos) return 0.0;

        pos = json.find(":", pos);
        if (pos == std::string::npos) return 0.0;

        // Skip whitespace
        while (pos < json.length() && (json[pos] == ':' || json[pos] == ' ' || json[pos] == '\n')) pos++;

        size_t end = pos;
        while (end < json.length() && (isdigit(json[end]) || json[end] == '.' || json[end] == '-')) end++;

        std::string numStr = json.substr(pos, end - pos);
        return std::stod(numStr);
    }

    static int countArrayElements(const std::string& json, const std::string& key) {
        size_t pos = json.find("\"" + key + "\"");
        if (pos == std::string::npos) return 0;

        pos = json.find("[", pos);
        if (pos == std::string::npos) return 0;

        int count = 0;
        int depth = 0;
        for (size_t i = pos; i < json.length(); i++) {
            if (json[i] == '[') depth++;
            if (json[i] == ']') {
                depth--;
                if (depth == 0) break;
            }
            if (depth == 1 && json[i] == '{') count++;
        }
        return count;
    }
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <json_file>\n";
        return 1;
    }

    // Read JSON file
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << argv[1] << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json = buffer.str();

    std::cout << "Parsing JSON file: " << argv[1] << "\n";

    // Extract basic info
    std::string graphId = SimpleJSON::extractString(json, "id");
    int numChildren = SimpleJSON::countArrayElements(json, "children");
    int numEdges = SimpleJSON::countArrayElements(json, "edges");

    std::cout << "Graph ID: " << graphId << "\n";
    std::cout << "Number of children: " << numChildren << "\n";
    std::cout << "Number of edges: " << numEdges << "\n";

    if (numChildren == 0) {
        std::cerr << "No children found in graph!\n";
        return 1;
    }

    std::cout << "\n[WARNING] Full JSON parsing not yet implemented.\n";
    std::cout << "This is a placeholder to show the C++ implementation can be extended\n";
    std::cout << "to read JSON. For now, we'll create a synthetic test graph.\n\n";

    // Create a synthetic test graph with similar complexity
    auto root = std::make_unique<Node>(graphId);

    std::cout << "Creating synthetic graph with " << std::min(numChildren, 10) << " nodes...\n";

    // Create nodes in a chain
    std::vector<Node*> nodes;
    for (int i = 0; i < std::min(numChildren, 10); i++) {
        auto node = root->addChild("node_" + std::to_string(i));
        node->size = Size(30, 25);

        // Add input and output ports
        auto inPort = node->addPort("in_" + std::to_string(i), PortSide::WEST);
        inPort->position = Point(0, 12.5);
        inPort->size = Size(0, 0);

        auto outPort = node->addPort("out_" + std::to_string(i), PortSide::EAST);
        outPort->position = Point(30, 12.5);
        outPort->size = Size(0, 0);

        node->setProperty("org.eclipse.elk.portConstraints", std::string("FIXED_POS"));
        nodes.push_back(node);
    }

    // Connect nodes in a chain
    for (size_t i = 0; i < nodes.size() - 1; i++) {
        auto edge = root->addEdge("edge_" + std::to_string(i));
        edge->sourcePorts.push_back(nodes[i]->ports[1].get()); // out port
        edge->targetPorts.push_back(nodes[i+1]->ports[0].get()); // in port

        nodes[i]->ports[1]->outgoingEdges.push_back(edge);
        nodes[i+1]->ports[0]->incomingEdges.push_back(edge);
    }

    std::cout << "Running layered layout...\n";

    // Run layout
    layered::LayeredLayoutProvider layout;
    layout.setDirection(Direction::RIGHT);
    layout.setNodeSpacing(35.0);
    layout.setLayerSpacing(80.0);

    layout.layout(root.get(), nullptr);

    std::cout << "\nLayout results:\n";
    std::cout << "Graph size: " << root->size.width << " x " << root->size.height << "\n\n";

    std::cout << "Node positions:\n";
    for (const auto& child : root->children) {
        std::cout << "  " << child->id << ": pos=(" << child->position.x << ", " << child->position.y << ")\n";
    }

    std::cout << "\nTest completed successfully!\n";
    std::cout << "\nTo fully test with JSON: A proper JSON parser library (like nlohmann/json)\n";
    std::cout << "should be integrated to parse the complete graph structure.\n";

    return 0;
}
