/**
 * Copyright (c) 2016 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.core.meta.ui.outline;

import org.eclipse.elk.core.meta.metaData.MdAlgorithm;
import org.eclipse.elk.core.meta.metaData.MdBundle;
import org.eclipse.elk.core.meta.metaData.MdBundleMember;
import org.eclipse.elk.core.meta.metaData.MdCategory;
import org.eclipse.elk.core.meta.metaData.MdGroup;
import org.eclipse.elk.core.meta.metaData.MdGroupOrOption;
import org.eclipse.elk.core.meta.metaData.MdModel;
import org.eclipse.elk.core.meta.metaData.MdOption;
import org.eclipse.emf.common.util.EList;
import org.eclipse.xtext.ui.editor.outline.IOutlineNode;
import org.eclipse.xtext.ui.editor.outline.impl.DefaultOutlineTreeProvider;
import org.eclipse.xtext.ui.editor.outline.impl.DocumentRootNode;

/**
 * Customization of the default outline structure.
 * 
 * See https://www.eclipse.org/Xtext/documentation/304_ide_concepts.html#outline
 */
@SuppressWarnings("all")
public class MetaDataOutlineTreeProvider extends DefaultOutlineTreeProvider {
  protected void _createChildren(final DocumentRootNode rootNode, final MdModel model) {
    MdBundle _bundle = model.getBundle();
    boolean _tripleNotEquals = (_bundle != null);
    if (_tripleNotEquals) {
      this.createNode(rootNode, model.getBundle());
    }
  }

  protected void _createChildren(final IOutlineNode parentNode, final MdBundle bundle) {
    EList<MdBundleMember> _members = bundle.getMembers();
    for (final MdBundleMember member : _members) {
      if (((((member instanceof MdCategory) || (member instanceof MdAlgorithm)) || (member instanceof MdOption)) || (member instanceof MdGroup))) {
        this.createNode(parentNode, member);
      }
    }
  }

  protected void _createChildren(final IOutlineNode parentNode, final MdCategory category) {
    return;
  }

  protected boolean _isLeaf(final MdCategory category) {
    return true;
  }

  protected void _createChildren(final IOutlineNode parentNode, final MdAlgorithm algorithm) {
    return;
  }

  protected boolean _isLeaf(final MdAlgorithm algorithm) {
    return true;
  }

  protected void _createChildren(final IOutlineNode parentNode, final MdOption option) {
    return;
  }

  protected boolean _isLeaf(final MdOption option) {
    return true;
  }

  protected void _createChildren(final IOutlineNode parentNode, final MdGroup group) {
    EList<MdGroupOrOption> _children = group.getChildren();
    for (final MdGroupOrOption child : _children) {
      this.createNode(parentNode, child);
    }
  }
}
