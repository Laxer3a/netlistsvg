/**
 * Copyright (c) 2017, 2020 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.json;

import com.google.common.base.Objects;
import com.google.common.collect.BiMap;
import com.google.common.collect.HashBiMap;
import com.google.common.collect.HashMultimap;
import com.google.common.collect.Maps;
import com.google.common.collect.Multimap;
import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import java.util.Arrays;
import java.util.Collection;
import java.util.Map;
import java.util.Set;
import java.util.function.Consumer;
import org.eclipse.elk.core.data.LayoutMetaDataService;
import org.eclipse.elk.core.data.LayoutOptionData;
import org.eclipse.elk.core.math.KVector;
import org.eclipse.elk.core.math.KVectorChain;
import org.eclipse.elk.core.options.CoreOptions;
import org.eclipse.elk.core.util.IndividualSpacings;
import org.eclipse.elk.graph.EMapPropertyHolder;
import org.eclipse.elk.graph.ElkBendPoint;
import org.eclipse.elk.graph.ElkConnectableShape;
import org.eclipse.elk.graph.ElkEdge;
import org.eclipse.elk.graph.ElkEdgeSection;
import org.eclipse.elk.graph.ElkGraphElement;
import org.eclipse.elk.graph.ElkLabel;
import org.eclipse.elk.graph.ElkNode;
import org.eclipse.elk.graph.ElkPort;
import org.eclipse.elk.graph.ElkShape;
import org.eclipse.elk.graph.properties.IPropertyHolder;
import org.eclipse.elk.graph.util.ElkGraphUtil;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.xtext.xbase.lib.ExclusiveRange;
import org.eclipse.xtext.xbase.lib.Extension;
import org.eclipse.xtext.xbase.lib.IterableExtensions;
import org.eclipse.xtext.xbase.lib.IteratorExtensions;
import org.eclipse.xtext.xbase.lib.ObjectExtensions;
import org.eclipse.xtext.xbase.lib.Procedures.Procedure1;
import org.eclipse.xtext.xbase.lib.Procedures.Procedure2;

/**
 * Importer from json to elk graph. Internally it maintains a mapping that can be used to
 * transfer any computed layout information to the original json.
 * 
 * <h3>Implementation Hints</h3>
 * The implementation of the importer is kept free of any explicit json library. This is possible thanks
 * to xtend's extensions methods. Wherever possible type-inference is used, if not the Object type is used.
 * The library-dependent code can be found in the {@link JsonAdapter} class.
 * 
 * To get an id that must be specified and to preserve the id's type (string or int) use 'getId'
 * (throws an exeption otherwise). If the result may be null and may always be a string
 * (e.g. when assembling the text for an exception) use 'getIdSave'.
 */
@SuppressWarnings("all")
public final class JsonImporter {
  @Extension
  private JsonAdapter _jsonAdapter = new JsonAdapter();

  /**
   * Id -> ElkGraph element maps
   * Id can be string or integer, thus {@link Object} is used.
   */
  private final BiMap<Object, ElkNode> nodeIdMap = HashBiMap.<Object, ElkNode>create();

  private final BiMap<Object, ElkPort> portIdMap = HashBiMap.<Object, ElkPort>create();

  private final Map<Object, ElkEdge> edgeIdMap = Maps.<Object, ElkEdge>newHashMap();

  private final BiMap<Object, ElkEdgeSection> edgeSectionIdMap = HashBiMap.<Object, ElkEdgeSection>create();

  /**
   * ElkGraph element -> Json element maps
   */
  private final BiMap<ElkNode, Object> nodeJsonMap = HashBiMap.<ElkNode, Object>create();

  private final Map<ElkPort, Object> portJsonMap = Maps.<ElkPort, Object>newHashMap();

  private final Map<ElkEdge, Object> edgeJsonMap = Maps.<ElkEdge, Object>newHashMap();

  private final Map<ElkEdgeSection, Object> edgeSectionJsonMap = Maps.<ElkEdgeSection, Object>newHashMap();

  private final Map<ElkLabel, Object> labelJsonMap = Maps.<ElkLabel, Object>newHashMap();

  private Object inputModel;

  /**
   * Main entry point for the json to ELK graph transformation. Runs through all elements
   * of the graph (nodes, ports, edges, edge sections) and creates correlating ELK graph elements.
   */
  public ElkNode transform(final Object graph) {
    this.inputModel = graph;
    this.clearMaps();
    final ElkNode root = this.transformNode(graph, null);
    this.transformEdges(graph);
    return root;
  }

  public Object getInputModel() {
    return this.inputModel;
  }

  private void clearMaps() {
    this.nodeIdMap.clear();
    this.portIdMap.clear();
    this.edgeIdMap.clear();
    this.edgeSectionIdMap.clear();
    this.nodeJsonMap.clear();
    this.portJsonMap.clear();
    this.edgeJsonMap.clear();
    this.edgeSectionJsonMap.clear();
  }

  private JsonArray transformChildNodes(final Object jsonNodeA, final ElkNode parent) {
    JsonArray _xblockexpression = null;
    {
      final JsonObject jsonNode = this._jsonAdapter.toJsonObject(jsonNodeA);
      JsonArray _optJSONArray = this._jsonAdapter.optJSONArray(jsonNode, "children");
      final Procedure1<JsonArray> _function = new Procedure1<JsonArray>() {
        @Override
        public void apply(final JsonArray children) {
          if ((children != null)) {
            int _sizeJsonArr = JsonImporter.this._jsonAdapter.sizeJsonArr(children);
            ExclusiveRange _doubleDotLessThan = new ExclusiveRange(0, _sizeJsonArr, true);
            for (final Integer i : _doubleDotLessThan) {
              JsonObject _optJSONObject = JsonImporter.this._jsonAdapter.optJSONObject(children, (i).intValue());
              if (_optJSONObject!=null) {
                JsonImporter.this.transformNode(_optJSONObject, parent);
              }
            }
          }
        }
      };
      _xblockexpression = ObjectExtensions.<JsonArray>operator_doubleArrow(_optJSONArray, _function);
    }
    return _xblockexpression;
  }

  private ElkNode transformNode(final Object jsonNode, final ElkNode parent) {
    final ElkNode node = this.register(ElkGraphUtil.createNode(parent), jsonNode);
    node.setIdentifier(this._jsonAdapter.getIdSave(this._jsonAdapter.toJsonObject(jsonNode)));
    this.transformProperties(jsonNode, node);
    this.transformIndividualSpacings(jsonNode, node);
    this.transformShapeLayout(jsonNode, node);
    this.transformPorts(jsonNode, node);
    this.transformLabels(jsonNode, node);
    this.transformChildNodes(jsonNode, node);
    return node;
  }

  private void transformEdges(final Object jsonObjA) {
    final JsonObject jsonObj = this._jsonAdapter.toJsonObject(jsonObjA);
    final ElkNode node = this.nodeJsonMap.inverse().get(jsonObj);
    if ((node == null)) {
      String _idSave = this._jsonAdapter.getIdSave(jsonObj);
      String _plus = ("Unable to find elk node for json object \'" + _idSave);
      String _plus_1 = (_plus + "\' Panic!");
      throw this._jsonAdapter.formatError(_plus_1);
    }
    JsonArray _optJSONArray = this._jsonAdapter.optJSONArray(jsonObj, "edges");
    final Procedure1<JsonArray> _function = new Procedure1<JsonArray>() {
      @Override
      public void apply(final JsonArray edges) {
        if ((edges != null)) {
          int _sizeJsonArr = JsonImporter.this._jsonAdapter.sizeJsonArr(edges);
          ExclusiveRange _doubleDotLessThan = new ExclusiveRange(0, _sizeJsonArr, true);
          for (final Integer i : _doubleDotLessThan) {
            {
              final JsonObject edge = JsonImporter.this._jsonAdapter.optJSONObject(edges, (i).intValue());
              if ((JsonImporter.this._jsonAdapter.hasJsonObj(edge, "sources") || JsonImporter.this._jsonAdapter.hasJsonObj(edge, "targets"))) {
                JsonImporter.this.transformEdge(edge, node);
              } else {
                JsonImporter.this.transformPrimitiveEdge(edge, node);
              }
              ElkGraphUtil.updateContainment(JsonImporter.this.edgeIdMap.get(JsonImporter.this._jsonAdapter.getId(edge)));
            }
          }
        }
      }
    };
    ObjectExtensions.<JsonArray>operator_doubleArrow(_optJSONArray, _function);
    JsonArray _optJSONArray_1 = this._jsonAdapter.optJSONArray(jsonObj, "children");
    final Procedure1<JsonArray> _function_1 = new Procedure1<JsonArray>() {
      @Override
      public void apply(final JsonArray children) {
        if ((children != null)) {
          int _sizeJsonArr = JsonImporter.this._jsonAdapter.sizeJsonArr(children);
          ExclusiveRange _doubleDotLessThan = new ExclusiveRange(0, _sizeJsonArr, true);
          for (final Integer i : _doubleDotLessThan) {
            JsonObject _optJSONObject = JsonImporter.this._jsonAdapter.optJSONObject(children, (i).intValue());
            if (_optJSONObject!=null) {
              JsonImporter.this.transformEdges(_optJSONObject);
            }
          }
        }
      }
    };
    ObjectExtensions.<JsonArray>operator_doubleArrow(_optJSONArray_1, _function_1);
  }

  private JsonArray transformPrimitiveEdge(final Object jsonObjA, final ElkNode parent) {
    JsonArray _xblockexpression = null;
    {
      final JsonObject jsonObj = this._jsonAdapter.toJsonObject(jsonObjA);
      final ElkEdge edge = this.register(ElkGraphUtil.createEdge(parent), jsonObj);
      edge.setIdentifier(this._jsonAdapter.getIdSave(jsonObj));
      final ElkNode srcNode = this.nodeIdMap.get(this._jsonAdapter.asId(this._jsonAdapter.getJsonObj(jsonObj, "source")));
      JsonElement _jsonObj = this._jsonAdapter.getJsonObj(jsonObj, "sourcePort");
      Object _asId = null;
      if (_jsonObj!=null) {
        _asId=this._jsonAdapter.asId(_jsonObj);
      }
      final ElkPort srcPort = this.portIdMap.get(_asId);
      if ((srcNode == null)) {
        Object _id = this._jsonAdapter.getId(jsonObj);
        String _plus = ("An edge must have a source node (edge id: \'" + _id);
        String _plus_1 = (_plus + "\').");
        throw this._jsonAdapter.formatError(_plus_1);
      }
      if (((srcPort != null) && (!Objects.equal(srcPort.getParent(), srcNode)))) {
        String _idSave = this._jsonAdapter.getIdSave(jsonObj);
        String _plus_2 = ("The source port of an edge must be a port of the edge\'s source node (edge id: \'" + _idSave);
        String _plus_3 = (_plus_2 + "\').");
        throw this._jsonAdapter.formatError(_plus_3);
      }
      EList<ElkConnectableShape> _sources = edge.getSources();
      ElkConnectableShape _elvis = null;
      if (srcPort != null) {
        _elvis = srcPort;
      } else {
        _elvis = srcNode;
      }
      _sources.add(_elvis);
      final ElkNode tgtNode = this.nodeIdMap.get(this._jsonAdapter.asId(this._jsonAdapter.getJsonObj(jsonObj, "target")));
      JsonElement _jsonObj_1 = this._jsonAdapter.getJsonObj(jsonObj, "targetPort");
      Object _asId_1 = null;
      if (_jsonObj_1!=null) {
        _asId_1=this._jsonAdapter.asId(_jsonObj_1);
      }
      final ElkPort tgtPort = this.portIdMap.get(_asId_1);
      if ((tgtNode == null)) {
        Object _id_1 = this._jsonAdapter.getId(jsonObj);
        String _plus_4 = ("An edge must have a target node (edge id: \'" + _id_1);
        String _plus_5 = (_plus_4 + "\').");
        throw this._jsonAdapter.formatError(_plus_5);
      }
      if (((tgtPort != null) && (!Objects.equal(tgtPort.getParent(), tgtNode)))) {
        String _idSave_1 = this._jsonAdapter.getIdSave(jsonObj);
        String _plus_6 = ("The target port of an edge must be a port of the edge\'s target node (edge id: \'" + _idSave_1);
        String _plus_7 = (_plus_6 + "\').");
        throw this._jsonAdapter.formatError(_plus_7);
      }
      EList<ElkConnectableShape> _targets = edge.getTargets();
      ElkConnectableShape _elvis_1 = null;
      if (tgtPort != null) {
        _elvis_1 = tgtPort;
      } else {
        _elvis_1 = tgtNode;
      }
      _targets.add(_elvis_1);
      if ((edge.getSources().isEmpty() || edge.getTargets().isEmpty())) {
        String _idSave_2 = this._jsonAdapter.getIdSave(jsonObj);
        String _plus_8 = ("An edge must have at least one source and one target (edge id: \'" + _idSave_2);
        String _plus_9 = (_plus_8 + "\').");
        throw this._jsonAdapter.formatError(_plus_9);
      }
      this.transformProperties(jsonObj, edge);
      this.transformPrimitiveEdgeLayout(jsonObj, edge);
      _xblockexpression = this.transformLabels(jsonObj, edge);
    }
    return _xblockexpression;
  }

  private JsonArray transformPrimitiveEdgeLayout(final Object jsonObjA, final ElkEdge edge) {
    JsonArray _xblockexpression = null;
    {
      final JsonObject jsonObj = this._jsonAdapter.toJsonObject(jsonObjA);
      JsonArray _xifexpression = null;
      if (((this._jsonAdapter.hasJsonObj(jsonObj, "sourcePoint") || this._jsonAdapter.hasJsonObj(jsonObj, "targetPoint")) || this._jsonAdapter.hasJsonObj(jsonObj, "bendPoints"))) {
        JsonArray _xblockexpression_1 = null;
        {
          final ElkEdgeSection section = ElkGraphUtil.createEdgeSection(edge);
          JsonObject _optJSONObject = this._jsonAdapter.optJSONObject(jsonObj, "sourcePoint");
          final Procedure1<JsonObject> _function = new Procedure1<JsonObject>() {
            @Override
            public void apply(final JsonObject srcPnt) {
              if ((srcPnt != null)) {
                Double _optDouble = JsonImporter.this._jsonAdapter.optDouble(srcPnt, "x");
                final Procedure1<Double> _function = new Procedure1<Double>() {
                  @Override
                  public void apply(final Double it) {
                    section.setStartX((it).doubleValue());
                  }
                };
                ObjectExtensions.<Double>operator_doubleArrow(_optDouble, _function);
                Double _optDouble_1 = JsonImporter.this._jsonAdapter.optDouble(srcPnt, "y");
                final Procedure1<Double> _function_1 = new Procedure1<Double>() {
                  @Override
                  public void apply(final Double it) {
                    section.setStartY((it).doubleValue());
                  }
                };
                ObjectExtensions.<Double>operator_doubleArrow(_optDouble_1, _function_1);
              }
            }
          };
          ObjectExtensions.<JsonObject>operator_doubleArrow(_optJSONObject, _function);
          JsonObject _optJSONObject_1 = this._jsonAdapter.optJSONObject(jsonObj, "targetPoint");
          final Procedure1<JsonObject> _function_1 = new Procedure1<JsonObject>() {
            @Override
            public void apply(final JsonObject tgtPnt) {
              if ((tgtPnt != null)) {
                Double _optDouble = JsonImporter.this._jsonAdapter.optDouble(tgtPnt, "x");
                final Procedure1<Double> _function = new Procedure1<Double>() {
                  @Override
                  public void apply(final Double it) {
                    section.setEndX((it).doubleValue());
                  }
                };
                ObjectExtensions.<Double>operator_doubleArrow(_optDouble, _function);
                Double _optDouble_1 = JsonImporter.this._jsonAdapter.optDouble(tgtPnt, "y");
                final Procedure1<Double> _function_1 = new Procedure1<Double>() {
                  @Override
                  public void apply(final Double it) {
                    section.setEndY((it).doubleValue());
                  }
                };
                ObjectExtensions.<Double>operator_doubleArrow(_optDouble_1, _function_1);
              }
            }
          };
          ObjectExtensions.<JsonObject>operator_doubleArrow(_optJSONObject_1, _function_1);
          JsonArray _optJSONArray = this._jsonAdapter.optJSONArray(jsonObj, "bendPoints");
          final Procedure1<JsonArray> _function_2 = new Procedure1<JsonArray>() {
            @Override
            public void apply(final JsonArray bendPoints) {
              if ((bendPoints != null)) {
                int _sizeJsonArr = JsonImporter.this._jsonAdapter.sizeJsonArr(bendPoints);
                ExclusiveRange _doubleDotLessThan = new ExclusiveRange(0, _sizeJsonArr, true);
                for (final Integer i : _doubleDotLessThan) {
                  JsonObject _optJSONObject = JsonImporter.this._jsonAdapter.optJSONObject(bendPoints, (i).intValue());
                  final Procedure1<JsonObject> _function = new Procedure1<JsonObject>() {
                    @Override
                    public void apply(final JsonObject bendPoint) {
                      ElkGraphUtil.createBendPoint(section, (JsonImporter.this._jsonAdapter.optDouble(bendPoint, "x")).doubleValue(), (JsonImporter.this._jsonAdapter.optDouble(bendPoint, "y")).doubleValue());
                    }
                  };
                  ObjectExtensions.<JsonObject>operator_doubleArrow(_optJSONObject, _function);
                }
              }
            }
          };
          _xblockexpression_1 = ObjectExtensions.<JsonArray>operator_doubleArrow(_optJSONArray, _function_2);
        }
        _xifexpression = _xblockexpression_1;
      }
      _xblockexpression = _xifexpression;
    }
    return _xblockexpression;
  }

  private JsonArray transformEdge(final Object jsonObjA, final ElkNode parent) {
    JsonArray _xblockexpression = null;
    {
      final JsonObject jsonObj = this._jsonAdapter.toJsonObject(jsonObjA);
      final ElkEdge edge = this.register(ElkGraphUtil.createEdge(parent), jsonObj);
      edge.setIdentifier(this._jsonAdapter.getIdSave(jsonObj));
      JsonArray _optJSONArray = this._jsonAdapter.optJSONArray(jsonObj, "sources");
      final Procedure1<JsonArray> _function = new Procedure1<JsonArray>() {
        @Override
        public void apply(final JsonArray sources) {
          if ((sources != null)) {
            int _sizeJsonArr = JsonImporter.this._jsonAdapter.sizeJsonArr(sources);
            ExclusiveRange _doubleDotLessThan = new ExclusiveRange(0, _sizeJsonArr, true);
            for (final Integer i : _doubleDotLessThan) {
              {
                final ElkConnectableShape sourceElement = JsonImporter.this.shapeById(JsonImporter.this._jsonAdapter.asId(JsonImporter.this._jsonAdapter.getJsonArr(sources, (i).intValue())));
                if ((sourceElement != null)) {
                  EList<ElkConnectableShape> _sources = edge.getSources();
                  _sources.add(sourceElement);
                }
              }
            }
          }
        }
      };
      ObjectExtensions.<JsonArray>operator_doubleArrow(_optJSONArray, _function);
      JsonArray _optJSONArray_1 = this._jsonAdapter.optJSONArray(jsonObj, "targets");
      final Procedure1<JsonArray> _function_1 = new Procedure1<JsonArray>() {
        @Override
        public void apply(final JsonArray targets) {
          if ((targets != null)) {
            int _sizeJsonArr = JsonImporter.this._jsonAdapter.sizeJsonArr(targets);
            ExclusiveRange _doubleDotLessThan = new ExclusiveRange(0, _sizeJsonArr, true);
            for (final Integer i : _doubleDotLessThan) {
              {
                final ElkConnectableShape targetElement = JsonImporter.this.shapeById(JsonImporter.this._jsonAdapter.asId(JsonImporter.this._jsonAdapter.getJsonArr(targets, (i).intValue())));
                if ((targetElement != null)) {
                  EList<ElkConnectableShape> _targets = edge.getTargets();
                  _targets.add(targetElement);
                }
              }
            }
          }
        }
      };
      ObjectExtensions.<JsonArray>operator_doubleArrow(_optJSONArray_1, _function_1);
      if ((edge.getSources().isEmpty() || edge.getTargets().isEmpty())) {
        String _idSave = this._jsonAdapter.getIdSave(jsonObj);
        String _plus = ("An edge must have at least one source and one target (edge id: \'" + _idSave);
        String _plus_1 = (_plus + "\').");
        throw this._jsonAdapter.formatError(_plus_1);
      }
      this.transformProperties(jsonObj, edge);
      this.transformEdgeSections(jsonObj, edge);
      _xblockexpression = this.transformLabels(jsonObj, edge);
    }
    return _xblockexpression;
  }

  private void transformEdgeSections(final Object jsonObjA, final ElkEdge edge) {
    final JsonObject jsonObj = this._jsonAdapter.toJsonObject(jsonObjA);
    final Multimap<ElkEdgeSection, Object> incomingSectionIdentifiers = HashMultimap.<ElkEdgeSection, Object>create();
    final Multimap<ElkEdgeSection, Object> outgoingSectionIdentifiers = HashMultimap.<ElkEdgeSection, Object>create();
    JsonArray _optJSONArray = this._jsonAdapter.optJSONArray(jsonObj, "sections");
    final Procedure1<JsonArray> _function = new Procedure1<JsonArray>() {
      @Override
      public void apply(final JsonArray sections) {
        if ((sections != null)) {
          int _sizeJsonArr = JsonImporter.this._jsonAdapter.sizeJsonArr(sections);
          ExclusiveRange _doubleDotLessThan = new ExclusiveRange(0, _sizeJsonArr, true);
          for (final Integer i : _doubleDotLessThan) {
            JsonObject _optJSONObject = JsonImporter.this._jsonAdapter.optJSONObject(sections, (i).intValue());
            final Procedure1<JsonObject> _function = new Procedure1<JsonObject>() {
              @Override
              public void apply(final JsonObject jsonSection) {
                final ElkEdgeSection elkSection = JsonImporter.this.register(ElkGraphUtil.createEdgeSection(edge), jsonSection);
                elkSection.setIdentifier(JsonImporter.this._jsonAdapter.getIdSave(jsonSection));
                JsonImporter.this.fillEdgeSectionCoordinates(jsonSection, elkSection);
                String _optString = JsonImporter.this._jsonAdapter.optString(jsonSection, "incomingShape");
                final Procedure1<String> _function = new Procedure1<String>() {
                  @Override
                  public void apply(final String jsonShapeId) {
                    if ((jsonShapeId != null)) {
                      elkSection.setIncomingShape(JsonImporter.this.shapeById(jsonShapeId));
                    }
                  }
                };
                ObjectExtensions.<String>operator_doubleArrow(_optString, _function);
                String _optString_1 = JsonImporter.this._jsonAdapter.optString(jsonSection, "outgoingShape");
                final Procedure1<String> _function_1 = new Procedure1<String>() {
                  @Override
                  public void apply(final String jsonShapeId) {
                    if ((jsonShapeId != null)) {
                      elkSection.setOutgoingShape(JsonImporter.this.shapeById(jsonShapeId));
                    }
                  }
                };
                ObjectExtensions.<String>operator_doubleArrow(_optString_1, _function_1);
                JsonArray _optJSONArray = JsonImporter.this._jsonAdapter.optJSONArray(jsonSection, "incomingSections");
                final Procedure1<JsonArray> _function_2 = new Procedure1<JsonArray>() {
                  @Override
                  public void apply(final JsonArray jsonSectionIds) {
                    if ((jsonSectionIds != null)) {
                      int _sizeJsonArr = JsonImporter.this._jsonAdapter.sizeJsonArr(jsonSectionIds);
                      ExclusiveRange _doubleDotLessThan = new ExclusiveRange(0, _sizeJsonArr, true);
                      for (final Integer j : _doubleDotLessThan) {
                        incomingSectionIdentifiers.put(elkSection, JsonImporter.this._jsonAdapter.asId(JsonImporter.this._jsonAdapter.getJsonArr(jsonSectionIds, (j).intValue())));
                      }
                    }
                  }
                };
                ObjectExtensions.<JsonArray>operator_doubleArrow(_optJSONArray, _function_2);
                JsonArray _optJSONArray_1 = JsonImporter.this._jsonAdapter.optJSONArray(jsonSection, "outgoingSections");
                final Procedure1<JsonArray> _function_3 = new Procedure1<JsonArray>() {
                  @Override
                  public void apply(final JsonArray jsonSectionIds) {
                    if ((jsonSectionIds != null)) {
                      int _sizeJsonArr = JsonImporter.this._jsonAdapter.sizeJsonArr(jsonSectionIds);
                      ExclusiveRange _doubleDotLessThan = new ExclusiveRange(0, _sizeJsonArr, true);
                      for (final Integer j : _doubleDotLessThan) {
                        outgoingSectionIdentifiers.put(elkSection, JsonImporter.this._jsonAdapter.asId(JsonImporter.this._jsonAdapter.getJsonArr(jsonSectionIds, (j).intValue())));
                      }
                    }
                  }
                };
                ObjectExtensions.<JsonArray>operator_doubleArrow(_optJSONArray_1, _function_3);
              }
            };
            ObjectExtensions.<JsonObject>operator_doubleArrow(_optJSONObject, _function);
          }
        }
      }
    };
    ObjectExtensions.<JsonArray>operator_doubleArrow(_optJSONArray, _function);
    Set<ElkEdgeSection> _keySet = incomingSectionIdentifiers.keySet();
    for (final ElkEdgeSection section : _keySet) {
      Collection<Object> _get = incomingSectionIdentifiers.get(section);
      for (final Object id : _get) {
        {
          final ElkEdgeSection referencedSection = this.edgeSectionIdMap.get(id);
          if ((referencedSection != null)) {
            EList<ElkEdgeSection> _incomingSections = section.getIncomingSections();
            _incomingSections.add(referencedSection);
          } else {
            String _idSave = this._jsonAdapter.getIdSave(jsonObj);
            String _plus = ((("Referenced edge section does not exist: " + id) + " (edge id: \'") + _idSave);
            String _plus_1 = (_plus + "\').");
            throw this._jsonAdapter.formatError(_plus_1);
          }
        }
      }
    }
    Set<ElkEdgeSection> _keySet_1 = outgoingSectionIdentifiers.keySet();
    for (final ElkEdgeSection section_1 : _keySet_1) {
      Collection<Object> _get_1 = outgoingSectionIdentifiers.get(section_1);
      for (final Object id_1 : _get_1) {
        {
          final ElkEdgeSection referencedSection = this.edgeSectionIdMap.get(id_1);
          if ((referencedSection != null)) {
            EList<ElkEdgeSection> _outgoingSections = section_1.getOutgoingSections();
            _outgoingSections.add(referencedSection);
          } else {
            String _idSave = this._jsonAdapter.getIdSave(jsonObj);
            String _plus = ((("Referenced edge section does not exist: " + id_1) + " (edge id: \'") + _idSave);
            String _plus_1 = (_plus + "\').");
            throw this._jsonAdapter.formatError(_plus_1);
          }
        }
      }
    }
    if (((edge.isConnected() && (!edge.isHyperedge())) && (edge.getSections().size() == 1))) {
      final ElkEdgeSection section_2 = edge.getSections().get(0);
      if (((section_2.getIncomingShape() == null) && (section_2.getOutgoingShape() == null))) {
        section_2.setIncomingShape(edge.getSources().get(0));
        section_2.setOutgoingShape(edge.getTargets().get(0));
      }
    }
  }

  private JsonArray fillEdgeSectionCoordinates(final Object jsonObjA, final ElkEdgeSection section) {
    JsonArray _xblockexpression = null;
    {
      final JsonObject jsonObj = this._jsonAdapter.toJsonObject(jsonObjA);
      JsonObject _optJSONObject = this._jsonAdapter.optJSONObject(jsonObj, "startPoint");
      final Procedure1<JsonObject> _function = new Procedure1<JsonObject>() {
        @Override
        public void apply(final JsonObject startPoint) {
          if ((startPoint != null)) {
            Double _optDouble = JsonImporter.this._jsonAdapter.optDouble(startPoint, "x");
            final Procedure1<Double> _function = new Procedure1<Double>() {
              @Override
              public void apply(final Double it) {
                section.setStartX((it).doubleValue());
              }
            };
            ObjectExtensions.<Double>operator_doubleArrow(_optDouble, _function);
            Double _optDouble_1 = JsonImporter.this._jsonAdapter.optDouble(startPoint, "y");
            final Procedure1<Double> _function_1 = new Procedure1<Double>() {
              @Override
              public void apply(final Double it) {
                section.setStartY((it).doubleValue());
              }
            };
            ObjectExtensions.<Double>operator_doubleArrow(_optDouble_1, _function_1);
          } else {
            throw JsonImporter.this._jsonAdapter.formatError("All edge sections need a start point.");
          }
        }
      };
      ObjectExtensions.<JsonObject>operator_doubleArrow(_optJSONObject, _function);
      JsonObject _optJSONObject_1 = this._jsonAdapter.optJSONObject(jsonObj, "endPoint");
      final Procedure1<JsonObject> _function_1 = new Procedure1<JsonObject>() {
        @Override
        public void apply(final JsonObject endPoint) {
          if ((endPoint != null)) {
            Double _optDouble = JsonImporter.this._jsonAdapter.optDouble(endPoint, "x");
            final Procedure1<Double> _function = new Procedure1<Double>() {
              @Override
              public void apply(final Double it) {
                section.setEndX((it).doubleValue());
              }
            };
            ObjectExtensions.<Double>operator_doubleArrow(_optDouble, _function);
            Double _optDouble_1 = JsonImporter.this._jsonAdapter.optDouble(endPoint, "y");
            final Procedure1<Double> _function_1 = new Procedure1<Double>() {
              @Override
              public void apply(final Double it) {
                section.setEndY((it).doubleValue());
              }
            };
            ObjectExtensions.<Double>operator_doubleArrow(_optDouble_1, _function_1);
          } else {
            throw JsonImporter.this._jsonAdapter.formatError("All edge sections need an end point.");
          }
        }
      };
      ObjectExtensions.<JsonObject>operator_doubleArrow(_optJSONObject_1, _function_1);
      JsonArray _optJSONArray = this._jsonAdapter.optJSONArray(jsonObj, "bendPoints");
      final Procedure1<JsonArray> _function_2 = new Procedure1<JsonArray>() {
        @Override
        public void apply(final JsonArray bendPoints) {
          if ((bendPoints != null)) {
            int _sizeJsonArr = JsonImporter.this._jsonAdapter.sizeJsonArr(bendPoints);
            ExclusiveRange _doubleDotLessThan = new ExclusiveRange(0, _sizeJsonArr, true);
            for (final Integer i : _doubleDotLessThan) {
              JsonObject _optJSONObject = JsonImporter.this._jsonAdapter.optJSONObject(bendPoints, (i).intValue());
              final Procedure1<JsonObject> _function = new Procedure1<JsonObject>() {
                @Override
                public void apply(final JsonObject bendPoint) {
                  ElkGraphUtil.createBendPoint(section, (JsonImporter.this._jsonAdapter.optDouble(bendPoint, "x")).doubleValue(), (JsonImporter.this._jsonAdapter.optDouble(bendPoint, "y")).doubleValue());
                }
              };
              ObjectExtensions.<JsonObject>operator_doubleArrow(_optJSONObject, _function);
            }
          }
        }
      };
      _xblockexpression = ObjectExtensions.<JsonArray>operator_doubleArrow(_optJSONArray, _function_2);
    }
    return _xblockexpression;
  }

  private void transformProperties(final Object jsonObjA, final IPropertyHolder layoutData) {
    final JsonObject jsonObj = this._jsonAdapter.toJsonObject(jsonObjA);
    JsonObject layoutOptions = this._jsonAdapter.optJSONObject(jsonObj, "layoutOptions");
    if ((layoutOptions == null)) {
      layoutOptions = this._jsonAdapter.optJSONObject(jsonObj, "properties");
    }
    if ((layoutOptions != null)) {
      final JsonObject opts = layoutOptions;
      Iterable<String> _keysJsonObj = null;
      if (opts!=null) {
        _keysJsonObj=this._jsonAdapter.keysJsonObj(opts);
      }
      if (_keysJsonObj!=null) {
        final Consumer<String> _function = new Consumer<String>() {
          @Override
          public void accept(final String k) {
            JsonElement _jsonObj = JsonImporter.this._jsonAdapter.getJsonObj(opts, k);
            String _stringVal = null;
            if (_jsonObj!=null) {
              _stringVal=JsonImporter.this._jsonAdapter.stringVal(_jsonObj);
            }
            final String value = _stringVal;
            JsonImporter.this.setOption(layoutData, k, value);
          }
        };
        _keysJsonObj.forEach(_function);
      }
    }
  }

  private void transformIndividualSpacings(final Object jsonObjA, final IPropertyHolder layoutData) {
    final JsonObject jsonObj = this._jsonAdapter.toJsonObject(jsonObjA);
    final JsonObject jsonIndividualSpacings = this._jsonAdapter.optJSONObject(jsonObj, "individualSpacings");
    if ((jsonIndividualSpacings != null)) {
      boolean _hasProperty = layoutData.hasProperty(CoreOptions.SPACING_INDIVIDUAL);
      boolean _not = (!_hasProperty);
      if (_not) {
        IndividualSpacings _individualSpacings = new IndividualSpacings();
        layoutData.<IndividualSpacings>setProperty(CoreOptions.SPACING_INDIVIDUAL, _individualSpacings);
      }
      final IndividualSpacings individualSpacings = layoutData.<IndividualSpacings>getProperty(CoreOptions.SPACING_INDIVIDUAL);
      final JsonObject opts = jsonIndividualSpacings;
      Iterable<String> _keysJsonObj = null;
      if (opts!=null) {
        _keysJsonObj=this._jsonAdapter.keysJsonObj(opts);
      }
      if (_keysJsonObj!=null) {
        final Consumer<String> _function = new Consumer<String>() {
          @Override
          public void accept(final String k) {
            JsonElement _jsonObj = JsonImporter.this._jsonAdapter.getJsonObj(opts, k);
            String _stringVal = null;
            if (_jsonObj!=null) {
              _stringVal=JsonImporter.this._jsonAdapter.stringVal(_jsonObj);
            }
            final String value = _stringVal;
            JsonImporter.this.setOption(individualSpacings, k, value);
          }
        };
        _keysJsonObj.forEach(_function);
      }
    }
  }

  private IPropertyHolder setOption(final IPropertyHolder e, final String id, final String value) {
    IPropertyHolder _xblockexpression = null;
    {
      final LayoutOptionData optionData = LayoutMetaDataService.getInstance().getOptionDataBySuffix(id);
      IPropertyHolder _xifexpression = null;
      if ((optionData != null)) {
        IPropertyHolder _xblockexpression_1 = null;
        {
          final Object parsed = optionData.parseValue(value);
          IPropertyHolder _xifexpression_1 = null;
          if ((parsed != null)) {
            _xifexpression_1 = e.<Object>setProperty(optionData, parsed);
          }
          _xblockexpression_1 = _xifexpression_1;
        }
        _xifexpression = _xblockexpression_1;
      }
      _xblockexpression = _xifexpression;
    }
    return _xblockexpression;
  }

  private JsonArray transformLabels(final Object jsonObjA, final ElkGraphElement element) {
    JsonArray _xblockexpression = null;
    {
      final JsonObject jsonObj = this._jsonAdapter.toJsonObject(jsonObjA);
      JsonArray _optJSONArray = this._jsonAdapter.optJSONArray(jsonObj, "labels");
      final Procedure1<JsonArray> _function = new Procedure1<JsonArray>() {
        @Override
        public void apply(final JsonArray labels) {
          if ((labels != null)) {
            int _sizeJsonArr = JsonImporter.this._jsonAdapter.sizeJsonArr(labels);
            ExclusiveRange _doubleDotLessThan = new ExclusiveRange(0, _sizeJsonArr, true);
            for (final Integer i : _doubleDotLessThan) {
              {
                final JsonObject jsonLabel = JsonImporter.this._jsonAdapter.optJSONObject(labels, (i).intValue());
                if ((jsonLabel != null)) {
                  final ElkLabel label = ElkGraphUtil.createLabel(JsonImporter.this._jsonAdapter.optString(jsonLabel, "text"), element);
                  JsonImporter.this.labelJsonMap.put(label, jsonLabel);
                  boolean _hasJsonObj = JsonImporter.this._jsonAdapter.hasJsonObj(jsonLabel, "id");
                  if (_hasJsonObj) {
                    label.setIdentifier(JsonImporter.this._jsonAdapter.getIdSave(jsonLabel));
                  }
                  JsonImporter.this.transformProperties(jsonLabel, label);
                  JsonImporter.this.transformShapeLayout(jsonLabel, label);
                }
              }
            }
          }
        }
      };
      _xblockexpression = ObjectExtensions.<JsonArray>operator_doubleArrow(_optJSONArray, _function);
    }
    return _xblockexpression;
  }

  private JsonArray transformPorts(final Object jsonObjA, final ElkNode parent) {
    JsonArray _xblockexpression = null;
    {
      final JsonObject jsonObj = this._jsonAdapter.toJsonObject(jsonObjA);
      JsonArray _optJSONArray = this._jsonAdapter.optJSONArray(jsonObj, "ports");
      final Procedure1<JsonArray> _function = new Procedure1<JsonArray>() {
        @Override
        public void apply(final JsonArray ports) {
          if ((ports != null)) {
            int _sizeJsonArr = JsonImporter.this._jsonAdapter.sizeJsonArr(ports);
            ExclusiveRange _doubleDotLessThan = new ExclusiveRange(0, _sizeJsonArr, true);
            for (final Integer i : _doubleDotLessThan) {
              JsonObject _optJSONObject = JsonImporter.this._jsonAdapter.optJSONObject(ports, (i).intValue());
              if (_optJSONObject!=null) {
                JsonImporter.this.transformPort(_optJSONObject, parent);
              }
            }
          }
        }
      };
      _xblockexpression = ObjectExtensions.<JsonArray>operator_doubleArrow(_optJSONArray, _function);
    }
    return _xblockexpression;
  }

  private JsonArray transformPort(final Object jsonPort, final ElkNode parent) {
    JsonArray _xblockexpression = null;
    {
      final ElkPort port = this.register(ElkGraphUtil.createPort(parent), jsonPort);
      port.setIdentifier(this._jsonAdapter.getIdSave(this._jsonAdapter.toJsonObject(jsonPort)));
      this.transformProperties(jsonPort, port);
      this.transformShapeLayout(jsonPort, port);
      _xblockexpression = this.transformLabels(jsonPort, port);
    }
    return _xblockexpression;
  }

  private Double transformShapeLayout(final Object jsonObjA, final ElkShape shape) {
    Double _xblockexpression = null;
    {
      final JsonObject jsonObj = this._jsonAdapter.toJsonObject(jsonObjA);
      Double _optDouble = this._jsonAdapter.optDouble(jsonObj, "x");
      final Procedure1<Double> _function = new Procedure1<Double>() {
        @Override
        public void apply(final Double it) {
          shape.setX(JsonImporter.this.doubleValueValid(it));
        }
      };
      ObjectExtensions.<Double>operator_doubleArrow(_optDouble, _function);
      Double _optDouble_1 = this._jsonAdapter.optDouble(jsonObj, "y");
      final Procedure1<Double> _function_1 = new Procedure1<Double>() {
        @Override
        public void apply(final Double it) {
          shape.setY(JsonImporter.this.doubleValueValid(it));
        }
      };
      ObjectExtensions.<Double>operator_doubleArrow(_optDouble_1, _function_1);
      Double _optDouble_2 = this._jsonAdapter.optDouble(jsonObj, "width");
      final Procedure1<Double> _function_2 = new Procedure1<Double>() {
        @Override
        public void apply(final Double it) {
          shape.setWidth(JsonImporter.this.doubleValueValid(it));
        }
      };
      ObjectExtensions.<Double>operator_doubleArrow(_optDouble_2, _function_2);
      Double _optDouble_3 = this._jsonAdapter.optDouble(jsonObj, "height");
      final Procedure1<Double> _function_3 = new Procedure1<Double>() {
        @Override
        public void apply(final Double it) {
          shape.setHeight(JsonImporter.this.doubleValueValid(it));
        }
      };
      _xblockexpression = ObjectExtensions.<Double>operator_doubleArrow(_optDouble_3, _function_3);
    }
    return _xblockexpression;
  }

  private double doubleValueValid(final Double d) {
    if ((((d == null) || d.isInfinite()) || d.isNaN())) {
      return 0.0;
    } else {
      return d.doubleValue();
    }
  }

  private ElkConnectableShape shapeById(final Object id) {
    final ElkNode node = this.nodeIdMap.get(id);
    if ((node != null)) {
      return node;
    }
    final ElkPort port = this.portIdMap.get(id);
    if ((port != null)) {
      return port;
    }
    throw this._jsonAdapter.formatError(("Referenced shape does not exist: " + id));
  }

  /**
   * Transfer the layout back to the formerly imported graph, using {@link #transform(Object)}.
   */
  public void transferLayout(final ElkNode graph) {
    final Procedure1<EObject> _function = new Procedure1<EObject>() {
      @Override
      public void apply(final EObject element) {
        JsonImporter.this.transferLayoutInt(element);
      }
    };
    IteratorExtensions.<EObject>forEach(ElkGraphUtil.propertiesSkippingIteratorFor(graph, true), _function);
  }

  private Object _transferLayoutInt(final ElkNode node) {
    final Object jsonObj = this.nodeJsonMap.get(node);
    if ((jsonObj == null)) {
      throw this._jsonAdapter.formatError("Node did not exist in input.");
    }
    this.transferShapeLayout(node, jsonObj);
    return null;
  }

  private Object _transferLayoutInt(final ElkPort port) {
    final Object jsonObj = this.portJsonMap.get(port);
    if ((jsonObj == null)) {
      throw this._jsonAdapter.formatError("Port did not exist in input.");
    }
    this.transferShapeLayout(port, jsonObj);
    return null;
  }

  private Object _transferLayoutInt(final ElkEdge edge) {
    final JsonObject jsonObj = this._jsonAdapter.toJsonObject(this.edgeJsonMap.get(edge));
    if ((jsonObj == null)) {
      throw this._jsonAdapter.formatError("Edge did not exist in input.");
    }
    final Object edgeId = this._jsonAdapter.getId(jsonObj);
    boolean _isNullOrEmpty = IterableExtensions.isNullOrEmpty(edge.getSections());
    boolean _not = (!_isNullOrEmpty);
    if (_not) {
      final JsonArray sections = this._jsonAdapter.newJsonArray();
      final Procedure2<ElkEdgeSection, Integer> _function = new Procedure2<ElkEdgeSection, Integer>() {
        @Override
        public void apply(final ElkEdgeSection elkSection, final Integer i) {
          Object maybeSection = JsonImporter.this.edgeSectionJsonMap.get(elkSection);
          if ((maybeSection == null)) {
            maybeSection = JsonImporter.this._jsonAdapter.newJsonObject();
            JsonObject _jsonObject = JsonImporter.this._jsonAdapter.toJsonObject(maybeSection);
            String _plus = (edgeId + "_s");
            String _plus_1 = (_plus + i);
            JsonImporter.this._jsonAdapter.addJsonObj(_jsonObject, "id", _plus_1);
          }
          final JsonObject jsonSection = JsonImporter.this._jsonAdapter.toJsonObject(maybeSection);
          JsonImporter.this._jsonAdapter.addJsonArr(sections, jsonSection);
          final JsonObject startPoint = JsonImporter.this._jsonAdapter.newJsonObject();
          JsonImporter.this._jsonAdapter.addJsonObj(startPoint, "x", Double.valueOf(elkSection.getStartX()));
          JsonImporter.this._jsonAdapter.addJsonObj(startPoint, "y", Double.valueOf(elkSection.getStartY()));
          JsonImporter.this._jsonAdapter.addJsonObj(jsonSection, "startPoint", startPoint);
          final JsonObject endPoint = JsonImporter.this._jsonAdapter.newJsonObject();
          JsonImporter.this._jsonAdapter.addJsonObj(endPoint, "x", Double.valueOf(elkSection.getEndX()));
          JsonImporter.this._jsonAdapter.addJsonObj(endPoint, "y", Double.valueOf(elkSection.getEndY()));
          JsonImporter.this._jsonAdapter.addJsonObj(jsonSection, "endPoint", endPoint);
          boolean _isNullOrEmpty = IterableExtensions.isNullOrEmpty(elkSection.getBendPoints());
          boolean _not = (!_isNullOrEmpty);
          if (_not) {
            final JsonArray bendPoints = JsonImporter.this._jsonAdapter.newJsonArray();
            final Consumer<ElkBendPoint> _function = new Consumer<ElkBendPoint>() {
              @Override
              public void accept(final ElkBendPoint pnt) {
                final JsonObject jsonPnt = JsonImporter.this._jsonAdapter.newJsonObject();
                JsonImporter.this._jsonAdapter.addJsonObj(jsonPnt, "x", Double.valueOf(pnt.getX()));
                JsonImporter.this._jsonAdapter.addJsonObj(jsonPnt, "y", Double.valueOf(pnt.getY()));
                JsonImporter.this._jsonAdapter.addJsonArr(bendPoints, jsonPnt);
              }
            };
            elkSection.getBendPoints().forEach(_function);
            JsonImporter.this._jsonAdapter.addJsonObj(jsonSection, "bendPoints", bendPoints);
          }
          ElkConnectableShape _incomingShape = elkSection.getIncomingShape();
          boolean _tripleNotEquals = (_incomingShape != null);
          if (_tripleNotEquals) {
            JsonImporter.this._jsonAdapter.addJsonObj(jsonSection, "incomingShape", JsonImporter.this.idByElement(elkSection.getIncomingShape()));
          }
          ElkConnectableShape _outgoingShape = elkSection.getOutgoingShape();
          boolean _tripleNotEquals_1 = (_outgoingShape != null);
          if (_tripleNotEquals_1) {
            JsonImporter.this._jsonAdapter.addJsonObj(jsonSection, "outgoingShape", JsonImporter.this.idByElement(elkSection.getOutgoingShape()));
          }
          boolean _isEmpty = elkSection.getIncomingSections().isEmpty();
          boolean _not_1 = (!_isEmpty);
          if (_not_1) {
            final JsonArray incomingSections = JsonImporter.this._jsonAdapter.newJsonArray();
            final Consumer<ElkEdgeSection> _function_1 = new Consumer<ElkEdgeSection>() {
              @Override
              public void accept(final ElkEdgeSection sec) {
                JsonImporter.this._jsonAdapter.addJsonArr(incomingSections, JsonImporter.this.idByElement(sec));
              }
            };
            elkSection.getIncomingSections().forEach(_function_1);
            JsonImporter.this._jsonAdapter.addJsonObj(jsonSection, "incomingSections", incomingSections);
          }
          boolean _isEmpty_1 = elkSection.getOutgoingSections().isEmpty();
          boolean _not_2 = (!_isEmpty_1);
          if (_not_2) {
            final JsonArray outgoingSections = JsonImporter.this._jsonAdapter.newJsonArray();
            final Consumer<ElkEdgeSection> _function_2 = new Consumer<ElkEdgeSection>() {
              @Override
              public void accept(final ElkEdgeSection sec) {
                JsonImporter.this._jsonAdapter.addJsonArr(outgoingSections, JsonImporter.this.idByElement(sec));
              }
            };
            elkSection.getOutgoingSections().forEach(_function_2);
            JsonImporter.this._jsonAdapter.addJsonObj(jsonSection, "outgoingSections", outgoingSections);
          }
        }
      };
      IterableExtensions.<ElkEdgeSection>forEach(edge.getSections(), _function);
      this._jsonAdapter.addJsonObj(jsonObj, "sections", sections);
    }
    boolean _hasProperty = edge.hasProperty(CoreOptions.JUNCTION_POINTS);
    if (_hasProperty) {
      final KVectorChain jps = edge.<KVectorChain>getProperty(CoreOptions.JUNCTION_POINTS);
      boolean _isNullOrEmpty_1 = IterableExtensions.isNullOrEmpty(jps);
      boolean _not_1 = (!_isNullOrEmpty_1);
      if (_not_1) {
        final JsonArray jsonJPs = this._jsonAdapter.newJsonArray();
        final Consumer<KVector> _function_1 = new Consumer<KVector>() {
          @Override
          public void accept(final KVector jp) {
            final JsonObject jsonPnt = JsonImporter.this._jsonAdapter.newJsonObject();
            JsonImporter.this._jsonAdapter.addJsonObj(jsonPnt, "x", Double.valueOf(jp.x));
            JsonImporter.this._jsonAdapter.addJsonObj(jsonPnt, "y", Double.valueOf(jp.y));
            JsonImporter.this._jsonAdapter.addJsonArr(jsonJPs, jsonPnt);
          }
        };
        jps.forEach(_function_1);
        this._jsonAdapter.addJsonObj(jsonObj, "junctionPoints", jsonJPs);
      }
    }
    this._jsonAdapter.addJsonObj(jsonObj, "container", edge.getContainingNode().getIdentifier());
    return null;
  }

  private Object _transferLayoutInt(final ElkLabel label) {
    final Object jsonObj = this.labelJsonMap.get(label);
    this.transferShapeLayout(label, jsonObj);
    return null;
  }

  private Object _transferLayoutInt(final Object obj) {
    return null;
  }

  private void transferShapeLayout(final ElkShape shape, final Object jsonObjA) {
    final JsonObject jsonObj = this._jsonAdapter.toJsonObject(jsonObjA);
    this._jsonAdapter.addJsonObj(jsonObj, "x", Double.valueOf(shape.getX()));
    this._jsonAdapter.addJsonObj(jsonObj, "y", Double.valueOf(shape.getY()));
    this._jsonAdapter.addJsonObj(jsonObj, "width", Double.valueOf(shape.getWidth()));
    this._jsonAdapter.addJsonObj(jsonObj, "height", Double.valueOf(shape.getHeight()));
  }

  private Object _idByElement(final ElkNode node) {
    return this.nodeIdMap.inverse().get(node);
  }

  private Object _idByElement(final ElkPort port) {
    return this.portIdMap.inverse().get(port);
  }

  private Object _idByElement(final ElkEdgeSection section) {
    return this.edgeSectionIdMap.inverse().get(section);
  }

  /**
   * ---------------------------------------------------------------------------
   *                            Convenience methods
   * ---------------------------------------------------------------------------
   */
  private ElkNode register(final ElkNode node, final Object obj) {
    final Object id = this._jsonAdapter.getId(this._jsonAdapter.toJsonObject(obj));
    this.nodeIdMap.put(id, node);
    this.nodeJsonMap.put(node, obj);
    return node;
  }

  private ElkPort register(final ElkPort port, final Object obj) {
    final Object id = this._jsonAdapter.getId(this._jsonAdapter.toJsonObject(obj));
    this.portIdMap.put(id, port);
    this.portJsonMap.put(port, obj);
    return port;
  }

  private ElkEdge register(final ElkEdge edge, final Object obj) {
    final Object id = this._jsonAdapter.getId(this._jsonAdapter.toJsonObject(obj));
    this.edgeIdMap.put(id, edge);
    this.edgeJsonMap.put(edge, obj);
    return edge;
  }

  private ElkEdgeSection register(final ElkEdgeSection edgeSection, final Object obj) {
    final Object id = this._jsonAdapter.getId(this._jsonAdapter.toJsonObject(obj));
    this.edgeSectionIdMap.put(id, edgeSection);
    this.edgeSectionJsonMap.put(edgeSection, obj);
    return edgeSection;
  }

  private Object transferLayoutInt(final Object node) {
    if (node instanceof ElkNode) {
      return _transferLayoutInt((ElkNode)node);
    } else if (node instanceof ElkPort) {
      return _transferLayoutInt((ElkPort)node);
    } else if (node instanceof ElkLabel) {
      return _transferLayoutInt((ElkLabel)node);
    } else if (node instanceof ElkEdge) {
      return _transferLayoutInt((ElkEdge)node);
    } else if (node != null) {
      return _transferLayoutInt(node);
    } else {
      throw new IllegalArgumentException("Unhandled parameter types: " +
        Arrays.<Object>asList(node).toString());
    }
  }

  private Object idByElement(final EMapPropertyHolder node) {
    if (node instanceof ElkNode) {
      return _idByElement((ElkNode)node);
    } else if (node instanceof ElkPort) {
      return _idByElement((ElkPort)node);
    } else if (node instanceof ElkEdgeSection) {
      return _idByElement((ElkEdgeSection)node);
    } else {
      throw new IllegalArgumentException("Unhandled parameter types: " +
        Arrays.<Object>asList(node).toString());
    }
  }
}
