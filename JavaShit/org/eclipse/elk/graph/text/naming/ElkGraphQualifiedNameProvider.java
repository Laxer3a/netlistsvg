/**
 * Copyright (c) 2016 TypeFox GmbH (http://www.typefox.io) and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.text.naming;

import org.eclipse.elk.graph.ElkEdgeSection;
import org.eclipse.elk.graph.ElkGraphElement;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.xtext.naming.DefaultDeclarativeQualifiedNameProvider;
import org.eclipse.xtext.naming.QualifiedName;

@SuppressWarnings("all")
public class ElkGraphQualifiedNameProvider extends DefaultDeclarativeQualifiedNameProvider {
  public QualifiedName qualifiedName(final ElkGraphElement element) {
    return this.prependParentNames(element, QualifiedName.create(element.getIdentifier()));
  }

  public QualifiedName qualifiedName(final ElkEdgeSection section) {
    return QualifiedName.create(section.getIdentifier());
  }

  private QualifiedName prependParentNames(final EObject object, final QualifiedName name) {
    EObject curr = object;
    while ((curr.eContainer() != null)) {
      {
        curr = curr.eContainer();
        final QualifiedName parentsQualifiedName = this.getFullyQualifiedName(curr);
        if ((parentsQualifiedName != null)) {
          return parentsQualifiedName.append(name);
        }
      }
    }
    return name;
  }
}
