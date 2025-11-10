/**
 * Copyright (c) 2017 TypeFox GmbH (http://www.typefox.io) and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.json.text.serializer;

import com.google.common.base.Objects;
import org.eclipse.elk.graph.ElkGraphPackage;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.xtext.serializer.sequencer.ITransientValueService;
import org.eclipse.xtext.serializer.sequencer.LegacyTransientValueService;

@SuppressWarnings("all")
public class ElkGraphJsonTransientValueService extends LegacyTransientValueService {
  @Override
  public ITransientValueService.ListTransient isListTransient(final EObject semanticObject, final EStructuralFeature feature) {
    boolean _matched = false;
    if (Objects.equal(feature, ElkGraphPackage.Literals.ELK_CONNECTABLE_SHAPE__INCOMING_EDGES)) {
      _matched=true;
    }
    if (!_matched) {
      if (Objects.equal(feature, ElkGraphPackage.Literals.ELK_CONNECTABLE_SHAPE__OUTGOING_EDGES)) {
        _matched=true;
      }
    }
    if (!_matched) {
      if (Objects.equal(feature, ElkGraphPackage.Literals.ELK_EDGE_SECTION__INCOMING_SECTIONS)) {
        _matched=true;
      }
    }
    if (_matched) {
      return ITransientValueService.ListTransient.YES;
    }
    return super.isListTransient(semanticObject, feature);
  }
}
