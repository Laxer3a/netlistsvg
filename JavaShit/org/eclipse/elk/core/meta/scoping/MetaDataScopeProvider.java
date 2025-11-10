/**
 * Copyright (c) 2016 TypeFox GmbH and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.core.meta.scoping;

import com.google.common.base.Objects;
import com.google.inject.Inject;
import org.eclipse.elk.core.meta.metaData.MetaDataPackage;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EReference;
import org.eclipse.xtext.scoping.IScope;
import org.eclipse.xtext.scoping.impl.ImportedNamespaceAwareLocalScopeProvider;

/**
 * Cross-references that do not target JVM elements must be scoped with the default Xtext means
 * instead of the Xbase scoping.
 */
@SuppressWarnings("all")
public class MetaDataScopeProvider extends AbstractMetaDataScopeProvider {
  @Inject
  private ImportedNamespaceAwareLocalScopeProvider delegate;

  @Override
  public IScope getScope(final EObject context, final EReference reference) {
    IScope _switchResult = null;
    boolean _matched = false;
    if (Objects.equal(reference, MetaDataPackage.Literals.MD_OPTION_DEPENDENCY__TARGET)) {
      _matched=true;
    }
    if (!_matched) {
      if (Objects.equal(reference, MetaDataPackage.Literals.MD_OPTION_SUPPORT__OPTION)) {
        _matched=true;
      }
    }
    if (_matched) {
      _switchResult = this.delegate.getScope(context, reference);
    }
    if (!_matched) {
      _switchResult = super.getScope(context, reference);
    }
    return _switchResult;
  }
}
