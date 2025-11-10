const fs = require('fs');
const { execSync } = require('child_process');

// Run C++ test and capture output
console.log('Running C++ ELK layout...');
const cppOutput = execSync('./elk-cpp/build/up3down5_test 2>&1', { encoding: 'utf8' });

// Parse C++ output
const cppLines = cppOutput.split('\n');
let cppWidth = 0, cppHeight = 0;
const cppNodes = [];

for (let i = 0; i < cppLines.length; i++) {
    const line = cppLines[i];
    if (line.includes('Graph size:')) {
        const match = line.match(/Graph size: (\d+) x (\d+)/);
        if (match) {
            cppWidth = parseInt(match[1]);
            cppHeight = parseInt(match[2]);
        }
    } else if (line.match(/^  [^ ].*: \(\d+, \d+\)$/)) {
        const match = line.match(/^  (.*): \((\d+), (\d+)\)$/);
        if (match) {
            const nodeId = match[1];
            const x = parseInt(match[2]);
            const y = parseInt(match[3]);
            // Get node dimensions from input graph
            cppNodes.push({ id: nodeId, x, y, width: 30, height: 25 });
        }
    }
}

console.log(`C++ layout: ${cppWidth} x ${cppHeight}, ${cppNodes.length} nodes`);

// Generate C++ SVG
const cppSvg = [];
cppSvg.push('<?xml version="1.0" encoding="UTF-8"?>');
cppSvg.push(`<svg width="${cppWidth + 40}" height="${cppHeight + 40}" xmlns="http://www.w3.org/2000/svg">`);
cppSvg.push('<rect width="100%" height="100%" fill="white"/>');
cppSvg.push('<g transform="translate(20, 20)">');
cppSvg.push(`<text x="0" y="-5" font-family="Arial" font-size="14" font-weight="bold">C++ ELK Layout (${cppWidth}x${cppHeight})</text>`);

// Draw nodes
for (const node of cppNodes) {
    const color = node.id.startsWith('$') ? '#4CAF50' : '#2196F3';
    cppSvg.push(`<rect x="${node.x}" y="${node.y}" width="${node.width}" height="${node.height}" fill="${color}" stroke="black" stroke-width="1"/>`);

    // Add node label (truncated)
    let label = node.id.length > 15 ? node.id.substring(0, 12) + '...' : node.id;
    cppSvg.push(`<text x="${node.x + node.width/2}" y="${node.y + node.height/2 + 4}" font-family="Arial" font-size="8" text-anchor="middle" fill="white">${label}</text>`);
}

cppSvg.push('</g>');
cppSvg.push('</svg>');

fs.writeFileSync('elk-cpp/test_output/up3down5_cpp.svg', cppSvg.join('\n'));
console.log('✓ Generated elk-cpp/test_output/up3down5_cpp.svg');

// Now generate JavaScript SVG
console.log('\nGenerating JavaScript elkjs layout...');

const netlistsvg = require('./built/index.js');
const ELK = require('elkjs');

const skinData = fs.readFileSync('lib/default.svg', 'utf8');
const netlistData = fs.readFileSync('test/digital/up3down5.json', 'utf8');
const netlist = JSON.parse(netlistData);

netlistsvg.dumpLayout(skinData, netlist, true, (err, prelayoutResult) => {
    if (err) {
        console.error('Error:', err);
        return;
    }

    const graph = JSON.parse(prelayoutResult);
    const elk = new ELK();
    const layoutOptions = {
        'elk.algorithm': 'layered',
        'elk.direction': 'RIGHT',
        'elk.layered.spacing.nodeNodeBetweenLayers': '35.0',
        'elk.spacing.nodeNode': '35.0'
    };

    graph.layoutOptions = layoutOptions;

    elk.layout(graph).then(g => {
        console.log(`JavaScript layout: ${g.width.toFixed(1)} x ${g.height.toFixed(1)}, ${g.children.length} nodes`);

        const jsSvg = [];
        jsSvg.push('<?xml version="1.0" encoding="UTF-8"?>');
        jsSvg.push(`<svg width="${g.width + 40}" height="${g.height + 40}" xmlns="http://www.w3.org/2000/svg">`);
        jsSvg.push('<rect width="100%" height="100%" fill="white"/>');
        jsSvg.push('<g transform="translate(20, 20)">');
        jsSvg.push(`<text x="0" y="-5" font-family="Arial" font-size="14" font-weight="bold">JavaScript elkjs Layout (${g.width.toFixed(0)}x${g.height.toFixed(0)})</text>`);

        // Draw nodes
        for (const node of g.children) {
            const color = node.id.startsWith('$') ? '#FF9800' : '#E91E63';
            jsSvg.push(`<rect x="${node.x}" y="${node.y}" width="${node.width}" height="${node.height}" fill="${color}" stroke="black" stroke-width="1"/>`);

            // Add node label (truncated)
            let label = node.id.length > 15 ? node.id.substring(0, 12) + '...' : node.id;
            jsSvg.push(`<text x="${node.x + node.width/2}" y="${node.y + node.height/2 + 4}" font-family="Arial" font-size="8" text-anchor="middle" fill="white">${label}</text>`);
        }

        jsSvg.push('</g>');
        jsSvg.push('</svg>');

        fs.writeFileSync('elk-cpp/test_output/up3down5_js.svg', jsSvg.join('\n'));
        console.log('✓ Generated elk-cpp/test_output/up3down5_js.svg');

        // Create comparison HTML
        const html = `<!DOCTYPE html>
<html>
<head>
    <title>up3down5 Layout Comparison</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background: #f5f5f5; }
        h1 { color: #333; }
        .container { display: flex; gap: 20px; flex-wrap: wrap; }
        .panel { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        img { border: 1px solid #ddd; }
    </style>
</head>
<body>
    <h1>up3down5 Circuit Layout Comparison</h1>
    <p><strong>C++:</strong> ${cppWidth}x${cppHeight} | <strong>JavaScript:</strong> ${g.width.toFixed(0)}x${g.height.toFixed(0)}</p>
    <div class="container">
        <div class="panel">
            <h2>C++ ELK Implementation</h2>
            <img src="up3down5_cpp.svg" alt="C++ Layout"/>
        </div>
        <div class="panel">
            <h2>JavaScript elkjs</h2>
            <img src="up3down5_js.svg" alt="JavaScript Layout"/>
        </div>
    </div>
</body>
</html>`;

        fs.writeFileSync('elk-cpp/test_output/up3down5_comparison.html', html);
        console.log('✓ Generated elk-cpp/test_output/up3down5_comparison.html');
        console.log('\nOpen elk-cpp/test_output/up3down5_comparison.html in a browser to compare');
    }).catch(console.error);
});
