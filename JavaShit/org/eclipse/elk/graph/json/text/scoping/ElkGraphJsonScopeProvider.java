/**
 * Copyright (c) 2020 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.json.text.scoping;

import com.google.common.base.Function;
import com.google.common.base.Objects;
import org.eclipse.elk.graph.ElkEdge;
import org.eclipse.elk.graph.ElkEdgeSection;
import org.eclipse.elk.graph.ElkGraphPackage;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EReference;
import org.eclipse.xtext.naming.QualifiedName;
import org.eclipse.xtext.scoping.IScope;
import org.eclipse.xtext.scoping.Scopes;

/**
 * This class contains custom scoping description.
 * 
 * See https://www.eclipse.org/Xtext/documentation/303_runtime_concepts.html#scoping
 * on how and when to use it.
 */
@SuppressWarnings("all")
public class ElkGraphJsonScopeProvider extends AbstractElkGraphJsonScopeProvider {
  @Override
  public IScope getScope(final EObject context, final EReference reference) {
    IScope _switchResult = null;
    boolean _matched = false;
    if (Objects.equal(reference, ElkGraphPackage.Literals.ELK_EDGE_SECTION__OUTGOING_SECTIONS)) {
      _matched=true;
      _switchResult = this.getScopeForEdgeSection(((ElkEdgeSection) context));
    }
    if (!_matched) {
      _switchResult = super.getScope(context, reference);
    }
    return _switchResult;
  }

  private IScope getScopeForEdgeSection(final ElkEdgeSection section) {
    final ElkEdge edge = section.getParent();
    final Function<ElkEdgeSection, QualifiedName> _function = new Function<ElkEdgeSection, QualifiedName>() {
      @Override
      public QualifiedName apply(final ElkEdgeSection it) {
        return QualifiedName.create(it.getIdentifier());
      }
    };
    return Scopes.<ElkEdgeSection>scopeFor(edge.getSections(), _function, IScope.NULLSCOPE);
  }
}
