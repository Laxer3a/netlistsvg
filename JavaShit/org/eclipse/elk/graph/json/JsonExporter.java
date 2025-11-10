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
import com.google.common.base.Splitter;
import com.google.common.collect.BiMap;
import com.google.common.collect.HashBiMap;
import com.google.common.collect.Maps;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import java.util.Arrays;
import java.util.Collections;
import java.util.Iterator;
import java.util.Map;
import java.util.Random;
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
import org.eclipse.elk.graph.ElkLabel;
import org.eclipse.elk.graph.ElkNode;
import org.eclipse.elk.graph.ElkPort;
import org.eclipse.elk.graph.ElkShape;
import org.eclipse.elk.graph.properties.IProperty;
import org.eclipse.elk.graph.properties.IPropertyHolder;
import org.eclipse.xtext.xbase.lib.Conversions;
import org.eclipse.xtext.xbase.lib.Extension;
import org.eclipse.xtext.xbase.lib.Functions.Function1;
import org.eclipse.xtext.xbase.lib.IterableExtensions;
import org.eclipse.xtext.xbase.lib.StringExtensions;

/**
 * Exporter from elk graph to json.
 */
@SuppressWarnings("all")
public final class JsonExporter {
  @Extension
  private JsonAdapter _jsonAdapter = new JsonAdapter();

  private final BiMap<ElkNode, String> nodeIdMap = HashBiMap.<ElkNode, String>create();

  private final BiMap<ElkPort, String> portIdMap = HashBiMap.<ElkPort, String>create();

  private final BiMap<ElkEdge, String> edgeIdMap = HashBiMap.<ElkEdge, String>create();

  private final BiMap<ElkEdgeSection, String> edgeSectionIdMap = HashBiMap.<ElkEdgeSection, String>create();

  private final Map<ElkNode, Object> nodeJsonMap = Maps.<ElkNode, Object>newHashMap();

  private final Map<ElkPort, Object> portJsonMap = Maps.<ElkPort, Object>newHashMap();

  private final Map<ElkEdge, Object> edgeJsonMap = Maps.<ElkEdge, Object>newHashMap();

  private final Map<ElkEdgeSection, Object> edgeSectionJsonMap = Maps.<ElkEdgeSection, Object>newHashMap();

  private int nodeIdCounter = 0;

  private int portIdCounter = 0;

  private int edgeIdCounter = 0;

  private int edgeSectionIdCounter = 0;

  private boolean omitZeroPos = true;

  private boolean omitZeroDim = true;

  private boolean omitLayout = false;

  private boolean shortLayoutOptionKeys = true;

  private boolean omitUnknownLayoutOptions = true;

  public JsonExporter() {
  }

  public boolean setOptions(final boolean omitZeroPos, final boolean omitZeroDim, final boolean omitLayout, final boolean shortLayoutOptionKeys, final boolean omitUnknownLayoutOptions) {
    boolean _xblockexpression = false;
    {
      this.omitZeroPos = omitZeroPos;
      this.omitZeroDim = omitZeroDim;
      this.omitLayout = omitLayout;
      this.shortLayoutOptionKeys = shortLayoutOptionKeys;
      _xblockexpression = this.omitUnknownLayoutOptions = omitUnknownLayoutOptions;
    }
    return _xblockexpression;
  }

  public JsonObject export(final ElkNode root) {
    this.init();
    final JsonArray jsonArray = this._jsonAdapter.newJsonArray();
    this.transformNode(root, jsonArray);
    this.transformEdges(root);
    final JsonObject jsonGraph = this._jsonAdapter.optJSONObject(jsonArray, 0);
    return jsonGraph;
  }

  private int init() {
    int _xblockexpression = (int) 0;
    {
      this.nodeIdMap.clear();
      this.portIdMap.clear();
      this.edgeIdMap.clear();
      this.edgeSectionIdMap.clear();
      this.nodeJsonMap.clear();
      this.portJsonMap.clear();
      this.edgeJsonMap.clear();
      this.edgeSectionJsonMap.clear();
      this.nodeIdCounter = 0;
      this.portIdCounter = 0;
      this.edgeIdCounter = 0;
      _xblockexpression = this.edgeSectionIdCounter = 0;
    }
    return _xblockexpression;
  }

  private void transformNode(final ElkNode node, final Object arrayA) {
    final JsonObject jsonObj = this.createAndRegister(node);
    final JsonArray array = this._jsonAdapter.toJsonArray(arrayA);
    this._jsonAdapter.addJsonArr(array, jsonObj);
    boolean _isNullOrEmpty = IterableExtensions.isNullOrEmpty(node.getLabels());
    boolean _not = (!_isNullOrEmpty);
    if (_not) {
      final JsonArray labels = this._jsonAdapter.newJsonArray();
      this._jsonAdapter.addJsonObj(jsonObj, "labels", labels);
      final Consumer<ElkLabel> _function = new Consumer<ElkLabel>() {
        @Override
        public void accept(final ElkLabel it) {
          JsonExporter.this.transformLabel(it, labels);
        }
      };
      node.getLabels().forEach(_function);
    }
    boolean _isNullOrEmpty_1 = IterableExtensions.isNullOrEmpty(node.getPorts());
    boolean _not_1 = (!_isNullOrEmpty_1);
    if (_not_1) {
      final JsonArray ports = this._jsonAdapter.newJsonArray();
      this._jsonAdapter.addJsonObj(jsonObj, "ports", ports);
      final Consumer<ElkPort> _function_1 = new Consumer<ElkPort>() {
        @Override
        public void accept(final ElkPort it) {
          JsonExporter.this.transformPort(it, ports);
        }
      };
      node.getPorts().forEach(_function_1);
    }
    boolean _isNullOrEmpty_2 = IterableExtensions.isNullOrEmpty(node.getChildren());
    boolean _not_2 = (!_isNullOrEmpty_2);
    if (_not_2) {
      final JsonArray children = this._jsonAdapter.newJsonArray();
      this._jsonAdapter.addJsonObj(jsonObj, "children", children);
      final Consumer<ElkNode> _function_2 = new Consumer<ElkNode>() {
        @Override
        public void accept(final ElkNode it) {
          JsonExporter.this.transformNode(it, children);
        }
      };
      node.getChildren().forEach(_function_2);
    }
    this.transformProperties(node, jsonObj);
    this.transformIndividualSpacings(node, jsonObj);
    this.transferShapeLayout(node, jsonObj);
  }

  private void transformPort(final ElkPort port, final Object arrayA) {
    final JsonObject jsonObj = this.createAndRegister(port);
    final JsonArray array = this._jsonAdapter.toJsonArray(arrayA);
    this._jsonAdapter.addJsonArr(array, jsonObj);
    boolean _isNullOrEmpty = IterableExtensions.isNullOrEmpty(port.getLabels());
    boolean _not = (!_isNullOrEmpty);
    if (_not) {
      final JsonArray labels = this._jsonAdapter.newJsonArray();
      this._jsonAdapter.addJsonObj(jsonObj, "labels", labels);
      final Consumer<ElkLabel> _function = new Consumer<ElkLabel>() {
        @Override
        public void accept(final ElkLabel it) {
          JsonExporter.this.transformLabel(it, labels);
        }
      };
      port.getLabels().forEach(_function);
    }
    this.transformProperties(port, jsonObj);
    this.transferShapeLayout(port, jsonObj);
  }

  private void transformEdges(final ElkNode node) {
    boolean _isNullOrEmpty = IterableExtensions.isNullOrEmpty(node.getContainedEdges());
    boolean _not = (!_isNullOrEmpty);
    if (_not) {
      final JsonObject jsonObj = this._jsonAdapter.toJsonObject(this.nodeJsonMap.get(node));
      final JsonArray edges = this._jsonAdapter.newJsonArray();
      this._jsonAdapter.addJsonObj(jsonObj, "edges", edges);
      final Consumer<ElkEdge> _function = new Consumer<ElkEdge>() {
        @Override
        public void accept(final ElkEdge it) {
          JsonExporter.this.transformEdge(it, edges);
        }
      };
      node.getContainedEdges().forEach(_function);
    }
    final Consumer<ElkNode> _function_1 = new Consumer<ElkNode>() {
      @Override
      public void accept(final ElkNode it) {
        JsonExporter.this.transformEdges(it);
      }
    };
    node.getChildren().forEach(_function_1);
  }

  private void transformEdge(final ElkEdge edge, final Object arrayA) {
    final JsonObject jsonObj = this.createAndRegister(edge);
    final JsonArray array = this._jsonAdapter.toJsonArray(arrayA);
    this._jsonAdapter.addJsonArr(array, jsonObj);
    final JsonArray sources = this._jsonAdapter.newJsonArray();
    final Consumer<ElkConnectableShape> _function = new Consumer<ElkConnectableShape>() {
      @Override
      public void accept(final ElkConnectableShape s) {
        String source = JsonExporter.this.portIdMap.get(s);
        if ((source == null)) {
          source = JsonExporter.this.nodeIdMap.get(s);
        }
        if ((source == null)) {
          JsonExporter.this._jsonAdapter.formatError(("Unknown edge source: " + s));
        }
        JsonExporter.this._jsonAdapter.addJsonArr(sources, JsonExporter.this._jsonAdapter.toJson(source));
      }
    };
    edge.getSources().forEach(_function);
    this._jsonAdapter.addJsonObj(jsonObj, "sources", sources);
    final JsonArray targets = this._jsonAdapter.newJsonArray();
    final Consumer<ElkConnectableShape> _function_1 = new Consumer<ElkConnectableShape>() {
      @Override
      public void accept(final ElkConnectableShape t) {
        String target = JsonExporter.this.portIdMap.get(t);
        if ((target == null)) {
          target = JsonExporter.this.nodeIdMap.get(t);
        }
        if ((target == null)) {
          JsonExporter.this._jsonAdapter.formatError(("Unknown edge target: " + target));
        }
        JsonExporter.this._jsonAdapter.addJsonArr(targets, JsonExporter.this._jsonAdapter.toJson(target));
      }
    };
    edge.getTargets().forEach(_function_1);
    this._jsonAdapter.addJsonObj(jsonObj, "targets", targets);
    boolean _isNullOrEmpty = IterableExtensions.isNullOrEmpty(edge.getLabels());
    boolean _not = (!_isNullOrEmpty);
    if (_not) {
      final JsonArray labels = this._jsonAdapter.newJsonArray();
      this._jsonAdapter.addJsonObj(jsonObj, "labels", labels);
      final Consumer<ElkLabel> _function_2 = new Consumer<ElkLabel>() {
        @Override
        public void accept(final ElkLabel it) {
          JsonExporter.this.transformLabel(it, labels);
        }
      };
      edge.getLabels().forEach(_function_2);
    }
    if (((!this.omitLayout) && (!IterableExtensions.isNullOrEmpty(edge.getSections())))) {
      final JsonArray sections = this._jsonAdapter.newJsonArray();
      this._jsonAdapter.addJsonObj(jsonObj, "sections", sections);
      final Consumer<ElkEdgeSection> _function_3 = new Consumer<ElkEdgeSection>() {
        @Override
        public void accept(final ElkEdgeSection it) {
          JsonExporter.this.transformSection(it, sections);
        }
      };
      edge.getSections().forEach(_function_3);
    }
    if (((!this.omitLayout) && edge.hasProperty(CoreOptions.JUNCTION_POINTS))) {
      final KVectorChain jps = edge.<KVectorChain>getProperty(CoreOptions.JUNCTION_POINTS);
      if (((!this.omitLayout) && (!IterableExtensions.isNullOrEmpty(jps)))) {
        final JsonArray jsonJPs = this._jsonAdapter.newJsonArray();
        final Consumer<KVector> _function_4 = new Consumer<KVector>() {
          @Override
          public void accept(final KVector jp) {
            final JsonObject jsonPnt = JsonExporter.this._jsonAdapter.newJsonObject();
            JsonExporter.this._jsonAdapter.addJsonObj(jsonPnt, "x", Double.valueOf(jp.x));
            JsonExporter.this._jsonAdapter.addJsonObj(jsonPnt, "y", Double.valueOf(jp.y));
            JsonExporter.this._jsonAdapter.addJsonArr(jsonJPs, jsonPnt);
          }
        };
        jps.forEach(_function_4);
        this._jsonAdapter.addJsonObj(jsonObj, "junctionPoints", jsonJPs);
      }
    }
    this.transformProperties(edge, jsonObj);
  }

  private void transformSection(final ElkEdgeSection section, final Object sectionsA) {
    final JsonObject jsonObj = this.createAndRegister(section);
    final JsonArray sections = this._jsonAdapter.toJsonArray(sectionsA);
    this._jsonAdapter.addJsonArr(sections, jsonObj);
    final JsonObject startPoint = this._jsonAdapter.newJsonObject();
    startPoint.addProperty("x", Double.valueOf(section.getStartX()));
    startPoint.addProperty("y", Double.valueOf(section.getStartY()));
    this._jsonAdapter.addJsonObj(jsonObj, "startPoint", startPoint);
    final JsonObject endPoint = this._jsonAdapter.newJsonObject();
    endPoint.addProperty("x", Double.valueOf(section.getEndX()));
    endPoint.addProperty("y", Double.valueOf(section.getEndY()));
    this._jsonAdapter.addJsonObj(jsonObj, "endPoint", endPoint);
    if (((!this.omitLayout) && (!IterableExtensions.isNullOrEmpty(section.getBendPoints())))) {
      final JsonArray bendPoints = this._jsonAdapter.newJsonArray();
      final Consumer<ElkBendPoint> _function = new Consumer<ElkBendPoint>() {
        @Override
        public void accept(final ElkBendPoint pnt) {
          final JsonObject jsonPnt = JsonExporter.this._jsonAdapter.newJsonObject();
          jsonPnt.addProperty("x", Double.valueOf(pnt.getX()));
          jsonPnt.addProperty("y", Double.valueOf(pnt.getY()));
          JsonExporter.this._jsonAdapter.addJsonArr(bendPoints, jsonPnt);
        }
      };
      section.getBendPoints().forEach(_function);
      this._jsonAdapter.addJsonObj(jsonObj, "bendPoints", bendPoints);
    }
    ElkConnectableShape _incomingShape = section.getIncomingShape();
    boolean _tripleNotEquals = (_incomingShape != null);
    if (_tripleNotEquals) {
      jsonObj.addProperty("incomingShape", this.idByElement(section.getIncomingShape()));
    }
    ElkConnectableShape _outgoingShape = section.getOutgoingShape();
    boolean _tripleNotEquals_1 = (_outgoingShape != null);
    if (_tripleNotEquals_1) {
      jsonObj.addProperty("outgoingShape", this.idByElement(section.getOutgoingShape()));
    }
    boolean _isNullOrEmpty = IterableExtensions.isNullOrEmpty(section.getIncomingSections());
    boolean _not = (!_isNullOrEmpty);
    if (_not) {
      final JsonArray incomingSections = this._jsonAdapter.newJsonArray();
      final Consumer<ElkEdgeSection> _function_1 = new Consumer<ElkEdgeSection>() {
        @Override
        public void accept(final ElkEdgeSection sec) {
          JsonExporter.this._jsonAdapter.addJsonArr(incomingSections, JsonExporter.this._jsonAdapter.toJson(JsonExporter.this.idByElement(sec)));
        }
      };
      section.getIncomingSections().forEach(_function_1);
      this._jsonAdapter.addJsonObj(jsonObj, "incomingSections", incomingSections);
    }
    boolean _isNullOrEmpty_1 = IterableExtensions.isNullOrEmpty(section.getOutgoingSections());
    boolean _not_1 = (!_isNullOrEmpty_1);
    if (_not_1) {
      final JsonArray outgoingSections = this._jsonAdapter.newJsonArray();
      final Consumer<ElkEdgeSection> _function_2 = new Consumer<ElkEdgeSection>() {
        @Override
        public void accept(final ElkEdgeSection sec) {
          JsonExporter.this._jsonAdapter.addJsonArr(outgoingSections, JsonExporter.this._jsonAdapter.toJson(JsonExporter.this.idByElement(sec)));
        }
      };
      section.getOutgoingSections().forEach(_function_2);
      this._jsonAdapter.addJsonObj(jsonObj, "outgoingSections", outgoingSections);
    }
    this.transformProperties(section, jsonObj);
  }

  private void transformLabel(final ElkLabel label, final Object array) {
    final JsonObject jsonLabel = this._jsonAdapter.newJsonObject();
    jsonLabel.addProperty("text", label.getText());
    boolean _isNullOrEmpty = StringExtensions.isNullOrEmpty(label.getIdentifier());
    boolean _not = (!_isNullOrEmpty);
    if (_not) {
      jsonLabel.addProperty("id", label.getIdentifier());
    }
    this._jsonAdapter.addJsonArr(this._jsonAdapter.toJsonArray(array), jsonLabel);
    this.transformProperties(label, jsonLabel);
    this.transferShapeLayout(label, jsonLabel);
  }

  private void transformProperties(final IPropertyHolder holder, final Object parentA) {
    if ((((holder == null) || (holder.getAllProperties() == null)) || holder.getAllProperties().isEmpty())) {
      return;
    }
    final JsonObject jsonProps = this._jsonAdapter.newJsonObject();
    final JsonObject parent = this._jsonAdapter.toJsonObject(parentA);
    this._jsonAdapter.addJsonObj(parent, "layoutOptions", jsonProps);
    final Function1<Map.Entry<IProperty<?>, Object>, Boolean> _function = new Function1<Map.Entry<IProperty<?>, Object>, Boolean>() {
      @Override
      public Boolean apply(final Map.Entry<IProperty<?>, Object> it) {
        IProperty<?> _key = it.getKey();
        return Boolean.valueOf((_key != null));
      }
    };
    final Function1<Map.Entry<IProperty<?>, Object>, Boolean> _function_1 = new Function1<Map.Entry<IProperty<?>, Object>, Boolean>() {
      @Override
      public Boolean apply(final Map.Entry<IProperty<?>, Object> it) {
        IProperty<?> _key = it.getKey();
        return Boolean.valueOf((!Objects.equal(_key, CoreOptions.SPACING_INDIVIDUAL)));
      }
    };
    final Consumer<Map.Entry<IProperty<?>, Object>> _function_2 = new Consumer<Map.Entry<IProperty<?>, Object>>() {
      @Override
      public void accept(final Map.Entry<IProperty<?>, Object> p) {
        if (((!JsonExporter.this.omitUnknownLayoutOptions) || JsonExporter.this.isKnown(p.getKey()))) {
          String _xifexpression = null;
          if (JsonExporter.this.shortLayoutOptionKeys) {
            _xifexpression = JsonExporter.this.getShortOptionKey(p.getKey().getId());
          } else {
            _xifexpression = p.getKey().getId();
          }
          String key = _xifexpression;
          jsonProps.addProperty(key, p.getValue().toString());
        }
      }
    };
    IterableExtensions.<Map.Entry<IProperty<?>, Object>>filter(IterableExtensions.<Map.Entry<IProperty<?>, Object>>filter(holder.getAllProperties().entrySet(), _function), _function_1).forEach(_function_2);
  }

  private void transformIndividualSpacings(final IPropertyHolder holder, final Object parentA) {
    if (((holder == null) || (!holder.hasProperty(CoreOptions.SPACING_INDIVIDUAL)))) {
      return;
    }
    final IndividualSpacings individualSpacings = holder.<IndividualSpacings>getProperty(CoreOptions.SPACING_INDIVIDUAL);
    if (((individualSpacings.getAllProperties() == null) || individualSpacings.getAllProperties().isEmpty())) {
      return;
    }
    final JsonObject jsonProps = this._jsonAdapter.newJsonObject();
    final JsonObject parent = this._jsonAdapter.toJsonObject(parentA);
    this._jsonAdapter.addJsonObj(parent, "individualSpacings", jsonProps);
    final Function1<Map.Entry<IProperty<?>, Object>, Boolean> _function = new Function1<Map.Entry<IProperty<?>, Object>, Boolean>() {
      @Override
      public Boolean apply(final Map.Entry<IProperty<?>, Object> it) {
        IProperty<?> _key = it.getKey();
        return Boolean.valueOf((_key != null));
      }
    };
    final Consumer<Map.Entry<IProperty<?>, Object>> _function_1 = new Consumer<Map.Entry<IProperty<?>, Object>>() {
      @Override
      public void accept(final Map.Entry<IProperty<?>, Object> p) {
        if (((!JsonExporter.this.omitUnknownLayoutOptions) || JsonExporter.this.isKnown(p.getKey()))) {
          String _xifexpression = null;
          if (JsonExporter.this.shortLayoutOptionKeys) {
            _xifexpression = JsonExporter.this.getShortOptionKey(p.getKey().getId());
          } else {
            _xifexpression = p.getKey().getId();
          }
          String key = _xifexpression;
          jsonProps.addProperty(key, p.getValue().toString());
        }
      }
    };
    IterableExtensions.<Map.Entry<IProperty<?>, Object>>filter(individualSpacings.getAllProperties().entrySet(), _function).forEach(_function_1);
  }

  private void transferShapeLayout(final ElkShape shape, final Object jsonObjA) {
    final JsonObject jsonObj = this._jsonAdapter.toJsonObject(jsonObjA);
    if ((!this.omitLayout)) {
      if ((!this.omitZeroPos)) {
        jsonObj.addProperty("x", Double.valueOf(shape.getX()));
        jsonObj.addProperty("y", Double.valueOf(shape.getY()));
      } else {
        double _x = shape.getX();
        boolean _notEquals = (_x != 0.0);
        if (_notEquals) {
          jsonObj.addProperty("x", Double.valueOf(shape.getX()));
        }
        double _y = shape.getY();
        boolean _notEquals_1 = (_y != 0.0);
        if (_notEquals_1) {
          jsonObj.addProperty("y", Double.valueOf(shape.getY()));
        }
      }
    }
    if ((!this.omitZeroDim)) {
      jsonObj.addProperty("width", Double.valueOf(shape.getWidth()));
      jsonObj.addProperty("height", Double.valueOf(shape.getHeight()));
    } else {
      double _width = shape.getWidth();
      boolean _notEquals_2 = (_width != 0.0);
      if (_notEquals_2) {
        jsonObj.addProperty("width", Double.valueOf(shape.getWidth()));
      }
      double _height = shape.getHeight();
      boolean _notEquals_3 = (_height != 0.0);
      if (_notEquals_3) {
        jsonObj.addProperty("height", Double.valueOf(shape.getHeight()));
      }
    }
  }

  /**
   * ---------------------------------------------------------------------------
   *   Convenience methods
   */
  private JsonObject createAndRegister(final ElkNode node) {
    final JsonObject obj = this._jsonAdapter.newJsonObject();
    String id = node.getIdentifier();
    if ((id == null)) {
      id = ("n" + Integer.valueOf(this.nodeIdCounter));
      this.nodeIdCounter = (this.nodeIdCounter + 1);
    }
    id = this.assertUnique(id, this.nodeIdMap.inverse());
    obj.addProperty("id", id);
    this.nodeIdMap.put(node, id);
    this.nodeJsonMap.put(node, obj);
    return obj;
  }

  private JsonObject createAndRegister(final ElkPort port) {
    final JsonObject obj = this._jsonAdapter.newJsonObject();
    String id = port.getIdentifier();
    if ((id == null)) {
      id = ("p" + Integer.valueOf(this.portIdCounter));
      this.portIdCounter = (this.portIdCounter + 1);
    }
    id = this.assertUnique(id, this.portIdMap.inverse());
    obj.addProperty("id", id);
    this.portIdMap.put(port, id);
    this.portJsonMap.put(port, obj);
    return obj;
  }

  private JsonObject createAndRegister(final ElkEdge edge) {
    final JsonObject obj = this._jsonAdapter.newJsonObject();
    String id = edge.getIdentifier();
    if ((id == null)) {
      id = ("e" + Integer.valueOf(this.edgeIdCounter));
      this.edgeIdCounter = (this.edgeIdCounter + 1);
    }
    id = this.assertUnique(id, this.edgeIdMap.inverse());
    obj.addProperty("id", id);
    this.edgeIdMap.put(edge, id);
    this.edgeJsonMap.put(edge, obj);
    return obj;
  }

  private JsonObject createAndRegister(final ElkEdgeSection section) {
    final JsonObject obj = this._jsonAdapter.newJsonObject();
    String id = section.getIdentifier();
    if ((id == null)) {
      id = ("s" + Integer.valueOf(this.edgeSectionIdCounter));
      this.edgeSectionIdCounter = (this.edgeSectionIdCounter + 1);
    }
    id = this.assertUnique(id, this.edgeSectionIdMap.inverse());
    obj.addProperty("id", id);
    this.edgeSectionIdMap.put(section, id);
    this.edgeSectionJsonMap.put(section, obj);
    return obj;
  }

  private String _idByElement(final ElkNode node) {
    return this.nodeIdMap.get(node);
  }

  private String _idByElement(final ElkPort port) {
    return this.portIdMap.get(port);
  }

  private String _idByElement(final ElkEdgeSection section) {
    return this.edgeSectionIdMap.get(section);
  }

  private String getShortOptionKey(final String fullId) {
    final LayoutOptionData option = LayoutMetaDataService.getInstance().getOptionDataBySuffix(fullId);
    if ((option == null)) {
      return fullId;
    }
    final Iterable<String> idSplit = Splitter.on(".").split(option.getId());
    boolean foundMatch = false;
    int _size = IterableExtensions.size(idSplit);
    int i = (_size - 1);
    if (((i >= 1) && Objects.equal(option.getGroup(), ((Object[])Conversions.unwrapArray(idSplit, Object.class))[(i - 1)]))) {
      i--;
    }
    while (((i >= 0) && (!foundMatch))) {
      {
        final Iterable<String> suffix = IterableExtensions.<String>drop(idSplit, i);
        LayoutOptionData _optionDataBySuffix = LayoutMetaDataService.getInstance().getOptionDataBySuffix(IterableExtensions.join(suffix, "."));
        boolean _tripleNotEquals = (_optionDataBySuffix != null);
        if (_tripleNotEquals) {
          foundMatch = true;
        } else {
          i--;
        }
      }
    }
    if (foundMatch) {
      return IterableExtensions.join(IterableExtensions.<String>drop(idSplit, i), ".");
    } else {
      return option.getId();
    }
  }

  public <T extends Object> Iterator<T> emptyIfNull(final Iterator<T> iterator) {
    Iterator<T> _xifexpression = null;
    if ((iterator == null)) {
      return Collections.<T>emptyIterator();
    } else {
      _xifexpression = iterator;
    }
    return _xifexpression;
  }

  private boolean isKnown(final IProperty<?> property) {
    LayoutOptionData _optionDataBySuffix = LayoutMetaDataService.getInstance().getOptionDataBySuffix(property.getId());
    return (_optionDataBySuffix != null);
  }

  private final Random RANDOM = new Random();

  private String sixDigitRandomNumber() {
    int _nextInt = this.RANDOM.nextInt(1000000);
    return (Integer.valueOf(_nextInt) + "");
  }

  private String padZeroes(final String s, final int length) {
    String tmp = s;
    while ((tmp.length() < length)) {
      tmp = ("0" + tmp);
    }
    return tmp;
  }

  private String assertUnique(final String id, final Map<String, ?> map) {
    String tmp = id;
    while (map.containsKey(tmp)) {
      String _padZeroes = this.padZeroes(this.sixDigitRandomNumber(), 6);
      String _plus = ((id + "_g") + _padZeroes);
      tmp = _plus;
    }
    return tmp;
  }

  private String idByElement(final EMapPropertyHolder node) {
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
