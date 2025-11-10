/**
 * Copyright (c) 2017 TypeFox GmbH (http://www.typefox.io) and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.text;

import org.eclipse.elk.core.data.LayoutAlgorithmData;
import org.eclipse.elk.core.data.LayoutMetaDataService;
import org.eclipse.elk.core.options.CoreOptions;
import org.eclipse.elk.graph.ElkEdge;
import org.eclipse.elk.graph.ElkGraphElement;
import org.eclipse.elk.graph.ElkLabel;
import org.eclipse.elk.graph.ElkNode;
import org.eclipse.elk.graph.ElkPort;
import org.eclipse.xtext.xbase.lib.StringExtensions;

/**
 * Utilities for the ELK Graph Text format.
 */
@SuppressWarnings("all")
public final class ElkGraphTextUtil {
  private ElkGraphTextUtil() {
  }

  /**
   * Return the algorithm that is responsible for the layout of the given element. Note that this
   * might be ambiguous: e.g. a port of a composite node can be handled both by the algorithm that
   * arranges that node and the algorithm that arranges its container node.
   */
  public static LayoutAlgorithmData getAlgorithm(final ElkGraphElement element) {
    ElkNode _switchResult = null;
    boolean _matched = false;
    if (element instanceof ElkNode) {
      _matched=true;
      _switchResult = ((ElkNode)element);
    }
    if (!_matched) {
      if (element instanceof ElkPort) {
        _matched=true;
        ElkNode _elvis = null;
        ElkNode _parent = ((ElkPort)element).getParent();
        ElkNode _parent_1 = null;
        if (_parent!=null) {
          _parent_1=_parent.getParent();
        }
        if (_parent_1 != null) {
          _elvis = _parent_1;
        } else {
          ElkNode _parent_2 = ((ElkPort)element).getParent();
          _elvis = _parent_2;
        }
        _switchResult = _elvis;
      }
    }
    if (!_matched) {
      if (element instanceof ElkEdge) {
        _matched=true;
        _switchResult = ((ElkEdge)element).getContainingNode();
      }
    }
    if (!_matched) {
      if (element instanceof ElkLabel) {
        _matched=true;
        ElkNode _xblockexpression = null;
        {
          ElkGraphElement parent = ((ElkLabel)element).getParent();
          while ((parent instanceof ElkLabel)) {
            parent = ((ElkLabel)parent).getParent();
          }
          ElkNode _switchResult_1 = null;
          boolean _matched_1 = false;
          if (parent instanceof ElkNode) {
            _matched_1=true;
            ElkNode _elvis = null;
            ElkNode _parent = ((ElkNode)parent).getParent();
            if (_parent != null) {
              _elvis = _parent;
            } else {
              _elvis = ((ElkNode)parent);
            }
            _switchResult_1 = _elvis;
          }
          if (!_matched_1) {
            if (parent instanceof ElkPort) {
              _matched_1=true;
              ElkNode _elvis = null;
              ElkNode _parent = ((ElkPort)parent).getParent();
              ElkNode _parent_1 = null;
              if (_parent!=null) {
                _parent_1=_parent.getParent();
              }
              if (_parent_1 != null) {
                _elvis = _parent_1;
              } else {
                ElkNode _parent_2 = ((ElkPort)parent).getParent();
                _elvis = _parent_2;
              }
              _switchResult_1 = _elvis;
            }
          }
          if (!_matched_1) {
            if (parent instanceof ElkEdge) {
              _matched_1=true;
              _switchResult_1 = ((ElkEdge)parent).getContainingNode();
            }
          }
          _xblockexpression = _switchResult_1;
        }
        _switchResult = _xblockexpression;
      }
    }
    final ElkNode node = _switchResult;
    if ((node != null)) {
      final String algorithmId = node.<String>getProperty(CoreOptions.ALGORITHM);
      boolean _isNullOrEmpty = StringExtensions.isNullOrEmpty(algorithmId);
      boolean _not = (!_isNullOrEmpty);
      if (_not) {
        return LayoutMetaDataService.getInstance().getAlgorithmDataBySuffix(algorithmId);
      }
    }
    return null;
  }
}
