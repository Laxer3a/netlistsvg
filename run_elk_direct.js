const ELK = require('elkjs');
const elk = new ELK();

// Simple multi-layer graph matching C++ test
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
    console.log('\n=== JavaScript ELK Layout (multilayer) ===');
    console.log('Graph dimensions:', g.width, 'x', g.height);
    console.log('\nNode positions:');
    g.children.forEach(node => {
      console.log(`  ${node.id}: (${node.x}, ${node.y})`);
    });
  })
  .catch(console.error);
