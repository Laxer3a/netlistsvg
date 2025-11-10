/**
 * Copyright (c) 2017 TypeFox GmbH (http://www.typefox.io) and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.text.formatting2;

import com.google.inject.Inject;
import java.util.Arrays;
import java.util.Map;
import java.util.function.Consumer;
import org.eclipse.elk.graph.EMapPropertyHolder;
import org.eclipse.elk.graph.ElkBendPoint;
import org.eclipse.elk.graph.ElkEdge;
import org.eclipse.elk.graph.ElkEdgeSection;
import org.eclipse.elk.graph.ElkGraphPackage;
import org.eclipse.elk.graph.ElkLabel;
import org.eclipse.elk.graph.ElkNode;
import org.eclipse.elk.graph.ElkPort;
import org.eclipse.elk.graph.ElkShape;
import org.eclipse.elk.graph.impl.ElkPropertyToValueMapEntryImpl;
import org.eclipse.elk.graph.properties.IProperty;
import org.eclipse.elk.graph.text.services.ElkGraphGrammarAccess;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.EMap;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.xtext.formatting2.AbstractFormatter2;
import org.eclipse.xtext.formatting2.IFormattableDocument;
import org.eclipse.xtext.formatting2.IHiddenRegionFormatter;
import org.eclipse.xtext.formatting2.regionaccess.ISemanticRegion;
import org.eclipse.xtext.resource.XtextResource;
import org.eclipse.xtext.xbase.lib.Extension;
import org.eclipse.xtext.xbase.lib.IterableExtensions;
import org.eclipse.xtext.xbase.lib.Procedures.Procedure1;

@SuppressWarnings("all")
public class ElkGraphFormatter extends AbstractFormatter2 {
  private static final Procedure1<? super IHiddenRegionFormatter> no_space = new Procedure1<IHiddenRegionFormatter>() {
    @Override
    public void apply(final IHiddenRegionFormatter it) {
      it.noSpace();
    }
  };

  private static final Procedure1<? super IHiddenRegionFormatter> one_space = new Procedure1<IHiddenRegionFormatter>() {
    @Override
    public void apply(final IHiddenRegionFormatter it) {
      it.oneSpace();
    }
  };

  private static final Procedure1<? super IHiddenRegionFormatter> new_line = new Procedure1<IHiddenRegionFormatter>() {
    @Override
    public void apply(final IHiddenRegionFormatter it) {
      it.newLine();
    }
  };

  private static final Procedure1<? super IHiddenRegionFormatter> new_lines = new Procedure1<IHiddenRegionFormatter>() {
    @Override
    public void apply(final IHiddenRegionFormatter it) {
      it.setNewLines(1, 1, 2);
    }
  };

  @Inject
  @Extension
  private ElkGraphGrammarAccess _elkGraphGrammarAccess;

  protected void _format(final ElkNode node, @Extension final IFormattableDocument document) {
    ElkNode _parent = node.getParent();
    boolean _tripleEquals = (_parent == null);
    if (_tripleEquals) {
      document.append(this.textRegionExtensions.regionFor(node).keyword(this._elkGraphGrammarAccess.getRootNodeAccess().getGraphKeyword_1_0()), ElkGraphFormatter.one_space);
      document.append(this.textRegionExtensions.regionFor(node).assignment(this._elkGraphGrammarAccess.getRootNodeAccess().getIdentifierAssignment_1_1()), ElkGraphFormatter.new_lines);
    } else {
      document.append(this.textRegionExtensions.regionFor(node).keyword(this._elkGraphGrammarAccess.getElkNodeAccess().getNodeKeyword_0()), ElkGraphFormatter.one_space);
      final Procedure1<IHiddenRegionFormatter> _function = new Procedure1<IHiddenRegionFormatter>() {
        @Override
        public void apply(final IHiddenRegionFormatter it) {
          it.indent();
        }
      };
      document.<ISemanticRegion, ISemanticRegion>interior(
        document.append(document.prepend(this.textRegionExtensions.regionFor(node).keyword(this._elkGraphGrammarAccess.getElkNodeAccess().getLeftCurlyBracketKeyword_2_0()), ElkGraphFormatter.one_space), ElkGraphFormatter.new_line), 
        this.textRegionExtensions.regionFor(node).keyword(this._elkGraphGrammarAccess.getElkNodeAccess().getRightCurlyBracketKeyword_2_4()), _function);
    }
    final Consumer<ISemanticRegion> _function_1 = new Consumer<ISemanticRegion>() {
      @Override
      public void accept(final ISemanticRegion it) {
        document.append(document.prepend(it, ElkGraphFormatter.no_space), ElkGraphFormatter.one_space);
      }
    };
    this.textRegionExtensions.regionFor(node).keywords(":", ",").forEach(_function_1);
    this.formatShapeLayout(node, document);
    EMap<IProperty<?>, Object> _properties = node.getProperties();
    for (final Map.Entry<IProperty<?>, Object> property : _properties) {
      document.<Map.Entry<IProperty<?>, Object>>format(property);
    }
    EList<ElkNode> _children = node.getChildren();
    for (final ElkNode child : _children) {
      document.<ElkNode>format(document.<ElkNode>append(child, ElkGraphFormatter.new_lines));
    }
    EList<ElkEdge> _containedEdges = node.getContainedEdges();
    for (final ElkEdge edge : _containedEdges) {
      document.<ElkEdge>format(document.<ElkEdge>append(edge, ElkGraphFormatter.new_lines));
    }
    EList<ElkPort> _ports = node.getPorts();
    for (final ElkPort port : _ports) {
      document.<ElkPort>format(document.<ElkPort>append(port, ElkGraphFormatter.new_lines));
    }
    EList<ElkLabel> _labels = node.getLabels();
    for (final ElkLabel label : _labels) {
      document.<ElkLabel>format(document.<ElkLabel>append(label, ElkGraphFormatter.new_lines));
    }
  }

  protected void _format(final ElkEdge edge, @Extension final IFormattableDocument document) {
    document.append(this.textRegionExtensions.regionFor(edge).keyword(this._elkGraphGrammarAccess.getElkEdgeAccess().getEdgeKeyword_0()), ElkGraphFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(edge).keyword(this._elkGraphGrammarAccess.getElkEdgeAccess().getHyphenMinusGreaterThanSignKeyword_4()), ElkGraphFormatter.one_space), ElkGraphFormatter.one_space);
    final Consumer<ISemanticRegion> _function = new Consumer<ISemanticRegion>() {
      @Override
      public void accept(final ISemanticRegion it) {
        document.append(document.prepend(it, ElkGraphFormatter.no_space), ElkGraphFormatter.one_space);
      }
    };
    this.textRegionExtensions.regionFor(edge).keywords(":", ",").forEach(_function);
    final Procedure1<IHiddenRegionFormatter> _function_1 = new Procedure1<IHiddenRegionFormatter>() {
      @Override
      public void apply(final IHiddenRegionFormatter it) {
        it.indent();
      }
    };
    document.<ISemanticRegion, ISemanticRegion>interior(
      document.append(document.prepend(this.textRegionExtensions.regionFor(edge).keyword(this._elkGraphGrammarAccess.getElkEdgeAccess().getLeftCurlyBracketKeyword_7_0()), ElkGraphFormatter.one_space), ElkGraphFormatter.new_line), 
      this.textRegionExtensions.regionFor(edge).keyword(this._elkGraphGrammarAccess.getElkEdgeAccess().getRightCurlyBracketKeyword_7_4()), _function_1);
    this.formatEdgeLayout(edge, document);
    EList<ElkEdgeSection> _sections = edge.getSections();
    for (final ElkEdgeSection section : _sections) {
      {
        document.<ElkEdgeSection>format(section);
        String _identifier = section.getIdentifier();
        boolean _tripleNotEquals = (_identifier != null);
        if (_tripleNotEquals) {
          document.<ElkEdgeSection>append(section, ElkGraphFormatter.new_lines);
        }
      }
    }
    EMap<IProperty<?>, Object> _properties = edge.getProperties();
    for (final Map.Entry<IProperty<?>, Object> property : _properties) {
      document.<Map.Entry<IProperty<?>, Object>>format(property);
    }
    EList<ElkLabel> _labels = edge.getLabels();
    for (final ElkLabel label : _labels) {
      document.<ElkLabel>format(document.<ElkLabel>append(label, ElkGraphFormatter.new_lines));
    }
  }

  protected void _format(final ElkPort port, @Extension final IFormattableDocument document) {
    document.append(this.textRegionExtensions.regionFor(port).keyword(this._elkGraphGrammarAccess.getElkPortAccess().getPortKeyword_0()), ElkGraphFormatter.one_space);
    final Consumer<ISemanticRegion> _function = new Consumer<ISemanticRegion>() {
      @Override
      public void accept(final ISemanticRegion it) {
        document.append(document.prepend(it, ElkGraphFormatter.no_space), ElkGraphFormatter.one_space);
      }
    };
    this.textRegionExtensions.regionFor(port).keywords(":", ",").forEach(_function);
    final Procedure1<IHiddenRegionFormatter> _function_1 = new Procedure1<IHiddenRegionFormatter>() {
      @Override
      public void apply(final IHiddenRegionFormatter it) {
        it.indent();
      }
    };
    document.<ISemanticRegion, ISemanticRegion>interior(
      document.append(document.prepend(this.textRegionExtensions.regionFor(port).keyword(this._elkGraphGrammarAccess.getElkPortAccess().getLeftCurlyBracketKeyword_2_0()), ElkGraphFormatter.one_space), ElkGraphFormatter.new_line), 
      this.textRegionExtensions.regionFor(port).keyword(this._elkGraphGrammarAccess.getElkPortAccess().getRightCurlyBracketKeyword_2_4()), _function_1);
    this.formatShapeLayout(port, document);
    EMap<IProperty<?>, Object> _properties = port.getProperties();
    for (final Map.Entry<IProperty<?>, Object> property : _properties) {
      document.<Map.Entry<IProperty<?>, Object>>format(property);
    }
    EList<ElkLabel> _labels = port.getLabels();
    for (final ElkLabel label : _labels) {
      document.<ElkLabel>format(document.<ElkLabel>append(label, ElkGraphFormatter.new_lines));
    }
  }

  protected void _format(final ElkLabel label, @Extension final IFormattableDocument document) {
    document.append(this.textRegionExtensions.regionFor(label).keyword(this._elkGraphGrammarAccess.getElkLabelAccess().getLabelKeyword_0()), ElkGraphFormatter.one_space);
    final Consumer<ISemanticRegion> _function = new Consumer<ISemanticRegion>() {
      @Override
      public void accept(final ISemanticRegion it) {
        document.append(document.prepend(it, ElkGraphFormatter.no_space), ElkGraphFormatter.one_space);
      }
    };
    this.textRegionExtensions.regionFor(label).keywords(":", ",").forEach(_function);
    final Procedure1<IHiddenRegionFormatter> _function_1 = new Procedure1<IHiddenRegionFormatter>() {
      @Override
      public void apply(final IHiddenRegionFormatter it) {
        it.indent();
      }
    };
    document.<ISemanticRegion, ISemanticRegion>interior(
      document.append(document.prepend(this.textRegionExtensions.regionFor(label).keyword(this._elkGraphGrammarAccess.getElkLabelAccess().getLeftCurlyBracketKeyword_3_0()), ElkGraphFormatter.one_space), ElkGraphFormatter.new_line), 
      this.textRegionExtensions.regionFor(label).keyword(this._elkGraphGrammarAccess.getElkLabelAccess().getRightCurlyBracketKeyword_3_4()), _function_1);
    this.formatShapeLayout(label, document);
    EMap<IProperty<?>, Object> _properties = label.getProperties();
    for (final Map.Entry<IProperty<?>, Object> property : _properties) {
      document.<Map.Entry<IProperty<?>, Object>>format(property);
    }
    EList<ElkLabel> _labels = label.getLabels();
    for (final ElkLabel nestedLabel : _labels) {
      document.<ElkLabel>format(document.<ElkLabel>append(nestedLabel, ElkGraphFormatter.new_lines));
    }
  }

  protected void _format(final ElkEdgeSection section, @Extension final IFormattableDocument document) {
    final Consumer<ISemanticRegion> _function = new Consumer<ISemanticRegion>() {
      @Override
      public void accept(final ISemanticRegion it) {
        document.append(document.prepend(it, ElkGraphFormatter.no_space), ElkGraphFormatter.one_space);
      }
    };
    this.textRegionExtensions.regionFor(section).keywords(":", ",").forEach(_function);
    String _identifier = section.getIdentifier();
    boolean _tripleEquals = (_identifier == null);
    if (_tripleEquals) {
      document.append(this.textRegionExtensions.regionFor(section).assignment(this._elkGraphGrammarAccess.getElkSingleEdgeSectionAccess().getIncomingShapeAssignment_1_0_0_2()), ElkGraphFormatter.new_line);
      document.append(this.textRegionExtensions.regionFor(section).assignment(this._elkGraphGrammarAccess.getElkSingleEdgeSectionAccess().getOutgoingShapeAssignment_1_0_1_2()), ElkGraphFormatter.new_line);
      document.append(this.textRegionExtensions.regionFor(section).assignment(this._elkGraphGrammarAccess.getElkSingleEdgeSectionAccess().getStartYAssignment_1_0_2_4()), ElkGraphFormatter.new_line);
      document.append(this.textRegionExtensions.regionFor(section).assignment(this._elkGraphGrammarAccess.getElkSingleEdgeSectionAccess().getEndYAssignment_1_0_3_4()), ElkGraphFormatter.new_line);
      final Consumer<ISemanticRegion> _function_1 = new Consumer<ISemanticRegion>() {
        @Override
        public void accept(final ISemanticRegion it) {
          document.append(document.prepend(it, ElkGraphFormatter.one_space), ElkGraphFormatter.one_space);
        }
      };
      this.textRegionExtensions.regionFor(section).keywords(this._elkGraphGrammarAccess.getElkSingleEdgeSectionAccess().getVerticalLineKeyword_1_1_3_0()).forEach(_function_1);
    } else {
      document.append(this.textRegionExtensions.regionFor(section).keyword(this._elkGraphGrammarAccess.getElkEdgeSectionAccess().getSectionKeyword_0()), ElkGraphFormatter.one_space);
      document.append(document.prepend(this.textRegionExtensions.regionFor(section).keyword(this._elkGraphGrammarAccess.getElkEdgeSectionAccess().getHyphenMinusGreaterThanSignKeyword_2_0()), ElkGraphFormatter.one_space), ElkGraphFormatter.one_space);
      boolean _hasAtLeastOneProperty = this.hasAtLeastOneProperty(section);
      if (_hasAtLeastOneProperty) {
        final Procedure1<IHiddenRegionFormatter> _function_2 = new Procedure1<IHiddenRegionFormatter>() {
          @Override
          public void apply(final IHiddenRegionFormatter it) {
            it.indent();
          }
        };
        document.<ISemanticRegion, ISemanticRegion>interior(
          document.append(document.prepend(this.textRegionExtensions.regionFor(section).keyword(this._elkGraphGrammarAccess.getElkEdgeSectionAccess().getLeftSquareBracketKeyword_3()), ElkGraphFormatter.one_space), ElkGraphFormatter.new_line), 
          this.textRegionExtensions.regionFor(section).keyword(this._elkGraphGrammarAccess.getElkEdgeSectionAccess().getRightSquareBracketKeyword_5()), _function_2);
        document.append(this.textRegionExtensions.regionFor(section).assignment(this._elkGraphGrammarAccess.getElkEdgeSectionAccess().getIncomingShapeAssignment_4_0_0_2()), ElkGraphFormatter.new_line);
        document.append(this.textRegionExtensions.regionFor(section).assignment(this._elkGraphGrammarAccess.getElkEdgeSectionAccess().getOutgoingShapeAssignment_4_0_1_2()), ElkGraphFormatter.new_line);
        document.append(this.textRegionExtensions.regionFor(section).assignment(this._elkGraphGrammarAccess.getElkEdgeSectionAccess().getStartYAssignment_4_0_2_4()), ElkGraphFormatter.new_line);
        document.append(this.textRegionExtensions.regionFor(section).assignment(this._elkGraphGrammarAccess.getElkEdgeSectionAccess().getEndYAssignment_4_0_3_4()), ElkGraphFormatter.new_line);
        final Consumer<ISemanticRegion> _function_3 = new Consumer<ISemanticRegion>() {
          @Override
          public void accept(final ISemanticRegion it) {
            document.append(document.prepend(it, ElkGraphFormatter.one_space), ElkGraphFormatter.one_space);
          }
        };
        this.textRegionExtensions.regionFor(section).keywords(this._elkGraphGrammarAccess.getElkEdgeSectionAccess().getVerticalLineKeyword_4_1_3_0()).forEach(_function_3);
      } else {
        document.prepend(this.textRegionExtensions.regionFor(section).keyword(this._elkGraphGrammarAccess.getElkEdgeSectionAccess().getLeftSquareBracketKeyword_3()), ElkGraphFormatter.one_space);
        document.prepend(this.textRegionExtensions.regionFor(section).keyword(this._elkGraphGrammarAccess.getElkEdgeSectionAccess().getRightSquareBracketKeyword_5()), ElkGraphFormatter.no_space);
      }
    }
    document.<ElkBendPoint>append(IterableExtensions.<ElkBendPoint>last(section.getBendPoints()), ElkGraphFormatter.new_line);
    EList<ElkBendPoint> _bendPoints = section.getBendPoints();
    for (final ElkBendPoint point : _bendPoints) {
      document.<ElkBendPoint>format(point);
    }
    EMap<IProperty<?>, Object> _properties = section.getProperties();
    for (final Map.Entry<IProperty<?>, Object> property : _properties) {
      document.<Map.Entry<IProperty<?>, Object>>format(property);
    }
  }

  private boolean hasAtLeastOneProperty(final ElkEdgeSection section) {
    return (((((((section.eIsSet(ElkGraphPackage.Literals.ELK_EDGE_SECTION__INCOMING_SHAPE) || section.eIsSet(ElkGraphPackage.Literals.ELK_EDGE_SECTION__OUTGOING_SHAPE)) || section.eIsSet(ElkGraphPackage.Literals.ELK_EDGE_SECTION__START_X)) || section.eIsSet(ElkGraphPackage.Literals.ELK_EDGE_SECTION__START_Y)) || section.eIsSet(ElkGraphPackage.Literals.ELK_EDGE_SECTION__END_X)) || section.eIsSet(ElkGraphPackage.Literals.ELK_EDGE_SECTION__END_Y)) || (!section.getBendPoints().isEmpty())) || (!section.getProperties().isEmpty()));
  }

  protected void _format(final ElkBendPoint bendPoint, @Extension final IFormattableDocument document) {
    document.append(document.prepend(this.textRegionExtensions.regionFor(bendPoint).keyword(this._elkGraphGrammarAccess.getElkBendPointAccess().getCommaKeyword_1()), ElkGraphFormatter.no_space), ElkGraphFormatter.one_space);
  }

  protected void _format(final ElkPropertyToValueMapEntryImpl entry, @Extension final IFormattableDocument document) {
    document.append(document.prepend(this.textRegionExtensions.regionFor(entry).keyword(this._elkGraphGrammarAccess.getPropertyAccess().getColonKeyword_1()), ElkGraphFormatter.no_space), ElkGraphFormatter.one_space);
    EObject _eContainer = entry.eContainer();
    final EMapPropertyHolder container = ((EMapPropertyHolder) _eContainer);
    int _indexOf = container.getProperties().indexOf(entry);
    int _size = container.getProperties().size();
    int _minus = (_size - 1);
    boolean _equals = (_indexOf == _minus);
    if (_equals) {
      document.<ElkPropertyToValueMapEntryImpl>append(entry, ElkGraphFormatter.new_lines);
    } else {
      document.<ElkPropertyToValueMapEntryImpl>append(entry, ElkGraphFormatter.new_line);
    }
  }

  private void formatShapeLayout(final ElkShape shape, @Extension final IFormattableDocument document) {
    int propCount = 0;
    if ((shape.eIsSet(ElkGraphPackage.Literals.ELK_SHAPE__X) || shape.eIsSet(ElkGraphPackage.Literals.ELK_SHAPE__Y))) {
      propCount++;
    }
    if ((shape.eIsSet(ElkGraphPackage.Literals.ELK_SHAPE__WIDTH) || shape.eIsSet(ElkGraphPackage.Literals.ELK_SHAPE__HEIGHT))) {
      propCount++;
    }
    if ((propCount == 0)) {
      document.prepend(this.textRegionExtensions.regionFor(shape).keyword(this._elkGraphGrammarAccess.getShapeLayoutAccess().getLeftSquareBracketKeyword_1()), ElkGraphFormatter.one_space);
      document.append(document.prepend(this.textRegionExtensions.regionFor(shape).keyword(this._elkGraphGrammarAccess.getShapeLayoutAccess().getRightSquareBracketKeyword_3()), ElkGraphFormatter.no_space), ElkGraphFormatter.new_lines);
    } else {
      if ((propCount == 1)) {
        document.prepend(this.textRegionExtensions.regionFor(shape).keyword(this._elkGraphGrammarAccess.getShapeLayoutAccess().getLeftSquareBracketKeyword_1()), ElkGraphFormatter.one_space);
        document.append(document.prepend(this.textRegionExtensions.regionFor(shape).keyword(this._elkGraphGrammarAccess.getShapeLayoutAccess().getRightSquareBracketKeyword_3()), ElkGraphFormatter.one_space), ElkGraphFormatter.new_lines);
        document.prepend(this.textRegionExtensions.regionFor(shape).keyword(this._elkGraphGrammarAccess.getShapeLayoutAccess().getPositionKeyword_2_0_0()), ElkGraphFormatter.one_space);
        document.prepend(this.textRegionExtensions.regionFor(shape).keyword(this._elkGraphGrammarAccess.getShapeLayoutAccess().getSizeKeyword_2_1_0()), ElkGraphFormatter.one_space);
      } else {
        final Procedure1<IHiddenRegionFormatter> _function = new Procedure1<IHiddenRegionFormatter>() {
          @Override
          public void apply(final IHiddenRegionFormatter it) {
            it.indent();
          }
        };
        document.<ISemanticRegion, ISemanticRegion>interior(
          document.append(document.prepend(this.textRegionExtensions.regionFor(shape).keyword(this._elkGraphGrammarAccess.getShapeLayoutAccess().getLeftSquareBracketKeyword_1()), ElkGraphFormatter.one_space), ElkGraphFormatter.new_line), 
          document.append(this.textRegionExtensions.regionFor(shape).keyword(this._elkGraphGrammarAccess.getShapeLayoutAccess().getRightSquareBracketKeyword_3()), ElkGraphFormatter.new_lines), _function);
        document.append(this.textRegionExtensions.regionFor(shape).assignment(this._elkGraphGrammarAccess.getShapeLayoutAccess().getYAssignment_2_0_4()), ElkGraphFormatter.new_line);
        document.append(this.textRegionExtensions.regionFor(shape).assignment(this._elkGraphGrammarAccess.getShapeLayoutAccess().getHeightAssignment_2_1_4()), ElkGraphFormatter.new_line);
      }
    }
  }

  private void formatEdgeLayout(final ElkEdge edge, @Extension final IFormattableDocument document) {
    final int sectionCount = edge.getSections().size();
    if ((sectionCount == 0)) {
      document.prepend(this.textRegionExtensions.regionFor(edge).keyword(this._elkGraphGrammarAccess.getEdgeLayoutAccess().getLeftSquareBracketKeyword_1()), ElkGraphFormatter.one_space);
      document.append(document.prepend(this.textRegionExtensions.regionFor(edge).keyword(this._elkGraphGrammarAccess.getEdgeLayoutAccess().getRightSquareBracketKeyword_3()), ElkGraphFormatter.no_space), ElkGraphFormatter.new_lines);
    } else {
      final Procedure1<IHiddenRegionFormatter> _function = new Procedure1<IHiddenRegionFormatter>() {
        @Override
        public void apply(final IHiddenRegionFormatter it) {
          it.indent();
        }
      };
      document.<ISemanticRegion, ISemanticRegion>interior(
        document.append(document.prepend(this.textRegionExtensions.regionFor(edge).keyword(this._elkGraphGrammarAccess.getEdgeLayoutAccess().getLeftSquareBracketKeyword_1()), ElkGraphFormatter.one_space), ElkGraphFormatter.new_line), 
        document.append(this.textRegionExtensions.regionFor(edge).keyword(this._elkGraphGrammarAccess.getEdgeLayoutAccess().getRightSquareBracketKeyword_3()), ElkGraphFormatter.new_lines), _function);
    }
  }

  public void format(final Object node, final IFormattableDocument document) {
    if (node instanceof ElkNode) {
      _format((ElkNode)node, document);
      return;
    } else if (node instanceof ElkPort) {
      _format((ElkPort)node, document);
      return;
    } else if (node instanceof ElkPropertyToValueMapEntryImpl) {
      _format((ElkPropertyToValueMapEntryImpl)node, document);
      return;
    } else if (node instanceof ElkLabel) {
      _format((ElkLabel)node, document);
      return;
    } else if (node instanceof ElkEdge) {
      _format((ElkEdge)node, document);
      return;
    } else if (node instanceof XtextResource) {
      _format((XtextResource)node, document);
      return;
    } else if (node instanceof ElkEdgeSection) {
      _format((ElkEdgeSection)node, document);
      return;
    } else if (node instanceof ElkBendPoint) {
      _format((ElkBendPoint)node, document);
      return;
    } else if (node instanceof EObject) {
      _format((EObject)node, document);
      return;
    } else if (node == null) {
      _format((Void)null, document);
      return;
    } else if (node != null) {
      _format(node, document);
      return;
    } else {
      throw new IllegalArgumentException("Unhandled parameter types: " +
        Arrays.<Object>asList(node, document).toString());
    }
  }
}
