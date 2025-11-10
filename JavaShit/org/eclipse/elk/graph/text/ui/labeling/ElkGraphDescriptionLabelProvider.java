/**
 * Copyright (c) 2016 TypeFox GmbH (http://www.typefox.io) and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.text.ui.labeling;

import com.google.common.base.Objects;
import org.eclipse.elk.graph.ElkGraphPackage;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.xtext.ui.label.DefaultDescriptionLabelProvider;

/**
 * Provides labels for IEObjectDescriptions and IResourceDescriptions.
 */
@SuppressWarnings("all")
public class ElkGraphDescriptionLabelProvider extends DefaultDescriptionLabelProvider {
  public String image(final EClass clazz) {
    String _switchResult = null;
    boolean _matched = false;
    if (Objects.equal(clazz, ElkGraphPackage.Literals.ELK_NODE)) {
      _matched=true;
      _switchResult = "elknode.gif";
    }
    if (!_matched) {
      if (Objects.equal(clazz, ElkGraphPackage.Literals.ELK_EDGE)) {
        _matched=true;
        _switchResult = "elkedge.gif";
      }
    }
    if (!_matched) {
      if (Objects.equal(clazz, ElkGraphPackage.Literals.ELK_PORT)) {
        _matched=true;
        _switchResult = "elkport.gif";
      }
    }
    if (!_matched) {
      if (Objects.equal(clazz, ElkGraphPackage.Literals.ELK_LABEL)) {
        _matched=true;
        _switchResult = "elklabel.gif";
      }
    }
    return _switchResult;
  }
}
