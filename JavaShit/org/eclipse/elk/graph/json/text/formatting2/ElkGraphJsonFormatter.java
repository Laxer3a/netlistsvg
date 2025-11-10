/**
 * Copyright (c) 2020 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.json.text.formatting2;

import com.google.inject.Inject;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.function.Consumer;
import org.eclipse.elk.graph.ElkEdge;
import org.eclipse.elk.graph.ElkGraphElement;
import org.eclipse.elk.graph.ElkLabel;
import org.eclipse.elk.graph.ElkNode;
import org.eclipse.elk.graph.ElkPort;
import org.eclipse.elk.graph.impl.ElkPropertyToValueMapEntryImpl;
import org.eclipse.elk.graph.json.text.services.ElkGraphJsonGrammarAccess;
import org.eclipse.elk.graph.properties.IProperty;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.xtext.Keyword;
import org.eclipse.xtext.formatting2.AbstractFormatter2;
import org.eclipse.xtext.formatting2.IFormattableDocument;
import org.eclipse.xtext.formatting2.IHiddenRegionFormatter;
import org.eclipse.xtext.formatting2.regionaccess.ISemanticRegion;
import org.eclipse.xtext.resource.XtextResource;
import org.eclipse.xtext.xbase.lib.CollectionLiterals;
import org.eclipse.xtext.xbase.lib.Conversions;
import org.eclipse.xtext.xbase.lib.Extension;
import org.eclipse.xtext.xbase.lib.IterableExtensions;
import org.eclipse.xtext.xbase.lib.ObjectExtensions;
import org.eclipse.xtext.xbase.lib.Procedures.Procedure1;

@SuppressWarnings("all")
public class ElkGraphJsonFormatter extends AbstractFormatter2 {
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

  @Inject
  @Extension
  private ElkGraphJsonGrammarAccess _elkGraphJsonGrammarAccess;

  protected void _format(final ElkNode node, @Extension final IFormattableDocument document) {
    final boolean compact = (((node.getChildren().isEmpty() && node.getPorts().isEmpty()) && node.getLabels().isEmpty()) && node.getProperties().isEmpty());
    Keyword _commaKeyword_2_1_0 = this._elkGraphJsonGrammarAccess.getElkNodeAccess().getCommaKeyword_2_1_0();
    Keyword _commaKeyword_3 = this._elkGraphJsonGrammarAccess.getElkNodeAccess().getCommaKeyword_3();
    this.formatWithCommonGraphElementStyle(node, Collections.<Keyword>unmodifiableList(CollectionLiterals.<Keyword>newArrayList(_commaKeyword_2_1_0, _commaKeyword_3)), 
      this._elkGraphJsonGrammarAccess.getElkNodeAccess().getLeftCurlyBracketKeyword_1(), this._elkGraphJsonGrammarAccess.getElkNodeAccess().getRightCurlyBracketKeyword_4(), compact, document);
    this.formatElkNodeChildren(node, document);
    this.formatElkNodePorts(node, document);
    this.formatElkGraphElementLabels(node, document);
    this.formatElkNodeEdges(node, document);
    this.formatLayoutOptions(node, document);
    this.formatJsonMember(node, document);
    final Consumer<ElkNode> _function = new Consumer<ElkNode>() {
      @Override
      public void accept(final ElkNode it) {
        document.<ElkNode>format(it);
      }
    };
    node.getChildren().forEach(_function);
    final Consumer<ElkPort> _function_1 = new Consumer<ElkPort>() {
      @Override
      public void accept(final ElkPort it) {
        document.<ElkPort>format(it);
      }
    };
    node.getPorts().forEach(_function_1);
    final Consumer<ElkLabel> _function_2 = new Consumer<ElkLabel>() {
      @Override
      public void accept(final ElkLabel it) {
        document.<ElkLabel>format(it);
      }
    };
    node.getLabels().forEach(_function_2);
    final Consumer<ElkEdge> _function_3 = new Consumer<ElkEdge>() {
      @Override
      public void accept(final ElkEdge it) {
        document.<ElkEdge>format(it);
      }
    };
    node.getContainedEdges().forEach(_function_3);
    final Consumer<Map.Entry<IProperty<?>, Object>> _function_4 = new Consumer<Map.Entry<IProperty<?>, Object>>() {
      @Override
      public void accept(final Map.Entry<IProperty<?>, Object> it) {
        document.<Map.Entry<IProperty<?>, Object>>format(it);
      }
    };
    node.getProperties().forEach(_function_4);
  }

  protected void _format(final ElkPort port, @Extension final IFormattableDocument document) {
    final boolean compact = (port.getProperties().isEmpty() && (port.getLabels().size() < 2));
    Keyword _commaKeyword_2_0 = this._elkGraphJsonGrammarAccess.getElkPortAccess().getCommaKeyword_2_0();
    Keyword _commaKeyword_3 = this._elkGraphJsonGrammarAccess.getElkPortAccess().getCommaKeyword_3();
    this.formatWithCommonGraphElementStyle(port, Collections.<Keyword>unmodifiableList(CollectionLiterals.<Keyword>newArrayList(_commaKeyword_2_0, _commaKeyword_3)), 
      this._elkGraphJsonGrammarAccess.getElkPortAccess().getLeftCurlyBracketKeyword_0(), this._elkGraphJsonGrammarAccess.getElkPortAccess().getRightCurlyBracketKeyword_4(), compact, document);
    this.formatElkGraphElementLabels(port, document);
    this.formatLayoutOptions(port, document);
    this.formatJsonMember(port, document);
    final Consumer<ElkLabel> _function = new Consumer<ElkLabel>() {
      @Override
      public void accept(final ElkLabel it) {
        document.<ElkLabel>format(it);
      }
    };
    port.getLabels().forEach(_function);
    final Consumer<Map.Entry<IProperty<?>, Object>> _function_1 = new Consumer<Map.Entry<IProperty<?>, Object>>() {
      @Override
      public void accept(final Map.Entry<IProperty<?>, Object> it) {
        document.<Map.Entry<IProperty<?>, Object>>format(it);
      }
    };
    port.getProperties().forEach(_function_1);
  }

  protected void _format(final ElkLabel label, @Extension final IFormattableDocument document) {
    final boolean compact = (label.getProperties().isEmpty() && label.getLabels().isEmpty());
    Keyword _commaKeyword_2_0 = this._elkGraphJsonGrammarAccess.getElkLabelAccess().getCommaKeyword_2_0();
    Keyword _commaKeyword_3 = this._elkGraphJsonGrammarAccess.getElkLabelAccess().getCommaKeyword_3();
    this.formatWithCommonGraphElementStyle(label, Collections.<Keyword>unmodifiableList(CollectionLiterals.<Keyword>newArrayList(_commaKeyword_2_0, _commaKeyword_3)), 
      this._elkGraphJsonGrammarAccess.getElkLabelAccess().getLeftCurlyBracketKeyword_0(), this._elkGraphJsonGrammarAccess.getElkLabelAccess().getRightCurlyBracketKeyword_4(), compact, document);
    this.formatElkGraphElementLabels(label, document);
    this.formatLayoutOptions(label, document);
    this.formatJsonMember(label, document);
    final Consumer<ElkLabel> _function = new Consumer<ElkLabel>() {
      @Override
      public void accept(final ElkLabel it) {
        document.<ElkLabel>format(it);
      }
    };
    label.getLabels().forEach(_function);
    final Consumer<Map.Entry<IProperty<?>, Object>> _function_1 = new Consumer<Map.Entry<IProperty<?>, Object>>() {
      @Override
      public void accept(final Map.Entry<IProperty<?>, Object> it) {
        document.<Map.Entry<IProperty<?>, Object>>format(it);
      }
    };
    label.getProperties().forEach(_function_1);
  }

  protected void _format(final ElkEdge edge, @Extension final IFormattableDocument document) {
    final boolean compact = ((edge.getProperties().isEmpty() && (edge.getSources().size() < 2)) && (edge.getTargets().size() < 2));
    Keyword _commaKeyword_2_0 = this._elkGraphJsonGrammarAccess.getElkEdgeAccess().getCommaKeyword_2_0();
    Keyword _commaKeyword_3 = this._elkGraphJsonGrammarAccess.getElkEdgeAccess().getCommaKeyword_3();
    this.formatWithCommonGraphElementStyle(edge, Collections.<Keyword>unmodifiableList(CollectionLiterals.<Keyword>newArrayList(_commaKeyword_2_0, _commaKeyword_3)), 
      this._elkGraphJsonGrammarAccess.getElkEdgeAccess().getLeftCurlyBracketKeyword_0(), this._elkGraphJsonGrammarAccess.getElkEdgeAccess().getRightCurlyBracketKeyword_4(), compact, document);
    this.formatElkEdgeSources(edge, document);
    this.formatElkEdgeTargets(edge, document);
    this.formatElkGraphElementLabels(edge, document);
    this.formatLayoutOptions(edge, document);
    this.formatJsonMember(edge, document);
    final Consumer<ElkLabel> _function = new Consumer<ElkLabel>() {
      @Override
      public void accept(final ElkLabel it) {
        document.<ElkLabel>format(it);
      }
    };
    edge.getLabels().forEach(_function);
    final Consumer<Map.Entry<IProperty<?>, Object>> _function_1 = new Consumer<Map.Entry<IProperty<?>, Object>>() {
      @Override
      public void accept(final Map.Entry<IProperty<?>, Object> it) {
        document.<Map.Entry<IProperty<?>, Object>>format(it);
      }
    };
    edge.getProperties().forEach(_function_1);
  }

  protected void _format(final ElkPropertyToValueMapEntryImpl entry, @Extension final IFormattableDocument document) {
    document.append(document.prepend(this.textRegionExtensions.regionFor(entry).keyword(this._elkGraphJsonGrammarAccess.getPropertyAccess().getColonKeyword_1()), ElkGraphJsonFormatter.no_space), ElkGraphJsonFormatter.one_space);
  }

  private void formatWithCommonGraphElementStyle(final ElkGraphElement element, final List<Keyword> commaKeywords, final Keyword openingKeyword, final Keyword closingKeyword, final boolean compact, @Extension final IFormattableDocument document) {
    final Consumer<ISemanticRegion> _function = new Consumer<ISemanticRegion>() {
      @Override
      public void accept(final ISemanticRegion it) {
        document.append(document.prepend(it, ElkGraphJsonFormatter.no_space), ElkGraphJsonFormatter.one_space);
      }
    };
    this.textRegionExtensions.regionFor(element).keywords(":").forEach(_function);
    final Consumer<ISemanticRegion> _function_1 = new Consumer<ISemanticRegion>() {
      @Override
      public void accept(final ISemanticRegion it) {
        document.prepend(it, ElkGraphJsonFormatter.no_space);
        if (compact) {
          document.append(it, ElkGraphJsonFormatter.one_space);
        } else {
          document.append(it, ElkGraphJsonFormatter.new_line);
        }
      }
    };
    this.textRegionExtensions.regionFor(element).keywords(((Keyword[])Conversions.unwrapArray(commaKeywords, Keyword.class))).forEach(_function_1);
    ISemanticRegion _prepend = document.prepend(this.textRegionExtensions.regionFor(element).keyword(openingKeyword), ElkGraphJsonFormatter.no_space);
    final Procedure1<ISemanticRegion> _function_2 = new Procedure1<ISemanticRegion>() {
      @Override
      public void apply(final ISemanticRegion it) {
        if (compact) {
          document.append(it, ElkGraphJsonFormatter.one_space);
        } else {
          document.append(it, ElkGraphJsonFormatter.new_line);
        }
      }
    };
    ISemanticRegion _doubleArrow = ObjectExtensions.<ISemanticRegion>operator_doubleArrow(_prepend, _function_2);
    ISemanticRegion _keyword = this.textRegionExtensions.regionFor(element).keyword(closingKeyword);
    final Procedure1<ISemanticRegion> _function_3 = new Procedure1<ISemanticRegion>() {
      @Override
      public void apply(final ISemanticRegion it) {
        if (compact) {
          document.prepend(it, ElkGraphJsonFormatter.one_space);
        } else {
          document.prepend(it, ElkGraphJsonFormatter.new_line);
        }
      }
    };
    ISemanticRegion _doubleArrow_1 = ObjectExtensions.<ISemanticRegion>operator_doubleArrow(_keyword, _function_3);
    final Procedure1<IHiddenRegionFormatter> _function_4 = new Procedure1<IHiddenRegionFormatter>() {
      @Override
      public void apply(final IHiddenRegionFormatter it) {
        it.indent();
      }
    };
    document.<ISemanticRegion, ISemanticRegion>interior(_doubleArrow, _doubleArrow_1, _function_4);
  }

  /**
   * Formatting of container fragments.
   */
  private void formatElkEdgeSources(final ElkEdge edge, @Extension final IFormattableDocument document) {
    Keyword _commaKeyword_1_1_0 = this._elkGraphJsonGrammarAccess.getElkEdgeSourcesAccess().getCommaKeyword_1_1_0();
    Keyword _commaKeyword_2 = this._elkGraphJsonGrammarAccess.getElkEdgeSourcesAccess().getCommaKeyword_2();
    this.formatWithCommonContainerStyle(edge, true, 
      Collections.<Keyword>unmodifiableList(CollectionLiterals.<Keyword>newArrayList(_commaKeyword_1_1_0, _commaKeyword_2)), 
      this._elkGraphJsonGrammarAccess.getElkEdgeSourcesAccess().getLeftSquareBracketKeyword_0(), this._elkGraphJsonGrammarAccess.getElkEdgeSourcesAccess().getRightSquareBracketKeyword_3(), document);
  }

  private void formatElkEdgeTargets(final ElkEdge edge, @Extension final IFormattableDocument document) {
    Keyword _commaKeyword_1_1_0 = this._elkGraphJsonGrammarAccess.getElkEdgeTargetsAccess().getCommaKeyword_1_1_0();
    Keyword _commaKeyword_2 = this._elkGraphJsonGrammarAccess.getElkEdgeTargetsAccess().getCommaKeyword_2();
    this.formatWithCommonContainerStyle(edge, true, 
      Collections.<Keyword>unmodifiableList(CollectionLiterals.<Keyword>newArrayList(_commaKeyword_1_1_0, _commaKeyword_2)), 
      this._elkGraphJsonGrammarAccess.getElkEdgeTargetsAccess().getLeftSquareBracketKeyword_0(), this._elkGraphJsonGrammarAccess.getElkEdgeTargetsAccess().getRightSquareBracketKeyword_3(), document);
  }

  private void formatElkNodeChildren(final ElkNode node, @Extension final IFormattableDocument document) {
    Keyword _commaKeyword_1_1_0 = this._elkGraphJsonGrammarAccess.getElkNodeChildrenAccess().getCommaKeyword_1_1_0();
    Keyword _commaKeyword_2 = this._elkGraphJsonGrammarAccess.getElkNodeChildrenAccess().getCommaKeyword_2();
    this.formatWithCommonContainerStyle(node, IterableExtensions.isNullOrEmpty(node.getChildren()), 
      Collections.<Keyword>unmodifiableList(CollectionLiterals.<Keyword>newArrayList(_commaKeyword_1_1_0, _commaKeyword_2)), 
      this._elkGraphJsonGrammarAccess.getElkNodeChildrenAccess().getLeftSquareBracketKeyword_0(), this._elkGraphJsonGrammarAccess.getElkNodeChildrenAccess().getRightSquareBracketKeyword_3(), document);
  }

  private void formatElkNodePorts(final ElkNode node, @Extension final IFormattableDocument document) {
    Keyword _commaKeyword_1_1_0 = this._elkGraphJsonGrammarAccess.getElkNodePortsAccess().getCommaKeyword_1_1_0();
    Keyword _commaKeyword_2 = this._elkGraphJsonGrammarAccess.getElkNodePortsAccess().getCommaKeyword_2();
    this.formatWithCommonContainerStyle(node, IterableExtensions.isNullOrEmpty(node.getPorts()), 
      Collections.<Keyword>unmodifiableList(CollectionLiterals.<Keyword>newArrayList(_commaKeyword_1_1_0, _commaKeyword_2)), 
      this._elkGraphJsonGrammarAccess.getElkNodePortsAccess().getLeftSquareBracketKeyword_0(), this._elkGraphJsonGrammarAccess.getElkNodePortsAccess().getRightSquareBracketKeyword_3(), document);
  }

  private void formatElkNodeEdges(final ElkNode node, @Extension final IFormattableDocument document) {
    Keyword _commaKeyword_1_1_0 = this._elkGraphJsonGrammarAccess.getElkNodeEdgesAccess().getCommaKeyword_1_1_0();
    Keyword _commaKeyword_2 = this._elkGraphJsonGrammarAccess.getElkNodeEdgesAccess().getCommaKeyword_2();
    this.formatWithCommonContainerStyle(node, IterableExtensions.isNullOrEmpty(node.getContainedEdges()), 
      Collections.<Keyword>unmodifiableList(CollectionLiterals.<Keyword>newArrayList(_commaKeyword_1_1_0, _commaKeyword_2)), 
      this._elkGraphJsonGrammarAccess.getElkNodeEdgesAccess().getLeftSquareBracketKeyword_0(), this._elkGraphJsonGrammarAccess.getElkNodeEdgesAccess().getRightSquareBracketKeyword_3(), document);
  }

  private void formatElkGraphElementLabels(final ElkGraphElement element, @Extension final IFormattableDocument document) {
    Keyword _commaKeyword_1_1_0 = this._elkGraphJsonGrammarAccess.getElkGraphElementLabelsAccess().getCommaKeyword_1_1_0();
    Keyword _commaKeyword_2 = this._elkGraphJsonGrammarAccess.getElkGraphElementLabelsAccess().getCommaKeyword_2();
    this.formatWithCommonContainerStyle(element, IterableExtensions.isNullOrEmpty(element.getLabels()), 
      Collections.<Keyword>unmodifiableList(CollectionLiterals.<Keyword>newArrayList(_commaKeyword_1_1_0, _commaKeyword_2)), 
      this._elkGraphJsonGrammarAccess.getElkGraphElementLabelsAccess().getLeftSquareBracketKeyword_0(), 
      this._elkGraphJsonGrammarAccess.getElkGraphElementLabelsAccess().getRightSquareBracketKeyword_3(), document);
  }

  private void formatLayoutOptions(final ElkGraphElement element, @Extension final IFormattableDocument document) {
    int _size = element.getProperties().size();
    final boolean compact = (_size < 2);
    Keyword _commaKeyword_1_1_0 = this._elkGraphJsonGrammarAccess.getElkGraphElementPropertiesAccess().getCommaKeyword_1_1_0();
    Keyword _commaKeyword_2 = this._elkGraphJsonGrammarAccess.getElkGraphElementPropertiesAccess().getCommaKeyword_2();
    final List<Keyword> commas = Collections.<Keyword>unmodifiableList(CollectionLiterals.<Keyword>newArrayList(_commaKeyword_1_1_0, _commaKeyword_2));
    this.formatWithCommonContainerStyle(element, compact, commas, 
      this._elkGraphJsonGrammarAccess.getElkGraphElementPropertiesAccess().getLeftCurlyBracketKeyword_0(), 
      this._elkGraphJsonGrammarAccess.getElkGraphElementPropertiesAccess().getRightCurlyBracketKeyword_3(), document);
    if ((!compact)) {
      document.append(this.textRegionExtensions.regionFor(element).keyword(this._elkGraphJsonGrammarAccess.getElkGraphElementPropertiesAccess().getLeftCurlyBracketKeyword_0()), ElkGraphJsonFormatter.new_line);
    }
    final Consumer<ISemanticRegion> _function = new Consumer<ISemanticRegion>() {
      @Override
      public void accept(final ISemanticRegion it) {
        if (compact) {
          document.append(document.prepend(it, ElkGraphJsonFormatter.no_space), ElkGraphJsonFormatter.one_space);
        } else {
          document.append(it, ElkGraphJsonFormatter.new_line);
        }
      }
    };
    this.textRegionExtensions.regionFor(element).keywords(((Keyword[])Conversions.unwrapArray(commas, Keyword.class))).forEach(_function);
  }

  private void formatWithCommonContainerStyle(final ElkGraphElement element, final boolean compact, final List<Keyword> commaKeywords, final Keyword openingKeyword, final Keyword closingKeyword, @Extension final IFormattableDocument document) {
    final Consumer<ISemanticRegion> _function = new Consumer<ISemanticRegion>() {
      @Override
      public void accept(final ISemanticRegion it) {
        document.append(document.prepend(it, ElkGraphJsonFormatter.no_space), ElkGraphJsonFormatter.new_line);
      }
    };
    this.textRegionExtensions.regionFor(element).keywords(((Keyword[])Conversions.unwrapArray(commaKeywords, Keyword.class))).forEach(_function);
    if (compact) {
      document.append(this.textRegionExtensions.regionFor(element).keyword(openingKeyword), ElkGraphJsonFormatter.one_space);
      document.prepend(this.textRegionExtensions.regionFor(element).keyword(closingKeyword), ElkGraphJsonFormatter.one_space);
    } else {
      final Procedure1<IHiddenRegionFormatter> _function_1 = new Procedure1<IHiddenRegionFormatter>() {
        @Override
        public void apply(final IHiddenRegionFormatter it) {
          it.indent();
        }
      };
      document.<ISemanticRegion, ISemanticRegion>interior(
        document.append(document.prepend(this.textRegionExtensions.regionFor(element).keyword(openingKeyword), ElkGraphJsonFormatter.one_space), ElkGraphJsonFormatter.new_line), 
        document.prepend(this.textRegionExtensions.regionFor(element).keyword(closingKeyword), ElkGraphJsonFormatter.new_line), _function_1);
    }
  }

  /**
   * TODO not working this way. There are not semantic regions found when using the 'keywords' accessor.
   * Maybe this is due to the fact that nothing of the json members is part of the actual semantic model?
   */
  private void formatJsonMember(final ElkGraphElement element, @Extension final IFormattableDocument document) {
    final Consumer<ISemanticRegion> _function = new Consumer<ISemanticRegion>() {
      @Override
      public void accept(final ISemanticRegion it) {
        document.append(document.prepend(it, ElkGraphJsonFormatter.no_space), ElkGraphJsonFormatter.one_space);
      }
    };
    this.textRegionExtensions.regionFor(element).keywords(this._elkGraphJsonGrammarAccess.getJsonMemberAccess().getColonKeyword_1()).forEach(_function);
    final Consumer<ISemanticRegion> _function_1 = new Consumer<ISemanticRegion>() {
      @Override
      public void accept(final ISemanticRegion it) {
        document.append(document.prepend(it, ElkGraphJsonFormatter.one_space), ElkGraphJsonFormatter.new_line);
      }
    };
    this.textRegionExtensions.regionFor(element).keywords(this._elkGraphJsonGrammarAccess.getJsonArrayAccess().getLeftSquareBracketKeyword_0()).forEach(_function_1);
    final Consumer<ISemanticRegion> _function_2 = new Consumer<ISemanticRegion>() {
      @Override
      public void accept(final ISemanticRegion it) {
        document.append(document.prepend(it, ElkGraphJsonFormatter.no_space), ElkGraphJsonFormatter.one_space);
      }
    };
    this.textRegionExtensions.regionFor(element).keywords(this._elkGraphJsonGrammarAccess.getJsonArrayAccess().getCommaKeyword_1_1_0(), this._elkGraphJsonGrammarAccess.getJsonArrayAccess().getCommaKeyword_2()).forEach(_function_2);
    final Consumer<ISemanticRegion> _function_3 = new Consumer<ISemanticRegion>() {
      @Override
      public void accept(final ISemanticRegion it) {
        document.append(document.prepend(it, ElkGraphJsonFormatter.one_space), ElkGraphJsonFormatter.new_line);
      }
    };
    this.textRegionExtensions.regionFor(element).keywords(this._elkGraphJsonGrammarAccess.getJsonObjectAccess().getLeftCurlyBracketKeyword_0()).forEach(_function_3);
    final Consumer<ISemanticRegion> _function_4 = new Consumer<ISemanticRegion>() {
      @Override
      public void accept(final ISemanticRegion it) {
        document.append(document.prepend(it, ElkGraphJsonFormatter.no_space), ElkGraphJsonFormatter.one_space);
      }
    };
    this.textRegionExtensions.regionFor(element).keywords(this._elkGraphJsonGrammarAccess.getJsonObjectAccess().getCommaKeyword_1_1_0(), this._elkGraphJsonGrammarAccess.getJsonObjectAccess().getCommaKeyword_2()).forEach(_function_4);
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
