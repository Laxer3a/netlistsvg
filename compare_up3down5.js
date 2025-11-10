const fs = require('fs');
const netlistsvg = require('./built/index.js');
const ELK = require('elkjs');

const skinData = fs.readFileSync('lib/default.svg', 'utf8');
const netlistData = fs.readFileSync('test/digital/up3down5.json', 'utf8');
const netlist = JSON.parse(netlistData);

console.log('=== Testing up3down5 with JavaScript elkjs ===\n');

// Get the elkjs graph that netlistsvg creates
netlistsvg.dumpLayout(skinData, netlist, true, (err, prelayoutResult) => {
    if (err) {
        console.error('Error:', err);
        return;
    }

    const graph = JSON.parse(prelayoutResult);
    console.log('Graph created by netlistsvg:');
    console.log('  Nodes:', graph.children.length);
    console.log('  Edges:', graph.edges.length);

    // Now run elkjs directly on it
    const elk = new ELK();
    const layoutOptions = {
        'elk.algorithm': 'layered',
        'elk.direction': 'RIGHT',
        'elk.layered.spacing.nodeNodeBetweenLayers': '35.0',
        'elk.spacing.nodeNode': '35.0'
    };

    graph.layoutOptions = layoutOptions;

    elk.layout(graph)
        .then(g => {
            console.log('\n=== JavaScript elkjs Layout Results ===');
            console.log('Dimensions:', g.width, 'x', g.height);
            console.log('\nNode Positions (first 20):');
            g.children.slice(0, 20).forEach((node, i) => {
                console.log(`  ${node.id}: (${node.x.toFixed(1)}, ${node.y.toFixed(1)})`);
            });

            // Save to file for C++ comparison
            fs.writeFileSync('/tmp/up3down5_js_positions.txt',
                g.children.map(n => `${n.id}: (${n.x.toFixed(1)}, ${n.y.toFixed(1)})`).join('\n')
            );

            // Also save the input graph for C++
            fs.writeFileSync('/tmp/up3down5_elk_graph.json', JSON.stringify(graph, null, 2));

            console.log('\n✓ Saved JavaScript positions to /tmp/up3down5_js_positions.txt');
            console.log('✓ Saved ELK graph to /tmp/up3down5_elk_graph.json');
            console.log('\nNow run C++ implementation on the same graph!');
        })
        .catch(console.error);
});
