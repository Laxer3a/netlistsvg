/**
 * Copyright (c) 2016 TypeFox GmbH (http://www.typefox.io) and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.text.validation;

import com.google.common.base.Objects;
import com.google.inject.Inject;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import org.eclipse.elk.core.data.LayoutAlgorithmData;
import org.eclipse.elk.core.data.LayoutMetaDataService;
import org.eclipse.elk.core.data.LayoutOptionData;
import org.eclipse.elk.core.options.CoreOptions;
import org.eclipse.elk.core.validation.GraphIssue;
import org.eclipse.elk.core.validation.LayoutOptionValidator;
import org.eclipse.elk.graph.EMapPropertyHolder;
import org.eclipse.elk.graph.ElkConnectableShape;
import org.eclipse.elk.graph.ElkEdge;
import org.eclipse.elk.graph.ElkEdgeSection;
import org.eclipse.elk.graph.ElkGraphElement;
import org.eclipse.elk.graph.ElkGraphPackage;
import org.eclipse.elk.graph.ElkLabel;
import org.eclipse.elk.graph.ElkNode;
import org.eclipse.elk.graph.ElkPort;
import org.eclipse.elk.graph.impl.ElkPropertyToValueMapEntryImpl;
import org.eclipse.elk.graph.properties.IProperty;
import org.eclipse.elk.graph.properties.IPropertyValueProxy;
import org.eclipse.elk.graph.text.ElkGraphTextUtil;
import org.eclipse.elk.graph.util.ElkGraphUtil;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.EMap;
import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.xtext.EcoreUtil2;
import org.eclipse.xtext.validation.Check;
import org.eclipse.xtext.xbase.lib.CollectionLiterals;
import org.eclipse.xtext.xbase.lib.Conversions;
import org.eclipse.xtext.xbase.lib.Functions.Function1;
import org.eclipse.xtext.xbase.lib.IterableExtensions;

/**
 * Custom validation rules for the ElkGraph language.
 */
@SuppressWarnings("all")
public class ElkGraphValidator extends AbstractElkGraphValidator {
  @Inject
  private LayoutOptionValidator layoutOptionValidator;

  @Check
  public void checkPropertyKey(final ElkPropertyToValueMapEntryImpl entry) {
    final LayoutOptionData optionData = LayoutMetaDataService.getInstance().getOptionData(entry.getKey().getId());
    if ((optionData == null)) {
      String _id = entry.getKey().getId();
      String _plus = ("\'" + _id);
      String _plus_1 = (_plus + "\' is not a layout option.");
      this.warning(_plus_1, ElkGraphPackage.Literals.ELK_PROPERTY_TO_VALUE_MAP_ENTRY__KEY);
    }
  }

  @Check
  public void checkPropertyValue(final ElkPropertyToValueMapEntryImpl entry) {
    final LayoutOptionData option = this.toLayoutOption(entry.getKey());
    if ((option != null)) {
      final EObject container = entry.eContainer();
      if ((container instanceof ElkGraphElement)) {
        boolean _matched = false;
        if (container instanceof ElkNode) {
          _matched=true;
          this.checkOptionTarget(option, LayoutOptionData.Target.NODES, LayoutOptionData.Target.PARENTS);
        }
        if (!_matched) {
          if (container instanceof ElkEdge) {
            _matched=true;
            this.checkOptionTarget(option, LayoutOptionData.Target.EDGES);
          }
        }
        if (!_matched) {
          if (container instanceof ElkPort) {
            _matched=true;
            this.checkOptionTarget(option, LayoutOptionData.Target.PORTS);
          }
        }
        if (!_matched) {
          if (container instanceof ElkLabel) {
            _matched=true;
            this.checkOptionTarget(option, LayoutOptionData.Target.LABELS);
          }
        }
        if (((container instanceof ElkNode) && option.getTargets().contains(LayoutOptionData.Target.NODES))) {
          this.checkAlgorithmSupport(option, ((ElkNode) container).getParent());
        } else {
          this.checkAlgorithmSupport(option, ((ElkGraphElement)container));
        }
      }
      Object value = entry.getValue();
      if ((value instanceof IPropertyValueProxy)) {
        value = ((IPropertyValueProxy)value).<Object>resolveValue(option);
        if ((value == null)) {
          LayoutOptionData.Type _type = option.getType();
          if (_type != null) {
            switch (_type) {
              case STRING:
                this.expectPropertyType(String.class);
                break;
              case BOOLEAN:
                this.expectPropertyType(Boolean.class);
                break;
              case INT:
                this.expectPropertyType(Integer.class);
                break;
              case DOUBLE:
                this.expectPropertyType(Double.class);
                break;
              case ENUMSET:
                this.expectPropertyType(EnumSet.class);
                break;
              default:
                Class<?> _optionClass = option.getOptionClass();
                boolean _tripleNotEquals = (_optionClass != null);
                if (_tripleNotEquals) {
                  this.expectPropertyType(option.getOptionClass());
                }
                break;
            }
          } else {
            Class<?> _optionClass = option.getOptionClass();
            boolean _tripleNotEquals = (_optionClass != null);
            if (_tripleNotEquals) {
              this.expectPropertyType(option.getOptionClass());
            }
          }
        } else {
          entry.setValue(value);
        }
      }
      if ((value != null)) {
        final List<GraphIssue> issues = this.layoutOptionValidator.checkProperty(option, value, 
          EcoreUtil2.<ElkGraphElement>getContainerOfType(entry, ElkGraphElement.class));
        for (final GraphIssue issue : issues) {
          GraphIssue.Severity _severity = issue.getSeverity();
          if (_severity != null) {
            switch (_severity) {
              case ERROR:
                this.error(issue.getMessage(), ElkGraphPackage.Literals.ELK_PROPERTY_TO_VALUE_MAP_ENTRY__VALUE);
                break;
              case WARNING:
                this.warning(issue.getMessage(), ElkGraphPackage.Literals.ELK_PROPERTY_TO_VALUE_MAP_ENTRY__VALUE);
                break;
              default:
                break;
            }
          }
        }
        boolean _equals = Objects.equal(CoreOptions.ALGORITHM, option);
        if (_equals) {
          LayoutAlgorithmData _algorithmDataBySuffix = LayoutMetaDataService.getInstance().getAlgorithmDataBySuffix(((String) value));
          boolean _tripleEquals = (_algorithmDataBySuffix == null);
          if (_tripleEquals) {
            this.error((("No layout algorithm with identifier \'" + value) + "\' can be found."), 
              ElkGraphPackage.Literals.ELK_PROPERTY_TO_VALUE_MAP_ENTRY__VALUE);
          }
        }
      }
    }
  }

  private LayoutOptionData toLayoutOption(final IProperty<?> property) {
    if ((property instanceof LayoutOptionData)) {
      return ((LayoutOptionData)property);
    } else {
      if ((property != null)) {
        return LayoutMetaDataService.getInstance().getOptionData(property.getId());
      }
    }
    return null;
  }

  private void checkOptionTarget(final LayoutOptionData option, final LayoutOptionData.Target... targetTypes) {
    final Function1<LayoutOptionData.Target, Boolean> _function = new Function1<LayoutOptionData.Target, Boolean>() {
      @Override
      public Boolean apply(final LayoutOptionData.Target it) {
        return Boolean.valueOf(option.getTargets().contains(it));
      }
    };
    boolean _exists = IterableExtensions.<LayoutOptionData.Target>exists(((Iterable<LayoutOptionData.Target>)Conversions.doWrapArray(targetTypes)), _function);
    boolean _not = (!_exists);
    if (_not) {
      String _id = option.getId();
      String _plus = ("The layout option \'" + _id);
      String _plus_1 = (_plus + "\' is not applicable to ");
      String _lowerCase = IterableExtensions.<LayoutOptionData.Target>head(((Iterable<LayoutOptionData.Target>)Conversions.doWrapArray(targetTypes))).toString().toLowerCase();
      String _plus_2 = (_plus_1 + _lowerCase);
      String _plus_3 = (_plus_2 + ".");
      this.warning(_plus_3, 
        ElkGraphPackage.Literals.ELK_PROPERTY_TO_VALUE_MAP_ENTRY__KEY, IssueCodes.OPTION_NOT_APPLICABLE);
    }
  }

  private void checkAlgorithmSupport(final LayoutOptionData option, final ElkGraphElement element) {
    boolean _notEquals = (!Objects.equal(option, CoreOptions.ALGORITHM));
    if (_notEquals) {
      LayoutAlgorithmData algorithm = ElkGraphTextUtil.getAlgorithm(element);
      if (((algorithm != null) && (!algorithm.knowsOption(option)))) {
        boolean foundMatch = false;
        ElkNode parent = EcoreUtil2.<ElkNode>getContainerOfType(element, ElkNode.class);
        while (((parent != null) && (!foundMatch))) {
          {
            algorithm = ElkGraphTextUtil.getAlgorithm(parent);
            foundMatch = ((algorithm != null) && algorithm.knowsOption(option));
            parent = parent.getParent();
          }
        }
        if ((!foundMatch)) {
          String _id = algorithm.getId();
          String _plus = ("The algorithm \'" + _id);
          String _plus_1 = (_plus + "\' does not support the option \'");
          String _id_1 = option.getId();
          String _plus_2 = (_plus_1 + _id_1);
          String _plus_3 = (_plus_2 + "\'.");
          this.warning(_plus_3, 
            ElkGraphPackage.Literals.ELK_PROPERTY_TO_VALUE_MAP_ENTRY__KEY);
        }
      }
    }
  }

  private void expectPropertyType(final Class<?> type) {
    String _simpleName = type.getSimpleName();
    String _plus = ("Expected value of type " + _simpleName);
    String _plus_1 = (_plus + ".");
    this.error(_plus_1, ElkGraphPackage.Literals.ELK_PROPERTY_TO_VALUE_MAP_ENTRY__VALUE);
  }

  @Check
  public void checkUniqueProperties(final EMapPropertyHolder propertyHolder) {
    final HashMap<IProperty<?>, Map.Entry<IProperty<?>, Object>> usedProperties = CollectionLiterals.<IProperty<?>, Map.Entry<IProperty<?>, Object>>newHashMap();
    EMap<IProperty<?>, Object> _properties = propertyHolder.getProperties();
    for (final Map.Entry<IProperty<?>, Object> entry : _properties) {
      {
        final IProperty<?> property = entry.getKey();
        boolean _containsKey = usedProperties.containsKey(property);
        if (_containsKey) {
          this.propertyAlreadyAssigned(entry);
          final Map.Entry<IProperty<?>, Object> other = usedProperties.get(property);
          if ((other != null)) {
            this.propertyAlreadyAssigned(other);
            usedProperties.put(property, null);
          }
        } else {
          usedProperties.put(property, entry);
        }
      }
    }
  }

  private void propertyAlreadyAssigned(final Map.Entry<IProperty<?>, Object> entry) {
    if ((entry instanceof ElkPropertyToValueMapEntryImpl)) {
      this.error("Property is already assigned.", ((EObject)entry), ElkGraphPackage.Literals.ELK_PROPERTY_TO_VALUE_MAP_ENTRY__KEY);
    }
  }

  @Check
  public void checkUniqueNames(final ElkGraphElement element) {
    final HashMap<String, EObject> usedNames = CollectionLiterals.<String, EObject>newHashMap();
    EList<EObject> _eContents = element.eContents();
    for (final EObject object : _eContents) {
      {
        String _switchResult = null;
        boolean _matched = false;
        if (object instanceof ElkGraphElement) {
          _matched=true;
          _switchResult = ((ElkGraphElement)object).getIdentifier();
        }
        if (!_matched) {
          if (object instanceof ElkEdgeSection) {
            _matched=true;
            _switchResult = ((ElkEdgeSection)object).getIdentifier();
          }
        }
        final String name = _switchResult;
        if ((name != null)) {
          boolean _containsKey = usedNames.containsKey(name);
          if (_containsKey) {
            this.nameAlreadyUsed(object);
            final EObject other = usedNames.get(name);
            if ((other != null)) {
              this.nameAlreadyUsed(other);
              usedNames.put(name, null);
            }
          } else {
            usedNames.put(name, object);
          }
        }
      }
    }
  }

  private void nameAlreadyUsed(final EObject object) {
    EAttribute _switchResult = null;
    boolean _matched = false;
    if (object instanceof ElkGraphElement) {
      _matched=true;
      _switchResult = ElkGraphPackage.Literals.ELK_GRAPH_ELEMENT__IDENTIFIER;
    }
    if (!_matched) {
      if (object instanceof ElkEdgeSection) {
        _matched=true;
        _switchResult = ElkGraphPackage.Literals.ELK_EDGE_SECTION__IDENTIFIER;
      }
    }
    final EAttribute feature = _switchResult;
    this.error("Identifier is already used.", object, feature);
  }

  @Check
  public void checkEdgeContainer(final ElkEdge edge) {
    boolean _not = (!(edge.getSources().isEmpty() && edge.getTargets().isEmpty()));
    if (_not) {
      final ElkNode bestContainer = ElkGraphUtil.findBestEdgeContainment(edge);
      if (((bestContainer != null) && (!Objects.equal(bestContainer, edge.getContainingNode())))) {
        ElkNode _parent = bestContainer.getParent();
        boolean _tripleEquals = (_parent == null);
        if (_tripleEquals) {
          this.warning("This edge should be declared in the root node of this graph.", null);
        } else {
          String _identifier = bestContainer.getIdentifier();
          String _plus = ("This edge should be declared in node " + _identifier);
          String _plus_1 = (_plus + ".");
          this.warning(_plus_1, null);
        }
      }
    }
  }

  @Check
  public void checkEdgeSection(final ElkEdgeSection edgeSection) {
    final ElkEdge edge = edgeSection.getParent();
    final ElkConnectableShape incomingShape = edgeSection.getIncomingShape();
    if (((incomingShape != null) && (!incomingShape.eIsProxy()))) {
      boolean _contains = edge.getSources().contains(incomingShape);
      boolean _not = (!_contains);
      if (_not) {
        String _name = incomingShape.eClass().getName();
        String _plus = ("The " + _name);
        String _plus_1 = (_plus + " ");
        String _identifier = incomingShape.getIdentifier();
        String _plus_2 = (_plus_1 + _identifier);
        String _plus_3 = (_plus_2 + " is not a source of this edge.");
        this.error(_plus_3, 
          ElkGraphPackage.Literals.ELK_EDGE_SECTION__INCOMING_SHAPE);
      }
      boolean _isEmpty = edgeSection.getIncomingSections().isEmpty();
      boolean _not_1 = (!_isEmpty);
      if (_not_1) {
        String _name_1 = incomingShape.eClass().getName();
        String _plus_4 = ("An edge section cannot be connected to an " + _name_1);
        String _plus_5 = (_plus_4 + " and other sections at the same time.");
        this.error(_plus_5, 
          ElkGraphPackage.Literals.ELK_EDGE_SECTION__INCOMING_SHAPE);
      }
    }
    final ElkConnectableShape outgoingShape = edgeSection.getOutgoingShape();
    if (((outgoingShape != null) && (!outgoingShape.eIsProxy()))) {
      boolean _contains_1 = edge.getTargets().contains(outgoingShape);
      boolean _not_2 = (!_contains_1);
      if (_not_2) {
        String _name_2 = outgoingShape.eClass().getName();
        String _plus_6 = ("The " + _name_2);
        String _plus_7 = (_plus_6 + " ");
        String _identifier_1 = outgoingShape.getIdentifier();
        String _plus_8 = (_plus_7 + _identifier_1);
        String _plus_9 = (_plus_8 + " is not a target of this edge.");
        this.error(_plus_9, 
          ElkGraphPackage.Literals.ELK_EDGE_SECTION__OUTGOING_SHAPE);
      }
      boolean _isEmpty_1 = edgeSection.getOutgoingSections().isEmpty();
      boolean _not_3 = (!_isEmpty_1);
      if (_not_3) {
        String _name_3 = outgoingShape.eClass().getName();
        String _plus_10 = ("An edge section cannot be connected to an " + _name_3);
        String _plus_11 = (_plus_10 + " and other sections at the same time.");
        this.error(_plus_11, 
          ElkGraphPackage.Literals.ELK_EDGE_SECTION__OUTGOING_SHAPE);
      }
    }
  }
}
