/**
 * Copyright (c) 2017 TypeFox GmbH (http://www.typefox.io) and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.text.linking;

import java.util.Collections;
import java.util.Set;
import org.eclipse.elk.graph.ElkGraphPackage;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EReference;
import org.eclipse.xtext.linking.impl.Linker;
import org.eclipse.xtext.xbase.lib.CollectionLiterals;

/**
 * Cross-reference linker for the ElkGraph language.
 */
@SuppressWarnings("all")
public class ElkGraphLinker extends Linker {
  private static final Set<EReference> OPPOSITE_REFS = Collections.<EReference>unmodifiableSet(CollectionLiterals.<EReference>newHashSet(ElkGraphPackage.Literals.ELK_CONNECTABLE_SHAPE__INCOMING_EDGES, ElkGraphPackage.Literals.ELK_CONNECTABLE_SHAPE__OUTGOING_EDGES, ElkGraphPackage.Literals.ELK_EDGE_SECTION__INCOMING_SECTIONS));

  @Override
  protected void clearReference(final EObject obj, final EReference ref) {
    boolean _contains = ElkGraphLinker.OPPOSITE_REFS.contains(ref);
    boolean _not = (!_contains);
    if (_not) {
      super.clearReference(obj, ref);
    }
  }
}
