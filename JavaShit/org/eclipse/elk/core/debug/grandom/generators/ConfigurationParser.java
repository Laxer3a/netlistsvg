/**
 * Copyright (c) 2016, 2020 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.core.debug.grandom.generators;

import java.util.Random;
import org.eclipse.elk.core.debug.grandom.gRandom.Configuration;
import org.eclipse.elk.core.debug.grandom.gRandom.ConstraintType;
import org.eclipse.elk.core.debug.grandom.gRandom.DoubleQuantity;
import org.eclipse.elk.core.debug.grandom.gRandom.Edges;
import org.eclipse.elk.core.debug.grandom.gRandom.Flow;
import org.eclipse.elk.core.debug.grandom.gRandom.FlowType;
import org.eclipse.elk.core.debug.grandom.gRandom.Form;
import org.eclipse.elk.core.debug.grandom.gRandom.Hierarchy;
import org.eclipse.elk.core.debug.grandom.gRandom.Nodes;
import org.eclipse.elk.core.debug.grandom.gRandom.Ports;
import org.eclipse.elk.core.debug.grandom.gRandom.Side;
import org.eclipse.elk.core.debug.grandom.gRandom.Size;
import org.eclipse.elk.core.options.PortConstraints;
import org.eclipse.elk.graph.properties.IProperty;
import org.eclipse.elk.graph.properties.MapPropertyHolder;
import org.eclipse.emf.common.util.EList;

/**
 * Knows how to turn a {@link Configuration} object into a {@link GeneratorOptions}
 * object.
 */
@SuppressWarnings("all")
public class ConfigurationParser {
  public static GeneratorOptions parse(final Configuration config, final Random r) {
    GeneratorOptions _xblockexpression = null;
    {
      final GeneratorOptions genOpt = new GeneratorOptions();
      ConfigurationParser.setGraphType(config, genOpt);
      ConfigurationParser.nodes(config.getNodes(), r, genOpt);
      ConfigurationParser.edges(config, genOpt, r);
      ConfigurationParser.hierarchy(config, genOpt, r);
      ConfigurationParser.setQuantities(genOpt, config.getFraction(), GeneratorOptions.PARTITION_FRAC);
      ConfigurationParser.<Integer>setIfExists(genOpt, config.getMaxWidth(), GeneratorOptions.MAX_WIDTH);
      ConfigurationParser.<Integer>setIfExists(genOpt, config.getMaxDegree(), GeneratorOptions.MAX_DEGREE);
      _xblockexpression = genOpt;
    }
    return _xblockexpression;
  }

  private static MapPropertyHolder hierarchy(final Configuration configuration, final GeneratorOptions options, final Random random) {
    MapPropertyHolder _xblockexpression = null;
    {
      final Hierarchy hierarchy = configuration.getHierarchy();
      MapPropertyHolder _xifexpression = null;
      boolean _exists = ConfigurationParser.exists(hierarchy);
      if (_exists) {
        MapPropertyHolder _xblockexpression_1 = null;
        {
          final DoubleQuantity hierarch = hierarchy.getNumHierarchNodes();
          ConfigurationParser.setQuantities(options, hierarchy.getCrossHierarchRel(), GeneratorOptions.EXACT_RELATIVE_HIER);
          options.<Boolean>setProperty(GeneratorOptions.SMALL_HIERARCHY, Boolean.valueOf(ConfigurationParser.exists(hierarch)));
          ConfigurationParser.setQuantities(options, hierarchy.getLevels(), GeneratorOptions.MAX_HIERARCHY_LEVEL);
          ConfigurationParser.setQuantities(options, hierarch, GeneratorOptions.NUMBER_HIERARCHICAL_NODES);
          _xblockexpression_1 = ConfigurationParser.setQuantities(options, hierarchy.getEdges(), GeneratorOptions.CROSS_HIER);
        }
        _xifexpression = _xblockexpression_1;
      }
      _xblockexpression = _xifexpression;
    }
    return _xblockexpression;
  }

  private static MapPropertyHolder edges(final Configuration config, final GeneratorOptions genOpt, final Random r) {
    MapPropertyHolder _xblockexpression = null;
    {
      final Edges edges = config.getEdges();
      MapPropertyHolder _xifexpression = null;
      boolean _exists = ConfigurationParser.exists(edges);
      if (_exists) {
        MapPropertyHolder _xblockexpression_1 = null;
        {
          boolean _isTotal = edges.isTotal();
          if (_isTotal) {
            genOpt.<GeneratorOptions.EdgeDetermination>setProperty(GeneratorOptions.EDGE_DETERMINATION, GeneratorOptions.EdgeDetermination.ABSOLUTE);
            ConfigurationParser.setQuantities(genOpt, edges.getNEdges(), GeneratorOptions.EDGES_ABSOLUTE);
          } else {
            boolean _isDensity = edges.isDensity();
            if (_isDensity) {
              ConfigurationParser.setQuantities(genOpt, edges.getNEdges(), GeneratorOptions.DENSITY);
              genOpt.<GeneratorOptions.EdgeDetermination>setProperty(GeneratorOptions.EDGE_DETERMINATION, GeneratorOptions.EdgeDetermination.DENSITY);
            } else {
              boolean _isRelative = edges.isRelative();
              if (_isRelative) {
                genOpt.<GeneratorOptions.EdgeDetermination>setProperty(GeneratorOptions.EDGE_DETERMINATION, GeneratorOptions.EdgeDetermination.RELATIVE);
                ConfigurationParser.setQuantities(genOpt, edges.getNEdges(), GeneratorOptions.RELATIVE_EDGES);
              } else {
                genOpt.<GeneratorOptions.EdgeDetermination>setProperty(GeneratorOptions.EDGE_DETERMINATION, GeneratorOptions.EdgeDetermination.OUTGOING);
                ConfigurationParser.setQuantities(genOpt, edges.getNEdges(), GeneratorOptions.OUTGOING_EDGES);
              }
            }
          }
          genOpt.<Boolean>setProperty(GeneratorOptions.EDGE_LABELS, Boolean.valueOf(edges.isLabels()));
          _xblockexpression_1 = genOpt.<Boolean>setProperty(GeneratorOptions.SELF_LOOPS, Boolean.valueOf(edges.isSelfLoops()));
        }
        _xifexpression = _xblockexpression_1;
      }
      _xblockexpression = _xifexpression;
    }
    return _xblockexpression;
  }

  private static MapPropertyHolder nodes(final Nodes nodes, final Random r, final GeneratorOptions genOpt) {
    MapPropertyHolder _xifexpression = null;
    boolean _exists = ConfigurationParser.exists(nodes);
    if (_exists) {
      MapPropertyHolder _xblockexpression = null;
      {
        ConfigurationParser.setQuantities(genOpt, nodes.getNNodes(), GeneratorOptions.NUMBER_OF_NODES);
        genOpt.<Boolean>setProperty(GeneratorOptions.CREATE_NODE_LABELS, Boolean.valueOf(nodes.isLabels()));
        ConfigurationParser.ports(nodes, genOpt, r);
        ConfigurationParser.size(nodes, r, genOpt);
        boolean _isRemoveIsolated = nodes.isRemoveIsolated();
        boolean _not = (!_isRemoveIsolated);
        _xblockexpression = genOpt.<Boolean>setProperty(GeneratorOptions.ISOLATED_NODES, Boolean.valueOf(_not));
      }
      _xifexpression = _xblockexpression;
    }
    return _xifexpression;
  }

  private static MapPropertyHolder size(final Nodes nodes, final Random r, final GeneratorOptions genOpt) {
    MapPropertyHolder _xblockexpression = null;
    {
      final Size size = nodes.getSize();
      MapPropertyHolder _xifexpression = null;
      boolean _exists = ConfigurationParser.exists(size);
      if (_exists) {
        MapPropertyHolder _xblockexpression_1 = null;
        {
          ConfigurationParser.setQuantities(genOpt, size.getWidth(), GeneratorOptions.NODE_WIDTH);
          _xblockexpression_1 = ConfigurationParser.setQuantities(genOpt, size.getHeight(), GeneratorOptions.NODE_HEIGHT);
        }
        _xifexpression = _xblockexpression_1;
      }
      _xblockexpression = _xifexpression;
    }
    return _xblockexpression;
  }

  private static MapPropertyHolder ports(final Nodes nodes, final GeneratorOptions genOpt, final Random r) {
    MapPropertyHolder _xblockexpression = null;
    {
      final Ports ports = nodes.getPorts();
      MapPropertyHolder _xifexpression = null;
      boolean _exists = ConfigurationParser.exists(ports);
      if (_exists) {
        MapPropertyHolder _xblockexpression_1 = null;
        {
          genOpt.<Boolean>setProperty(GeneratorOptions.ENABLE_PORTS, Boolean.valueOf(true));
          genOpt.<Boolean>setProperty(GeneratorOptions.CREATE_PORT_LABELS, Boolean.valueOf(ports.isLabels()));
          genOpt.<PortConstraints>setProperty(GeneratorOptions.PORT_CONSTRAINTS, ConfigurationParser.getConstraint(ports.getConstraint()));
          ConfigurationParser.setQuantities(genOpt, ports.getReUse(), GeneratorOptions.USE_EXISTING_PORTS_CHANCE);
          final EList<Flow> flows = ports.getFlow();
          boolean _exists_1 = ConfigurationParser.exists(flows);
          if (_exists_1) {
            for (final Flow f : flows) {
              ConfigurationParser.setFlowSide(genOpt, f.getFlowType(), f.getSide(), f.getAmount(), r);
            }
          }
          final Size size = ports.getSize();
          genOpt.<Boolean>setProperty(GeneratorOptions.SET_PORT_SIZE, Boolean.valueOf(true));
          MapPropertyHolder _xifexpression_1 = null;
          boolean _exists_2 = ConfigurationParser.exists(size);
          if (_exists_2) {
            MapPropertyHolder _xblockexpression_2 = null;
            {
              ConfigurationParser.setQuantities(genOpt, size.getHeight(), GeneratorOptions.PORT_HEIGHT);
              _xblockexpression_2 = ConfigurationParser.setQuantities(genOpt, size.getWidth(), GeneratorOptions.PORT_WIDTH);
            }
            _xifexpression_1 = _xblockexpression_2;
          }
          _xblockexpression_1 = _xifexpression_1;
        }
        _xifexpression = _xblockexpression_1;
      }
      _xblockexpression = _xifexpression;
    }
    return _xblockexpression;
  }

  private static MapPropertyHolder setQuantities(final GeneratorOptions genOpt, final DoubleQuantity quant, final IProperty<GeneratorOptions.RandVal> randomValue) {
    MapPropertyHolder _xifexpression = null;
    boolean _exists = ConfigurationParser.exists(quant);
    if (_exists) {
      _xifexpression = genOpt.<GeneratorOptions.RandVal>setProperty(randomValue, ConfigurationParser.toRandVal(quant));
    }
    return _xifexpression;
  }

  private static GeneratorOptions.RandVal toRandVal(final DoubleQuantity quant) {
    GeneratorOptions.RandVal _xifexpression = null;
    boolean _isMinMax = quant.isMinMax();
    if (_isMinMax) {
      _xifexpression = GeneratorOptions.RandVal.minMax((quant.getMin()).doubleValue(), (quant.getMax()).doubleValue());
    } else {
      GeneratorOptions.RandVal _xifexpression_1 = null;
      boolean _isGaussian = quant.isGaussian();
      if (_isGaussian) {
        _xifexpression_1 = GeneratorOptions.RandVal.gaussian((quant.getMean()).doubleValue(), (quant.getStddv()).doubleValue());
      } else {
        _xifexpression_1 = GeneratorOptions.RandVal.exact((quant.getQuant()).doubleValue());
      }
      _xifexpression = _xifexpression_1;
    }
    return _xifexpression;
  }

  private static MapPropertyHolder setFlowSide(final GeneratorOptions options, final FlowType type, final Side side, final DoubleQuantity quant, final Random r) {
    MapPropertyHolder _xblockexpression = null;
    {
      final int amount = ConfigurationParser.toRandVal(quant).intVal(r);
      MapPropertyHolder _switchResult = null;
      if (type != null) {
        switch (type) {
          case INCOMING:
            MapPropertyHolder _switchResult_1 = null;
            if (side != null) {
              switch (side) {
                case EAST:
                  _switchResult_1 = options.<Integer>setProperty(GeneratorOptions.INCOMING_EAST_SIDE, Integer.valueOf(amount));
                  break;
                case NORTH:
                  _switchResult_1 = options.<Integer>setProperty(GeneratorOptions.INCOMING_NORTH_SIDE, Integer.valueOf(amount));
                  break;
                case SOUTH:
                  _switchResult_1 = options.<Integer>setProperty(GeneratorOptions.INCOMING_SOUTH_SIDE, Integer.valueOf(amount));
                  break;
                case WEST:
                  _switchResult_1 = options.<Integer>setProperty(GeneratorOptions.INCOMING_WEST_SIDE, Integer.valueOf(amount));
                  break;
                default:
                  break;
              }
            }
            _switchResult = _switchResult_1;
            break;
          case OUTGOING:
            MapPropertyHolder _switchResult_2 = null;
            if (side != null) {
              switch (side) {
                case EAST:
                  _switchResult_2 = options.<Integer>setProperty(GeneratorOptions.OUTGOING_EAST_SIDE, Integer.valueOf(amount));
                  break;
                case NORTH:
                  _switchResult_2 = options.<Integer>setProperty(GeneratorOptions.OUTGOING_NORTH_SIDE, Integer.valueOf(amount));
                  break;
                case SOUTH:
                  _switchResult_2 = options.<Integer>setProperty(GeneratorOptions.OUTGOING_SOUTH_SIDE, Integer.valueOf(amount));
                  break;
                case WEST:
                  _switchResult_2 = options.<Integer>setProperty(GeneratorOptions.OUTGOING_WEST_SIDE, Integer.valueOf(amount));
                  break;
                default:
                  break;
              }
            }
            _switchResult = _switchResult_2;
            break;
          default:
            break;
        }
      }
      _xblockexpression = _switchResult;
    }
    return _xblockexpression;
  }

  private static PortConstraints getConstraint(final ConstraintType constraint) {
    if (constraint != null) {
      switch (constraint) {
        case FREE:
          return PortConstraints.FREE;
        case ORDER:
          return PortConstraints.FIXED_ORDER;
        case POSITION:
          return PortConstraints.FIXED_POS;
        case SIDE:
          return PortConstraints.FIXED_SIDE;
        case RATIO:
          return PortConstraints.FIXED_RATIO;
        default:
          break;
      }
    }
    return PortConstraints.UNDEFINED;
  }

  private static void setGraphType(final Configuration configuration, final GeneratorOptions options) {
    Form _form = configuration.getForm();
    if (_form != null) {
      switch (_form) {
        case ACYCLIC:
          options.<GeneratorOptions.GraphType>setProperty(GeneratorOptions.GRAPH_TYPE, GeneratorOptions.GraphType.ACYCLIC_NO_TRANSITIVE_EDGES);
          break;
        case BICONNECTED:
          options.<GeneratorOptions.GraphType>setProperty(GeneratorOptions.GRAPH_TYPE, GeneratorOptions.GraphType.BICONNECTED);
          break;
        case BIPARTITE:
          options.<GeneratorOptions.GraphType>setProperty(GeneratorOptions.GRAPH_TYPE, GeneratorOptions.GraphType.BIPARTITE);
          break;
        case CUSTOM:
          options.<GeneratorOptions.GraphType>setProperty(GeneratorOptions.GRAPH_TYPE, GeneratorOptions.GraphType.CUSTOM);
          break;
        case TREES:
          options.<GeneratorOptions.GraphType>setProperty(GeneratorOptions.GRAPH_TYPE, GeneratorOptions.GraphType.TREE);
          break;
        case TRICONNECTED:
          options.<GeneratorOptions.GraphType>setProperty(GeneratorOptions.GRAPH_TYPE, GeneratorOptions.GraphType.TRICONNECTED);
          break;
        default:
          break;
      }
    }
  }

  private static boolean exists(final Object o) {
    return (o != null);
  }

  private static <T extends Object> boolean setIfExists(final GeneratorOptions genOpt, final T value, final IProperty<T> property) {
    boolean _exists = ConfigurationParser.exists(value);
    if (_exists) {
      genOpt.<T>setProperty(property, value);
      return true;
    }
    return false;
  }
}
