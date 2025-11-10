/**
 * Copyright (c) 2020 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.json.text.ui.contentassist;

import com.google.common.collect.ImmutableList;
import com.google.inject.Inject;
import com.google.inject.Provider;
import java.util.ArrayList;
import java.util.Collections;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.viewers.StyledString;
import org.eclipse.xtext.AbstractElement;
import org.eclipse.xtext.ide.editor.contentassist.ContentAssistContext;
import org.eclipse.xtext.ide.editor.contentassist.ContentAssistEntry;
import org.eclipse.xtext.ide.editor.contentassist.IIdeContentProposalAcceptor;
import org.eclipse.xtext.ide.editor.contentassist.IdeContentProposalProvider;
import org.eclipse.xtext.ui.editor.contentassist.AbstractContentProposalProvider;
import org.eclipse.xtext.ui.editor.contentassist.ConfigurableCompletionProposal;
import org.eclipse.xtext.ui.editor.contentassist.ICompletionProposalAcceptor;
import org.eclipse.xtext.util.TextRegion;
import org.eclipse.xtext.xbase.lib.CollectionLiterals;
import org.eclipse.xtext.xbase.lib.IterableExtensions;
import org.eclipse.xtext.xbase.lib.Pair;
import org.eclipse.xtext.xbase.lib.Procedures.Procedure2;
import org.eclipse.xtext.xbase.lib.StringExtensions;

/**
 * See https://www.eclipse.org/Xtext/documentation/310_eclipse_support.html#content-assist
 * on how to customize the content assistant.
 */
@SuppressWarnings("all")
public class ElkGraphJsonProposalProvider extends AbstractElkGraphJsonProposalProvider {
  private static final int MAX_ENTRIES = 1000;

  @Inject
  private IdeContentProposalProvider ideProvider;

  @Inject
  private Provider<ContentAssistContext.Builder> builderProvider;

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
        return (_size < ElkGraphJsonProposalProvider.MAX_ENTRIES);
      }
    };
    ContentAssistContext _ideContext = this.getIdeContext(context);
    this.ideProvider.createProposals(Collections.<ContentAssistContext>unmodifiableList(CollectionLiterals.<ContentAssistContext>newArrayList(_ideContext)), ideAcceptor);
    final AbstractContentProposalProvider.NullSafeCompletionProposalAcceptor uiAcceptor = new AbstractContentProposalProvider.NullSafeCompletionProposalAcceptor(acceptor);
    final Procedure2<Pair<ContentAssistEntry, Integer>, Integer> _function = new Procedure2<Pair<ContentAssistEntry, Integer>, Integer>() {
      @Override
      public void apply(final Pair<ContentAssistEntry, Integer> p, final Integer index) {
        final ContentAssistEntry entry = p.getKey();
        final ConfigurableCompletionProposal proposal = ElkGraphJsonProposalProvider.this.doCreateProposal(entry.getProposal(), ElkGraphJsonProposalProvider.this.getDisplayString(entry), null, (p.getValue()).intValue(), context);
        uiAcceptor.accept(proposal);
      }
    };
    IterableExtensions.<Pair<ContentAssistEntry, Integer>>forEach(entries, _function);
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
}
