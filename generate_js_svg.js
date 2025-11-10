const ELK = require('elkjs');
const fs = require('fs');
const elk = new ELK();

const graph = {
  id: "root",
  layoutOptions: {
    'elk.algorithm': 'layered',
    'elk.direction': 'RIGHT',
    'elk.spacing.nodeNode': '35.0',
    'elk.layered.spacing.nodeNodeBetweenLayers': '80.0'
  },
  children: [
    { id: 's0', width: 30, height: 25 },
    { id: 's1', width: 30, height: 25 },
    { id: 'p0', width: 30, height: 25 },
    { id: 'p1', width: 30, height: 25 },
    { id: 'p2', width: 30, height: 25 },
    { id: 'p3', width: 30, height: 25 },
    { id: 't0', width: 30, height: 25 },
    { id: 't1', width: 30, height: 25 }
  ],
  edges: [
    { id: 'e0', sources: ['s0'], targets: ['p0'] },
    { id: 'e1', sources: ['s0'], targets: ['p1'] },
    { id: 'e2', sources: ['s1'], targets: ['p2'] },
    { id: 'e3', sources: ['s1'], targets: ['p3'] },
    { id: 'e4', sources: ['p0'], targets: ['t0'] },
    { id: 'e5', sources: ['p1'], targets: ['t0'] },
    { id: 'e6', sources: ['p2'], targets: ['t1'] },
    { id: 'e7', sources: ['p3'], targets: ['t1'] }
  ]
};

elk.layout(graph)
  .then(g => {
    console.log('JavaScript elkjs layout complete');
    console.log('Graph dimensions:', g.width, 'x', g.height);

    // Calculate bounds
    let maxX = 0, maxY = 0;
    g.children.forEach(node => {
      maxX = Math.max(maxX, node.x + node.width);
      maxY = Math.max(maxY, node.y + node.height);
    });

    const width = maxX + 24;
    const height = maxY + 24;

    // Generate SVG
    let svg = '<?xml version="1.0" encoding="UTF-8"?>\n';
    svg += `<svg xmlns="http://www.w3.org/2000/svg" width="${width}" height="${height}">\n`;
    svg += '  <title>JavaScript elkjs Layout</title>\n';
    svg += '  <defs>\n';
    svg += '    <marker id="arrow" markerWidth="10" markerHeight="10" refX="9" refY="3" orient="auto" markerUnits="strokeWidth">\n';
    svg += '      <path d="M0,0 L0,6 L9,3 z" fill="#666"/>\n';
    svg += '    </marker>\n';
    svg += '  </defs>\n';
    svg += '  <rect width="100%" height="100%" fill="white"/>\n';

    // Draw edges
    svg += '  <g id="edges" stroke="#666" stroke-width="1" fill="none" marker-end="url(#arrow)">\n';
    g.edges.forEach(edge => {
      // Find source and target nodes
      const sourceNode = g.children.find(n => n.id === edge.sources[0]);
      const targetNode = g.children.find(n => n.id === edge.targets[0]);

      if (sourceNode && targetNode) {
        const x1 = sourceNode.x + sourceNode.width;
        const y1 = sourceNode.y + sourceNode.height / 2;
        const x2 = targetNode.x;
        const y2 = targetNode.y + targetNode.height / 2;

        svg += `    <line x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}"/>\n`;
      }
    });
    svg += '  </g>\n';

    // Draw nodes
    svg += '  <g id="nodes">\n';
    g.children.forEach(node => {
      svg += `    <g id="${node.id}">\n`;
      svg += `      <rect x="${node.x}" y="${node.y}" width="${node.width}" height="${node.height}" `;
      svg += 'fill="#fff3e0" stroke="#f57c00" stroke-width="2" rx="2"/>\n';
      svg += `      <text x="${node.x + node.width/2}" y="${node.y + node.height/2 + 4}" `;
      svg += 'text-anchor="middle" font-family="monospace" font-size="10" fill="#000">';
      svg += `${node.id}</text>\n`;
      svg += '    </g>\n';
    });
    svg += '  </g>\n';

    // Add title
    svg += '  <text x="10" y="15" font-family="Arial" font-size="14" font-weight="bold" fill="#000">';
    svg += 'JavaScript elkjs Layout</text>\n';

    svg += '</svg>\n';

    fs.writeFileSync('/tmp/js_layout.svg', svg);
    console.log('Generated /tmp/js_layout.svg');

    console.log('\nNode positions:');
    g.children.forEach(node => {
      console.log(`  ${node.id}: (${node.x.toFixed(1)}, ${node.y.toFixed(1)})`);
    });
  })
  .catch(console.error);
