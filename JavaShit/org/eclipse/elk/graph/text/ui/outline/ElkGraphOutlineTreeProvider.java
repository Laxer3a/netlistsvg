/**
 * Copyright (c) 2016 TypeFox GmbH (http://www.typefox.io) and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.text.ui.outline;

import java.util.Arrays;
import org.eclipse.elk.graph.ElkEdge;
import org.eclipse.elk.graph.ElkLabel;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.xtext.ui.editor.outline.IOutlineNode;
import org.eclipse.xtext.ui.editor.outline.impl.DefaultOutlineTreeProvider;
import org.eclipse.xtext.ui.editor.outline.impl.DocumentRootNode;
import org.eclipse.xtext.ui.editor.outline.impl.EStructuralFeatureNode;

/**
 * Customization of the outline tree structure.
 */
@SuppressWarnings("all")
public class ElkGraphOutlineTreeProvider extends DefaultOutlineTreeProvider {
  protected void _createChildren(final IOutlineNode parentNode, final ElkEdge edge) {
    EList<ElkLabel> _labels = edge.getLabels();
    for (final ElkLabel label : _labels) {
      this.createEObjectNode(parentNode, label);
    }
  }

  public void createChildren(final IOutlineNode parentNode, final EObject edge) {
    if (parentNode instanceof DocumentRootNode
         && edge != null) {
      _createChildren((DocumentRootNode)parentNode, edge);
      return;
    } else if (parentNode instanceof EStructuralFeatureNode
         && edge != null) {
      _createChildren((EStructuralFeatureNode)parentNode, edge);
      return;
    } else if (parentNode != null
         && edge instanceof ElkEdge) {
      _createChildren(parentNode, (ElkEdge)edge);
      return;
    } else if (parentNode != null
         && edge != null) {
      _createChildren(parentNode, edge);
      return;
    } else if (parentNode != null
         && edge != null) {
      _createChildren(parentNode, edge);
      return;
    } else {
      throw new IllegalArgumentException("Unhandled parameter types: " +
        Arrays.<Object>asList(parentNode, edge).toString());
    }
  }
}
