const fs = require('fs');

// Read the ELK graph JSON
const graph = JSON.parse(fs.readFileSync('/tmp/up3down5_elk_graph.json', 'utf8'));

console.log('// Auto-generated C++ code for up3down5 graph');
console.log('// Generated from netlistsvg + elkjs output');
console.log('// ' + graph.children.length + ' nodes, ' + graph.edges.length + ' edges\n');

console.log('#include <elk/graph/graph.h>');
console.log('#include <elk/alg/layered/layered_layout.h>');
console.log('#include <iostream>');
console.log('#include <memory>');
console.log('#include <map>\n');

console.log('using namespace elk;\n');

console.log('int main() {');
console.log('    auto root = std::make_unique<Node>("up3down5");\n');
console.log('    std::map<std::string, Node*> nodeMap;\n');

// Generate node creation code
console.log('    // Create nodes');
graph.children.forEach((node, i) => {
    const safeId = 'node_' + node.id.replace(/\$/g, '_D_').replace(/:/g, '_C_').replace(/\./g, '_P_').replace(/,/g, '_');
    console.log(`    auto ${safeId} = root->addChild("${node.id}");`);
    console.log(`    ${safeId}->size = Size(${node.width}, ${node.height});`);
    console.log(`    nodeMap["${node.id}"] = ${safeId};`);

    // Create ports
    if (node.ports && node.ports.length > 0) {
        node.ports.forEach((port, j) => {
            const portSide = port.x === 0 ? 'WEST' : (port.x > node.width / 2 ? 'EAST' : 'NORTH');
            console.log(`    auto ${safeId}_p${j} = ${safeId}->addPort("${port.id}", PortSide::${portSide});`);
            console.log(`    ${safeId}_p${j}->position = Point(${port.x}, ${port.y});`);
        });
    }
    console.log();
});

console.log('    // Create edges');
graph.edges.forEach((edge, i) => {
    console.log(`    auto e${i} = root->addEdge("${edge.id}");`);

    edge.sources.forEach(srcPort => {
        // Find which node this port belongs to (node IDs can contain dots!)
        const srcNode = graph.children.find(n => n.ports && n.ports.some(p => p.id === srcPort));
        if (!srcNode) {
            console.error(`ERROR: Source port ${srcPort} not found in any node for edge ${edge.id}`);
        } else {
            const srcNodeId = srcNode.id;
            const safeNodeId = 'node_' + srcNodeId.replace(/\$/g, '_D_').replace(/:/g, '_C_').replace(/\./g, '_P_').replace(/,/g, '_');
            const portIdx = srcNode.ports.findIndex(p => p.id === srcPort);
            if (portIdx >= 0) {
                console.log(`    e${i}->sourcePorts.push_back(${safeNodeId}_p${portIdx});`);
                console.log(`    ${safeNodeId}_p${portIdx}->outgoingEdges.push_back(e${i});`);
            } else {
                console.error(`ERROR: Source port ${srcPort} not found in node ${srcNodeId} for edge ${edge.id}`);
            }
        }
    });

    edge.targets.forEach(tgtPort => {
        // Find which node this port belongs to (node IDs can contain dots!)
        const tgtNode = graph.children.find(n => n.ports && n.ports.some(p => p.id === tgtPort));
        if (!tgtNode) {
            console.error(`ERROR: Target port ${tgtPort} not found in any node for edge ${edge.id}`);
        } else {
            const tgtNodeId = tgtNode.id;
            const safeNodeId = 'node_' + tgtNodeId.replace(/\$/g, '_D_').replace(/:/g, '_C_').replace(/\./g, '_P_').replace(/,/g, '_');
            const portIdx = tgtNode.ports.findIndex(p => p.id === tgtPort);
            if (portIdx >= 0) {
                console.log(`    e${i}->targetPorts.push_back(${safeNodeId}_p${portIdx});`);
                console.log(`    ${safeNodeId}_p${portIdx}->incomingEdges.push_back(e${i});`);
            } else {
                console.error(`ERROR: Target port ${tgtPort} not found in node ${tgtNodeId} for edge ${edge.id}`);
            }
        }
    });
    console.log();
});

console.log('    // Run layout');
console.log('    layered::LayeredLayoutProvider layout;');
console.log('    layout.setDirection(Direction::RIGHT);');
console.log('    layout.setNodeSpacing(35.0);');
console.log('    layout.setLayerSpacing(35.0);');
console.log('    layout.layout(root.get(), nullptr);\n');

console.log('    // Print results');
console.log('    std::cout << "=== C++ ELK Layout Results ===" << std::endl;');
console.log('    std::cout << "Graph size: " << root->size.width << " x " << root->size.height << std::endl;');
console.log('    std::cout << "\\nNode Positions:" << std::endl;');
console.log('    for (const auto& child : root->children) {');
console.log('        std::cout << "  " << child->id << ": (" << child->position.x << ", " << child->position.y << ")" << std::endl;');
console.log('    }');

console.log('    return 0;');
console.log('}');
