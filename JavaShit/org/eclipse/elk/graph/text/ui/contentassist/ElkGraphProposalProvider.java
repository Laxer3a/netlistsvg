/**
 * Copyright (c) 2016 TypeFox GmbH (http://www.typefox.io) and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.text.ui.contentassist;

import com.google.common.base.Objects;
import com.google.common.collect.ImmutableList;
import com.google.inject.Inject;
import com.google.inject.Provider;
import java.util.ArrayList;
import java.util.Collections;
import org.eclipse.elk.core.data.LayoutAlgorithmData;
import org.eclipse.elk.core.data.LayoutOptionData;
import org.eclipse.elk.graph.text.services.ElkGraphGrammarAccess;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.viewers.StyledString;
import org.eclipse.swt.graphics.Image;
import org.eclipse.xtext.AbstractElement;
import org.eclipse.xtext.Assignment;
import org.eclipse.xtext.Keyword;
import org.eclipse.xtext.RuleCall;
import org.eclipse.xtext.ide.editor.contentassist.ContentAssistContext;
import org.eclipse.xtext.ide.editor.contentassist.ContentAssistEntry;
import org.eclipse.xtext.ide.editor.contentassist.IIdeContentProposalAcceptor;
import org.eclipse.xtext.ide.editor.contentassist.IdeContentProposalProvider;
import org.eclipse.xtext.resource.IEObjectDescription;
import org.eclipse.xtext.ui.IImageHelper;
import org.eclipse.xtext.ui.editor.contentassist.AbstractContentProposalProvider;
import org.eclipse.xtext.ui.editor.contentassist.AbstractJavaBasedContentProposalProvider;
import org.eclipse.xtext.ui.editor.contentassist.ConfigurableCompletionProposal;
import org.eclipse.xtext.ui.editor.contentassist.ICompletionProposalAcceptor;
import org.eclipse.xtext.util.TextRegion;
import org.eclipse.xtext.xbase.lib.CollectionLiterals;
import org.eclipse.xtext.xbase.lib.IterableExtensions;
import org.eclipse.xtext.xbase.lib.Pair;
import org.eclipse.xtext.xbase.lib.Procedures.Procedure2;
import org.eclipse.xtext.xbase.lib.StringExtensions;

/**
 * Proposal provider that delegates to the generic IDE implementation.
 */
@SuppressWarnings("all")
public class ElkGraphProposalProvider extends AbstractJavaBasedContentProposalProvider {
  private static final int MAX_ENTRIES = 1000;

  @Inject
  private IdeContentProposalProvider ideProvider;

  @Inject
  private Provider<ContentAssistContext.Builder> builderProvider;

  @Inject
  private ElkGraphGrammarAccess grammar;

  @Inject
  private IImageHelper imageHelper;

  @Override
  public void createProposals(final org.eclipse.xtext.ui.editor.contentassist.ContentAssistContext context, final ICompletionProposalAcceptor acceptor) {
    final ArrayList<Pair<ContentAssistEntry, Integer>> entries = new ArrayList<Pair<ContentAssistEntry, Integer>>();
    final IIdeContentProposalAcceptor ideAcceptor = new IIdeContentProposalAcceptor() {
      @Override
      public void accept(final ContentAssistEntry entry, final int priority) {
        if ((entry != null)) {
          Pair<ContentAssistEntry, Integer> _mappedTo = Pair.<ContentAssistEntry, Integer>of(entry, Integer.valueOf(priority));
          entries.add(_mappedTo);
        }
      }

      @Override
      public boolean canAcceptMoreProposals() {
        int _size = entries.size();
        return (_size < ElkGraphProposalProvider.MAX_ENTRIES);
      }
    };
    ContentAssistContext _ideContext = this.getIdeContext(context);
    this.ideProvider.createProposals(Collections.<ContentAssistContext>unmodifiableList(CollectionLiterals.<ContentAssistContext>newArrayList(_ideContext)), ideAcceptor);
    final AbstractContentProposalProvider.NullSafeCompletionProposalAcceptor uiAcceptor = new AbstractContentProposalProvider.NullSafeCompletionProposalAcceptor(acceptor);
    final Procedure2<Pair<ContentAssistEntry, Integer>, Integer> _function = new Procedure2<Pair<ContentAssistEntry, Integer>, Integer>() {
      @Override
      public void apply(final Pair<ContentAssistEntry, Integer> p, final Integer index) {
        final ContentAssistEntry entry = p.getKey();
        final ConfigurableCompletionProposal proposal = ElkGraphProposalProvider.this.doCreateProposal(entry.getProposal(), ElkGraphProposalProvider.this.getDisplayString(entry), ElkGraphProposalProvider.this.getImage(entry), (p.getValue()).intValue(), context);
        uiAcceptor.accept(proposal);
      }
    };
    IterableExtensions.<Pair<ContentAssistEntry, Integer>>forEach(entries, _function);
  }

  @Override
  public void completeAssignment(final Assignment object, final org.eclipse.xtext.ui.editor.contentassist.ContentAssistContext context, final ICompletionProposalAcceptor acceptor) {
  }

  @Override
  public void completeKeyword(final Keyword object, final org.eclipse.xtext.ui.editor.contentassist.ContentAssistContext context, final ICompletionProposalAcceptor acceptor) {
  }

  @Override
  public void completeRuleCall(final RuleCall object, final org.eclipse.xtext.ui.editor.contentassist.ContentAssistContext context, final ICompletionProposalAcceptor acceptor) {
  }

  protected StyledString getDisplayString(final ContentAssistEntry entry) {
    String _elvis = null;
    String _label = entry.getLabel();
    if (_label != null) {
      _elvis = _label;
    } else {
      String _proposal = entry.getProposal();
      _elvis = _proposal;
    }
    final StyledString result = new StyledString(_elvis);
    boolean _isNullOrEmpty = StringExtensions.isNullOrEmpty(entry.getDescription());
    boolean _not = (!_isNullOrEmpty);
    if (_not) {
      String _description = entry.getDescription();
      String _plus = (" – " + _description);
      StyledString _styledString = new StyledString(_plus, StyledString.QUALIFIER_STYLER);
      result.append(_styledString);
    }
    return result;
  }

  private ContentAssistContext getIdeContext(final org.eclipse.xtext.ui.editor.contentassist.ContentAssistContext c) {
    final ContentAssistContext.Builder builder = this.builderProvider.get();
    final Region replaceRegion = c.getReplaceRegion();
    ContentAssistContext.Builder _setOffset = builder.setPrefix(c.getPrefix()).setSelectedText(c.getSelectedText()).setRootModel(c.getRootModel()).setRootNode(c.getRootNode()).setCurrentModel(c.getCurrentModel()).setPreviousModel(c.getPreviousModel()).setCurrentNode(c.getCurrentNode()).setLastCompleteNode(c.getLastCompleteNode()).setOffset(c.getOffset());
    int _offset = replaceRegion.getOffset();
    int _length = replaceRegion.getLength();
    TextRegion _textRegion = new TextRegion(_offset, _length);
    _setOffset.setReplaceRegion(_textRegion).setResource(c.getResource());
    ImmutableList<AbstractElement> _firstSetGrammarElements = c.getFirstSetGrammarElements();
    for (final AbstractElement grammarElement : _firstSetGrammarElements) {
      builder.accept(grammarElement);
    }
    return builder.toContext();
  }

  protected Image getImage(final ContentAssistEntry entry) {
    Image _switchResult = null;
    Object _source = entry.getSource();
    final Object source = _source;
    boolean _matched = false;
    if (source instanceof IEObjectDescription) {
      _matched=true;
      _switchResult = this.getImage(((IEObjectDescription)source));
    }
    if (!_matched) {
      if (source instanceof EObject) {
        _matched=true;
        _switchResult = this.getImage(((EObject)source));
      }
    }
    if (!_matched) {
      if (source instanceof LayoutOptionData) {
        _matched=true;
        _switchResult = this.getImage(((LayoutOptionData)source), entry.getProposal());
      }
    }
    if (!_matched) {
      if (source instanceof LayoutAlgorithmData) {
        _matched=true;
        _switchResult = this.imageHelper.getImage("prop_text.gif");
      }
    }
    return _switchResult;
  }

  @Override
  protected Image getImage(final EObject eObject) {
    if ((eObject instanceof Keyword)) {
      String _switchResult = null;
      boolean _matched = false;
      Keyword _graphKeyword_1_0 = this.grammar.getRootNodeAccess().getGraphKeyword_1_0();
      if (Objects.equal(eObject, _graphKeyword_1_0)) {
        _matched=true;
        _switchResult = "elkgraph";
      }
      if (!_matched) {
        Keyword _nodeKeyword_0 = this.grammar.getElkNodeAccess().getNodeKeyword_0();
        if (Objects.equal(eObject, _nodeKeyword_0)) {
          _matched=true;
          _switchResult = "elknode";
        }
      }
      if (!_matched) {
        Keyword _edgeKeyword_0 = this.grammar.getElkEdgeAccess().getEdgeKeyword_0();
        if (Objects.equal(eObject, _edgeKeyword_0)) {
          _matched=true;
          _switchResult = "elkedge";
        }
      }
      if (!_matched) {
        Keyword _portKeyword_0 = this.grammar.getElkPortAccess().getPortKeyword_0();
        if (Objects.equal(eObject, _portKeyword_0)) {
          _matched=true;
          _switchResult = "elkport";
        }
      }
      if (!_matched) {
        Keyword _labelKeyword_0 = this.grammar.getElkLabelAccess().getLabelKeyword_0();
        if (Objects.equal(eObject, _labelKeyword_0)) {
          _matched=true;
          _switchResult = "elklabel";
        }
      }
      final String key = _switchResult;
      if ((key != null)) {
        return this.imageHelper.getImage((key + ".gif"));
      }
    }
    return super.getImage(eObject);
  }

  private Image getImage(final LayoutOptionData option, final String value) {
    String _switchResult = null;
    LayoutOptionData.Type _type = option.getType();
    if (_type != null) {
      switch (_type) {
        case BOOLEAN:
          String _xifexpression = null;
          boolean _equals = Objects.equal(value, "false");
          if (_equals) {
            _xifexpression = "prop_false";
          } else {
            _xifexpression = "prop_true";
          }
          _switchResult = _xifexpression;
          break;
        case INT:
          _switchResult = "prop_int";
          break;
        case DOUBLE:
          _switchResult = "prop_double";
          break;
        case ENUM:
        case ENUMSET:
          _switchResult = "prop_choice";
          break;
        default:
          _switchResult = "prop_text";
          break;
      }
    } else {
      _switchResult = "prop_text";
    }
    final String key = _switchResult;
    return this.imageHelper.getImage((key + ".gif"));
  }
}
