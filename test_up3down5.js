const fs = require('fs');
const netlistsvg = require('./built/index.js');

const skinData = fs.readFileSync('lib/default.svg', 'utf8');
const netlistData = fs.readFileSync('test/digital/up3down5.json', 'utf8');
const netlist = JSON.parse(netlistData);

console.log('Loading up3down5.json circuit...');

netlistsvg.dumpLayout(skinData, netlist, false, (err, result) => {
    if (err) {
        console.error('Error:', err);
    } else {
        const layout = JSON.parse(result);
        console.log('\n=== up3down5 Graph Structure ===');
        console.log('Total nodes:', layout.children.length);
        console.log('Total edges:', layout.edges.length);
        console.log('Graph dimensions:', layout.width, 'x', layout.height);

        console.log('\nNodes:');
        layout.children.forEach((node, i) => {
            console.log(`  ${i}: ${node.id} at (${node.x.toFixed(1)}, ${node.y.toFixed(1)})`);
        });

        // Save full layout to file
        fs.writeFileSync('/tmp/up3down5_layout.json', JSON.stringify(layout, null, 2));
        console.log('\nFull layout saved to /tmp/up3down5_layout.json');
    }
});
