/**
 * Copyright (c) 2017 TypeFox GmbH (http://www.typefox.io) and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.text.ide.contentassist;

import com.google.common.base.Objects;
import com.google.common.base.Predicate;
import com.google.inject.Inject;
import com.google.inject.Provider;
import java.util.Collections;
import java.util.List;
import java.util.Set;
import java.util.function.Consumer;
import org.eclipse.elk.core.data.ILayoutMetaData;
import org.eclipse.elk.core.data.LayoutAlgorithmData;
import org.eclipse.elk.core.data.LayoutDataContentAssist;
import org.eclipse.elk.core.data.LayoutOptionData;
import org.eclipse.elk.core.options.CoreOptions;
import org.eclipse.elk.graph.ElkEdgeSection;
import org.eclipse.elk.graph.ElkGraphElement;
import org.eclipse.elk.graph.impl.ElkPropertyToValueMapEntryImpl;
import org.eclipse.elk.graph.properties.IProperty;
import org.eclipse.elk.graph.text.services.ElkGraphGrammarAccess;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.xtend.lib.annotations.FinalFieldsConstructor;
import org.eclipse.xtend2.lib.StringConcatenation;
import org.eclipse.xtext.Assignment;
import org.eclipse.xtext.CrossReference;
import org.eclipse.xtext.Keyword;
import org.eclipse.xtext.conversion.impl.IDValueConverter;
import org.eclipse.xtext.ide.editor.contentassist.ContentAssistContext;
import org.eclipse.xtext.ide.editor.contentassist.ContentAssistEntry;
import org.eclipse.xtext.ide.editor.contentassist.IIdeContentProposalAcceptor;
import org.eclipse.xtext.ide.editor.contentassist.IProposalConflictHelper;
import org.eclipse.xtext.ide.editor.contentassist.IdeContentProposalProvider;
import org.eclipse.xtext.resource.IEObjectDescription;
import org.eclipse.xtext.util.Strings;
import org.eclipse.xtext.xbase.lib.CollectionLiterals;
import org.eclipse.xtext.xbase.lib.Functions.Function1;
import org.eclipse.xtext.xbase.lib.IterableExtensions;
import org.eclipse.xtext.xbase.lib.ObjectExtensions;
import org.eclipse.xtext.xbase.lib.Procedures.Procedure1;

/**
 * Special content assist proposals for the ELK Graph language.
 */
@SuppressWarnings("all")
public class ElkGraphProposalProvider extends IdeContentProposalProvider {
  @FinalFieldsConstructor
  public static class SectionShapeFilter implements Predicate<IEObjectDescription> {
    private static final int INCOMING = 0;

    private static final int OUTGOING = 1;

    private final ElkEdgeSection section;

    private final int type;

    @Override
    public boolean apply(final IEObjectDescription input) {
      boolean _switchResult = false;
      final int type = this.type;
      switch (type) {
        case ElkGraphProposalProvider.SectionShapeFilter.INCOMING:
          _switchResult = this.isInList(input, this.section.getParent().getSources());
          break;
        case ElkGraphProposalProvider.SectionShapeFilter.OUTGOING:
          _switchResult = this.isInList(input, this.section.getParent().getTargets());
          break;
        default:
          _switchResult = true;
          break;
      }
      return _switchResult;
    }

    private boolean isInList(final IEObjectDescription input, final List<? extends EObject> list) {
      boolean _xblockexpression = false;
      {
        final EObject object = input.getEObjectOrProxy();
        boolean _xifexpression = false;
        boolean _eIsProxy = object.eIsProxy();
        if (_eIsProxy) {
          final Function1<EObject, Boolean> _function = new Function1<EObject, Boolean>() {
            @Override
            public Boolean apply(final EObject it) {
              URI _uRI = EcoreUtil.getURI(it);
              URI _eObjectURI = input.getEObjectURI();
              return Boolean.valueOf(Objects.equal(_uRI, _eObjectURI));
            }
          };
          _xifexpression = IterableExtensions.exists(list, _function);
        } else {
          _xifexpression = list.contains(object);
        }
        _xblockexpression = _xifexpression;
      }
      return _xblockexpression;
    }

    public SectionShapeFilter(final ElkEdgeSection section, final int type) {
      super();
      this.section = section;
      this.type = type;
    }
  }

  private static final Set<String> DISABLED_KEYWORDS = Collections.<String>unmodifiableSet(CollectionLiterals.<String>newHashSet("}", "]"));

  private ElkGraphGrammarAccess grammar;

  private IDValueConverter idValueConverter;

  @Inject
  private IProposalConflictHelper conflictHelper;

  @Inject
  public void initialize(final Provider<IDValueConverter> idValueConverterProvider, final ElkGraphGrammarAccess grammarAccess) {
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
    return ((!ElkGraphProposalProvider.DISABLED_KEYWORDS.contains(keyword.getValue())) && (!Objects.equal(keyword.getValue(), context.getPrefix())));
  }

  @Override
  protected void _createProposals(final Keyword keyword, final ContentAssistContext context, final IIdeContentProposalAcceptor acceptor) {
    boolean _filterKeyword = this.filterKeyword(keyword, context);
    if (_filterKeyword) {
      final ContentAssistEntry entry = this.getProposalCreator().createProposal(keyword.getValue(), context);
      if ((entry != null)) {
        entry.setKind(ContentAssistEntry.KIND_KEYWORD);
        entry.setSource(keyword);
        acceptor.accept(entry, this.getProposalPriorities().getKeywordPriority(keyword.getValue(), entry));
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
      if (!_matched) {
        Assignment _valueAssignment_2_3 = this.grammar.getPropertyAccess().getValueAssignment_2_3();
        if (Objects.equal(assignment, _valueAssignment_2_3)) {
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
    boolean _existsConflict = this.conflictHelper.existsConflict("a", context);
    if (_existsConflict) {
      return;
    }
    final EObject model = context.getCurrentModel();
    if ((model instanceof ElkGraphElement)) {
      final Consumer<LayoutDataContentAssist.Proposal<LayoutOptionData>> _function = new Consumer<LayoutDataContentAssist.Proposal<LayoutOptionData>>() {
        @Override
        public void accept(final LayoutDataContentAssist.Proposal<LayoutOptionData> p) {
          ContentAssistEntry _contentAssistEntry = new ContentAssistEntry();
          final Procedure1<ContentAssistEntry> _function = new Procedure1<ContentAssistEntry>() {
            @Override
            public void apply(final ContentAssistEntry it) {
              it.setProposal(ElkGraphProposalProvider.this.convert(Strings.split(p.proposal, ".")));
              it.setPrefix(context.getPrefix());
              it.setKind(ContentAssistEntry.KIND_PROPERTY);
              String _elvis = null;
              if (p.label != null) {
                _elvis = p.label;
              } else {
                _elvis = p.proposal;
              }
              it.setLabel(_elvis);
              it.setDescription(ElkGraphProposalProvider.this.getDescription(p.data));
              it.setDocumentation(p.data.getDescription());
              it.setSource(p.data);
            }
          };
          final ContentAssistEntry entry = ObjectExtensions.<ContentAssistEntry>operator_doubleArrow(_contentAssistEntry, _function);
          acceptor.accept(entry, ElkGraphProposalProvider.this.getProposalPriorities().getDefaultPriority(entry));
        }
      };
      LayoutDataContentAssist.getLayoutOptionProposals(((ElkGraphElement)model), context.getPrefix()).forEach(_function);
    }
  }

  /**
   * There's another option that allows to select a layout algorithm other than {@link CoreOptions#ALGORITHM}.
   *  To avoid a dependency to that plugin, the option is hard-coded here.
   */
  public static final String DISCO_LAYOUT_ALG_ID = "org.eclipse.elk.disco.componentCompaction.componentLayoutAlgorithm";

  protected void completePropertyValue(final ContentAssistContext context, final IIdeContentProposalAcceptor acceptor) {
    final EObject model = context.getCurrentModel();
    if ((model instanceof ElkPropertyToValueMapEntryImpl)) {
      final IProperty<?> option = ((ElkPropertyToValueMapEntryImpl)model).getKey();
      if ((option instanceof LayoutOptionData)) {
        if ((Objects.equal(CoreOptions.ALGORITHM, option) || Objects.equal(((LayoutOptionData)option).getId(), ElkGraphProposalProvider.DISCO_LAYOUT_ALG_ID))) {
          this.proposeAlgorithms(context, acceptor);
        } else {
          this.typeAwarePropertyValueProposal(((LayoutOptionData)option), context, acceptor);
        }
      }
    }
  }

  private void typeAwarePropertyValueProposal(final LayoutOptionData property, final ContentAssistContext context, final IIdeContentProposalAcceptor acceptor) {
    final Consumer<LayoutDataContentAssist.Proposal<Object>> _function = new Consumer<LayoutDataContentAssist.Proposal<Object>>() {
      @Override
      public void accept(final LayoutDataContentAssist.Proposal<Object> p) {
        final Procedure1<ContentAssistEntry> _function = new Procedure1<ContentAssistEntry>() {
          @Override
          public void apply(final ContentAssistEntry it) {
            it.setLabel(p.label);
            it.setSource(property);
          }
        };
        final ContentAssistEntry entry = ElkGraphProposalProvider.this.getProposalCreator().createProposal(p.proposal, context, ContentAssistEntry.KIND_VALUE, _function);
        acceptor.accept(entry, ElkGraphProposalProvider.this.getProposalPriorities().getDefaultPriority(entry));
      }
    };
    LayoutDataContentAssist.getLayoutOptionValueProposal(property, context.getPrefix()).forEach(_function);
  }

  protected void proposeAlgorithms(final ContentAssistContext context, final IIdeContentProposalAcceptor acceptor) {
    boolean _existsConflict = this.conflictHelper.existsConflict("a", context);
    if (_existsConflict) {
      return;
    }
    final Consumer<LayoutDataContentAssist.Proposal<LayoutAlgorithmData>> _function = new Consumer<LayoutDataContentAssist.Proposal<LayoutAlgorithmData>>() {
      @Override
      public void accept(final LayoutDataContentAssist.Proposal<LayoutAlgorithmData> p) {
        ContentAssistEntry _contentAssistEntry = new ContentAssistEntry();
        final Procedure1<ContentAssistEntry> _function = new Procedure1<ContentAssistEntry>() {
          @Override
          public void apply(final ContentAssistEntry it) {
            it.setProposal(ElkGraphProposalProvider.this.convert(Strings.split(p.proposal, ".")));
            it.setPrefix(context.getPrefix());
            it.setKind(ContentAssistEntry.KIND_VALUE);
            String _elvis = null;
            if (p.label != null) {
              _elvis = p.label;
            } else {
              _elvis = p.proposal;
            }
            it.setLabel(_elvis);
            it.setDescription(ElkGraphProposalProvider.this.getDescription(p.data));
            it.setDocumentation(p.data.getDescription());
            it.setSource(p.data);
          }
        };
        final ContentAssistEntry entry = ObjectExtensions.<ContentAssistEntry>operator_doubleArrow(_contentAssistEntry, _function);
        acceptor.accept(entry, ElkGraphProposalProvider.this.getProposalPriorities().getDefaultPriority(entry));
      }
    };
    LayoutDataContentAssist.getLayoutAlgorithmProposals(context.getPrefix()).forEach(_function);
  }

  private String convert(final Iterable<String> suffix) {
    final Function1<String, String> _function = new Function1<String, String>() {
      @Override
      public String apply(final String it) {
        return ElkGraphProposalProvider.this.idValueConverter.toString(it);
      }
    };
    return IterableExtensions.join(IterableExtensions.<String, String>map(suffix, _function), ".");
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

  @Override
  protected Predicate<IEObjectDescription> getCrossrefFilter(final CrossReference reference, final ContentAssistContext context) {
    final EObject model = context.getCurrentModel();
    if ((model instanceof ElkEdgeSection)) {
      boolean _matched = false;
      CrossReference _incomingShapeElkConnectableShapeCrossReference_4_0_0_2_0 = this.grammar.getElkEdgeSectionAccess().getIncomingShapeElkConnectableShapeCrossReference_4_0_0_2_0();
      if (Objects.equal(reference, _incomingShapeElkConnectableShapeCrossReference_4_0_0_2_0)) {
        _matched=true;
      }
      if (!_matched) {
        CrossReference _incomingShapeElkConnectableShapeCrossReference_1_0_0_2_0 = this.grammar.getElkSingleEdgeSectionAccess().getIncomingShapeElkConnectableShapeCrossReference_1_0_0_2_0();
        if (Objects.equal(reference, _incomingShapeElkConnectableShapeCrossReference_1_0_0_2_0)) {
          _matched=true;
        }
      }
      if (_matched) {
        return new ElkGraphProposalProvider.SectionShapeFilter(((ElkEdgeSection)model), ElkGraphProposalProvider.SectionShapeFilter.INCOMING);
      }
      if (!_matched) {
        CrossReference _outgoingShapeElkConnectableShapeCrossReference_4_0_1_2_0 = this.grammar.getElkEdgeSectionAccess().getOutgoingShapeElkConnectableShapeCrossReference_4_0_1_2_0();
        if (Objects.equal(reference, _outgoingShapeElkConnectableShapeCrossReference_4_0_1_2_0)) {
          _matched=true;
        }
        if (!_matched) {
          CrossReference _outgoingShapeElkConnectableShapeCrossReference_1_0_1_2_0 = this.grammar.getElkSingleEdgeSectionAccess().getOutgoingShapeElkConnectableShapeCrossReference_1_0_1_2_0();
          if (Objects.equal(reference, _outgoingShapeElkConnectableShapeCrossReference_1_0_1_2_0)) {
            _matched=true;
          }
        }
        if (_matched) {
          return new ElkGraphProposalProvider.SectionShapeFilter(((ElkEdgeSection)model), ElkGraphProposalProvider.SectionShapeFilter.OUTGOING);
        }
      }
    }
    final URI resourceURI = model.eResource().getURI();
    final Predicate<IEObjectDescription> _function = new Predicate<IEObjectDescription>() {
      @Override
      public boolean apply(final IEObjectDescription candidate) {
        URI _trimFragment = candidate.getEObjectURI().trimFragment();
        return Objects.equal(_trimFragment, resourceURI);
      }
    };
    return _function;
  }
}
