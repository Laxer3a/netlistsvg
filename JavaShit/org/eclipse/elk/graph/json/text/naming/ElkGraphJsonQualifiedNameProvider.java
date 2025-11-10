/**
 * Copyright (c) 2020 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.json.text.naming;

import org.eclipse.elk.graph.ElkGraphElement;
import org.eclipse.xtext.naming.DefaultDeclarativeQualifiedNameProvider;
import org.eclipse.xtext.naming.QualifiedName;

@SuppressWarnings("all")
public class ElkGraphJsonQualifiedNameProvider extends DefaultDeclarativeQualifiedNameProvider {
  public QualifiedName qualifiedName(final ElkGraphElement element) {
    return QualifiedName.create(element.getIdentifier());
  }
}
