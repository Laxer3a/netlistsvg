/**
 * Copyright (c) 2020 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.json.text.ide.contentassist;

import com.google.common.base.Objects;
import com.google.common.collect.Iterables;
import com.google.inject.Inject;
import com.google.inject.Provider;
import java.util.Collections;
import java.util.Set;
import java.util.function.Consumer;
import org.eclipse.elk.core.data.ILayoutMetaData;
import org.eclipse.elk.core.data.LayoutAlgorithmData;
import org.eclipse.elk.core.data.LayoutDataContentAssist;
import org.eclipse.elk.core.data.LayoutOptionData;
import org.eclipse.elk.core.options.CoreOptions;
import org.eclipse.elk.graph.ElkGraphElement;
import org.eclipse.elk.graph.impl.ElkPropertyToValueMapEntryImpl;
import org.eclipse.elk.graph.json.text.services.ElkGraphJsonGrammarAccess;
import org.eclipse.elk.graph.properties.IProperty;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.xtend2.lib.StringConcatenation;
import org.eclipse.xtext.AbstractElement;
import org.eclipse.xtext.Alternatives;
import org.eclipse.xtext.Assignment;
import org.eclipse.xtext.Keyword;
import org.eclipse.xtext.RuleCall;
import org.eclipse.xtext.conversion.impl.IDValueConverter;
import org.eclipse.xtext.ide.editor.contentassist.ContentAssistContext;
import org.eclipse.xtext.ide.editor.contentassist.ContentAssistEntry;
import org.eclipse.xtext.ide.editor.contentassist.IIdeContentProposalAcceptor;
import org.eclipse.xtext.ide.editor.contentassist.IdeContentProposalProvider;
import org.eclipse.xtext.util.Strings;
import org.eclipse.xtext.xbase.lib.CollectionLiterals;
import org.eclipse.xtext.xbase.lib.Functions.Function1;
import org.eclipse.xtext.xbase.lib.IterableExtensions;
import org.eclipse.xtext.xbase.lib.ListExtensions;
import org.eclipse.xtext.xbase.lib.ObjectExtensions;
import org.eclipse.xtext.xbase.lib.Procedures.Procedure1;

/**
 * Special content assist proposals for the ELK Graph JSON language.
 */
@SuppressWarnings("all")
public class ElkGraphJsonProposalProvider extends IdeContentProposalProvider {
  private static final Set<String> DISABLED_KEYWORDS = Collections.<String>unmodifiableSet(CollectionLiterals.<String>newHashSet("}", "]"));

  private ElkGraphJsonGrammarAccess grammar;

  private IDValueConverter idValueConverter;

  @Inject
  public void initialize(final Provider<IDValueConverter> idValueConverterProvider, final ElkGraphJsonGrammarAccess grammarAccess) {
    IDValueConverter _get = idValueConverterProvider.get();
    final Procedure1<IDValueConverter> _function = new Procedure1<IDValueConverter>() {
      @Override
      public void apply(final IDValueConverter it) {
        it.setRule(grammarAccess.getIDRule());
      }
    };
    IDValueConverter _doubleArrow = ObjectExtensions.<IDValueConverter>operator_doubleArrow(_get, _function);
    this.idValueConverter = _doubleArrow;
    this.grammar = grammarAccess;
  }

  @Override
  protected boolean filterKeyword(final Keyword keyword, final ContentAssistContext context) {
    return ((!ElkGraphJsonProposalProvider.DISABLED_KEYWORDS.contains(keyword.getValue())) && (!Objects.equal(keyword.getValue(), context.getPrefix())));
  }

  @Override
  protected void _createProposals(final Keyword keyword, final ContentAssistContext context, final IIdeContentProposalAcceptor acceptor) {
    boolean _filterKeyword = this.filterKeyword(keyword, context);
    if (_filterKeyword) {
      final ContentAssistEntry entry = this.getProposalCreator().createProposal(keyword.getValue(), context, ContentAssistEntry.KIND_KEYWORD, null);
      if ((entry != null)) {
        entry.setKind(ContentAssistEntry.KIND_KEYWORD);
        entry.setSource(keyword);
        acceptor.accept(entry, this.getProposalPriorities().getKeywordPriority(keyword.getValue(), entry));
      }
    }
  }

  @Override
  protected void _createProposals(final RuleCall ruleCall, final ContentAssistContext context, final IIdeContentProposalAcceptor acceptor) {
    boolean _startsWith = ruleCall.getRule().getName().startsWith("Key");
    if (_startsWith) {
      AbstractElement _alternatives = ruleCall.getRule().getAlternatives();
      final AbstractElement alternatives = _alternatives;
      boolean _matched = false;
      if (alternatives instanceof Alternatives) {
        _matched=true;
        final Function1<Keyword, Boolean> _function = new Function1<Keyword, Boolean>() {
          @Override
          public Boolean apply(final Keyword it) {
            return Boolean.valueOf(((!it.getValue().startsWith("\"")) && (!it.getValue().startsWith("\'"))));
          }
        };
        final Consumer<Keyword> _function_1 = new Consumer<Keyword>() {
          @Override
          public void accept(final Keyword it) {
            ElkGraphJsonProposalProvider.this.createProposals(it, context, acceptor);
          }
        };
        IterableExtensions.<Keyword>filter(Iterables.<Keyword>filter(((Alternatives)alternatives).getElements(), Keyword.class), _function).forEach(_function_1);
      }
    }
  }

  @Override
  protected void _createProposals(final Assignment assignment, final ContentAssistContext context, final IIdeContentProposalAcceptor acceptor) {
    boolean _matched = false;
    Assignment _keyAssignment_0 = this.grammar.getPropertyAccess().getKeyAssignment_0();
    if (Objects.equal(assignment, _keyAssignment_0)) {
      _matched=true;
      this.completePropertyKey(context, acceptor);
    }
    if (!_matched) {
      Assignment _valueAssignment_2_0 = this.grammar.getPropertyAccess().getValueAssignment_2_0();
      if (Objects.equal(assignment, _valueAssignment_2_0)) {
        _matched=true;
      }
      if (!_matched) {
        Assignment _valueAssignment_2_1 = this.grammar.getPropertyAccess().getValueAssignment_2_1();
        if (Objects.equal(assignment, _valueAssignment_2_1)) {
          _matched=true;
        }
      }
      if (!_matched) {
        Assignment _valueAssignment_2_2 = this.grammar.getPropertyAccess().getValueAssignment_2_2();
        if (Objects.equal(assignment, _valueAssignment_2_2)) {
          _matched=true;
        }
      }
      if (_matched) {
        this.completePropertyValue(context, acceptor);
      }
    }
    if (!_matched) {
      super._createProposals(assignment, context, acceptor);
    }
  }

  protected void completePropertyKey(final ContentAssistContext context, final IIdeContentProposalAcceptor acceptor) {
    final EObject model = context.getCurrentModel();
    if ((model instanceof ElkGraphElement)) {
      final Consumer<LayoutDataContentAssist.Proposal<LayoutOptionData>> _function = new Consumer<LayoutDataContentAssist.Proposal<LayoutOptionData>>() {
        @Override
        public void accept(final LayoutDataContentAssist.Proposal<LayoutOptionData> p) {
          ContentAssistEntry _contentAssistEntry = new ContentAssistEntry();
          final Procedure1<ContentAssistEntry> _function = new Procedure1<ContentAssistEntry>() {
            @Override
            public void apply(final ContentAssistEntry it) {
              it.setProposal(ElkGraphJsonProposalProvider.this.convertPropertyId(p.proposal));
              it.setPrefix(context.getPrefix());
              it.setKind(ContentAssistEntry.KIND_PROPERTY);
              String _elvis = null;
              if (p.label != null) {
                _elvis = p.label;
              } else {
                _elvis = p.proposal;
              }
              it.setLabel(_elvis);
              it.setDescription(ElkGraphJsonProposalProvider.this.getDescription(p.data));
              it.setDocumentation(p.data.getDescription());
              it.setSource(p.data);
            }
          };
          final ContentAssistEntry entry = ObjectExtensions.<ContentAssistEntry>operator_doubleArrow(_contentAssistEntry, _function);
          acceptor.accept(entry, ElkGraphJsonProposalProvider.this.getProposalPriorities().getDefaultPriority(entry));
        }
      };
      LayoutDataContentAssist.getLayoutOptionProposals(((ElkGraphElement)model), this.unquoteIfNecessary(context.getPrefix())).forEach(_function);
    }
  }

  /**
   * Besides {@link CoreOptions#ALGORITHM}, there's another option that allows to select a layout algorithm.
   *  To avoid a dependency to that plugin, the option is hard-coded here.
   */
  public static final String DISCO_LAYOUT_ALG_ID = "org.eclipse.elk.disco.componentCompaction.componentLayoutAlgorithm";

  protected void completePropertyValue(final ContentAssistContext context, final IIdeContentProposalAcceptor acceptor) {
    final EObject model = context.getCurrentModel();
    if ((model instanceof ElkPropertyToValueMapEntryImpl)) {
      final IProperty<?> option = ((ElkPropertyToValueMapEntryImpl)model).getKey();
      if ((option instanceof LayoutOptionData)) {
        if ((Objects.equal(CoreOptions.ALGORITHM, option) || Objects.equal(((LayoutOptionData)option).getId(), ElkGraphJsonProposalProvider.DISCO_LAYOUT_ALG_ID))) {
          this.proposeAlgorithms(context, acceptor);
        } else {
          this.typeAwarePropertyValueProposal(((LayoutOptionData)option), context, acceptor);
        }
      }
    }
  }

  protected void proposeAlgorithms(final ContentAssistContext context, final IIdeContentProposalAcceptor acceptor) {
    final Consumer<LayoutDataContentAssist.Proposal<LayoutAlgorithmData>> _function = new Consumer<LayoutDataContentAssist.Proposal<LayoutAlgorithmData>>() {
      @Override
      public void accept(final LayoutDataContentAssist.Proposal<LayoutAlgorithmData> p) {
        ContentAssistEntry _contentAssistEntry = new ContentAssistEntry();
        final Procedure1<ContentAssistEntry> _function = new Procedure1<ContentAssistEntry>() {
          @Override
          public void apply(final ContentAssistEntry it) {
            it.setProposal(ElkGraphJsonProposalProvider.this.convertPropertyId(p.proposal));
            it.setPrefix(context.getPrefix());
            it.setKind(ContentAssistEntry.KIND_VALUE);
            String _elvis = null;
            if (p.label != null) {
              _elvis = p.label;
            } else {
              _elvis = p.proposal;
            }
            it.setLabel(_elvis);
            it.setDescription(ElkGraphJsonProposalProvider.this.getDescription(p.data));
            it.setDocumentation(p.data.getDescription());
            it.setSource(p.data);
          }
        };
        final ContentAssistEntry entry = ObjectExtensions.<ContentAssistEntry>operator_doubleArrow(_contentAssistEntry, _function);
        acceptor.accept(entry, ElkGraphJsonProposalProvider.this.getProposalPriorities().getDefaultPriority(entry));
      }
    };
    LayoutDataContentAssist.getLayoutAlgorithmProposals(this.unquoteIfNecessary(context.getPrefix())).forEach(_function);
  }

  private void typeAwarePropertyValueProposal(final LayoutOptionData option, final ContentAssistContext context, final IIdeContentProposalAcceptor acceptor) {
    final Consumer<LayoutDataContentAssist.Proposal<Object>> _function = new Consumer<LayoutDataContentAssist.Proposal<Object>>() {
      @Override
      public void accept(final LayoutDataContentAssist.Proposal<Object> p) {
        String _xifexpression = null;
        boolean _contains = Collections.<LayoutOptionData.Type>unmodifiableSet(CollectionLiterals.<LayoutOptionData.Type>newHashSet(LayoutOptionData.Type.ENUM, LayoutOptionData.Type.ENUMSET, LayoutOptionData.Type.STRING)).contains(option.getType());
        if (_contains) {
          _xifexpression = (("\"" + p.proposal) + "\"");
        } else {
          _xifexpression = p.proposal;
        }
        final String proposal = _xifexpression;
        final Procedure1<ContentAssistEntry> _function = new Procedure1<ContentAssistEntry>() {
          @Override
          public void apply(final ContentAssistEntry it) {
            it.setLabel(p.label);
            it.setSource(option);
          }
        };
        final ContentAssistEntry entry = ElkGraphJsonProposalProvider.this.getProposalCreator().createProposal(proposal, context, ContentAssistEntry.KIND_VALUE, _function);
        acceptor.accept(entry, ElkGraphJsonProposalProvider.this.getProposalPriorities().getDefaultPriority(entry));
      }
    };
    LayoutDataContentAssist.getLayoutOptionValueProposal(option, this.unquoteIfNecessary(context.getPrefix())).forEach(_function);
  }

  private String convertPropertyId(final String proposal) {
    final Function1<String, String> _function = new Function1<String, String>() {
      @Override
      public String apply(final String it) {
        return ElkGraphJsonProposalProvider.this.idValueConverter.toString(it);
      }
    };
    String _join = IterableExtensions.join(ListExtensions.<String, String>map(Strings.split(proposal, "."), _function), ".");
    String _plus = ("\"" + _join);
    return (_plus + "\"");
  }

  private String getDescription(final ILayoutMetaData data) {
    StringConcatenation _builder = new StringConcatenation();
    String _name = data.getName();
    _builder.append(_name);
    _builder.append(" (");
    String _id = data.getId();
    _builder.append(_id);
    _builder.append(")");
    return _builder.toString();
  }

  private String unquoteIfNecessary(final String s) {
    String unquoted = s;
    while ((unquoted.startsWith("\"") || unquoted.startsWith("\'"))) {
      unquoted = unquoted.substring(1, unquoted.length());
    }
    return unquoted;
  }
}
