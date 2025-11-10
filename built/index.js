'use strict';
Object.defineProperty(exports, "__esModule", { value: true });
exports.render = exports.dumpLayout = void 0;
var ELK = require("elkjs");
var onml = require("onml");
var FlatModule_1 = require("./FlatModule");
var Skin_1 = require("./Skin");
var elkGraph_1 = require("./elkGraph");
var drawModule_1 = require("./drawModule");
var elk = new ELK();
function createFlatModule(skinData, yosysNetlist) {
    Skin_1.default.skin = onml.p(skinData);
    var layoutProps = Skin_1.default.getProperties();
    var flatModule = new FlatModule_1.FlatModule(yosysNetlist);
    // this can be skipped if there are no 0's or 1's
    if (layoutProps.constants !== false) {
        flatModule.addConstants();
    }
    // this can be skipped if there are no splits or joins
    if (layoutProps.splitsAndJoins !== false) {
        flatModule.addSplitsJoins();
    }
    flatModule.createWires();
    return flatModule;
}
function dumpLayout(skinData, yosysNetlist, prelayout, done) {
    var flatModule = createFlatModule(skinData, yosysNetlist);
    var kgraph = elkGraph_1.buildElkGraph(flatModule);
    // Log input graph details
    console.log('\n========== ELK INPUT GRAPH ==========');
    console.log('Number of nodes (children):', kgraph.children.length);
    console.log('Number of edges:', kgraph.edges.length);
    console.log('\nInput Graph (JSON):');
    console.log(JSON.stringify(kgraph, null, 2));
    if (prelayout) {
        done(null, JSON.stringify(kgraph, null, 2));
        return;
    }
    var layoutProps = Skin_1.default.getProperties();
    console.log('\nLayout Options:');
    console.log(JSON.stringify(layoutProps.layoutEngine, null, 2));
    var promise = elk.layout(kgraph, { layoutOptions: layoutProps.layoutEngine });
    promise.then(function (graph) {
        console.log('\n========== ELK OUTPUT GRAPH ==========');
        console.log('Graph dimensions:', graph.width, 'x', graph.height);
        console.log('\nNode positions:');
        graph.children.forEach(function (node) {
            console.log("  " + node.id + ": pos=(" + node.x + ", " + node.y + "), size=(" + node.width + ", " + node.height + ")");
            if (node.ports) {
                node.ports.forEach(function (port) {
                    console.log("    port " + port.id + ": pos=(" + port.x + ", " + port.y + ")");
                });
            }
        });
        console.log('\nEdge routing:');
        graph.edges.forEach(function (edge) {
            console.log("  " + edge.id + ": " + edge.source + "." + edge.sourcePort + " -> " + edge.target + "." + edge.targetPort);
            if (edge.sections) {
                edge.sections.forEach(function (section, idx) {
                    console.log("    section " + idx + ": start=(" + section.startPoint.x + ", " + section.startPoint.y + "), end=(" + section.endPoint.x + ", " + section.endPoint.y + ")");
                    if (section.bendPoints) {
                        console.log("      bendPoints:", section.bendPoints);
                    }
                });
            }
        });
        console.log('\nOutput Graph (JSON):');
        console.log(JSON.stringify(graph, null, 2));
        console.log('========================================\n');
        done(null, JSON.stringify(graph, null, 2));
    }).catch(function (reason) {
        throw Error(reason);
    });
}
exports.dumpLayout = dumpLayout;
function render(skinData, yosysNetlist, done, elkData) {
    var flatModule = createFlatModule(skinData, yosysNetlist);
    var kgraph = elkGraph_1.buildElkGraph(flatModule);
    var layoutProps = Skin_1.default.getProperties();
    var promise;
    // if we already have a layout then use it
    if (elkData) {
        promise = new Promise(function (resolve) {
            drawModule_1.default(elkData, flatModule);
            resolve();
        });
    }
    else {
        // otherwise use ELK to generate the layout
        promise = elk.layout(kgraph, { layoutOptions: layoutProps.layoutEngine })
            .then(function (g) { return drawModule_1.default(g, flatModule); })
            // tslint:disable-next-line:no-console
            .catch(function (e) { console.error(e); });
    }
    // support legacy callback style
    if (typeof done === 'function') {
        promise.then(function (output) {
            done(null, output);
            return output;
        }).catch(function (reason) {
            throw Error(reason);
        });
    }
    return promise;
}
exports.render = render;
