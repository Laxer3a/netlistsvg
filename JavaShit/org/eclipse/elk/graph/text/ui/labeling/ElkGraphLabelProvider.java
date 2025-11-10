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

import com.google.inject.Inject;
import org.eclipse.elk.graph.ElkConnectableShape;
import org.eclipse.elk.graph.ElkEdge;
import org.eclipse.elk.graph.ElkLabel;
import org.eclipse.elk.graph.ElkNode;
import org.eclipse.elk.graph.ElkPort;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider;
import org.eclipse.xtext.naming.IQualifiedNameProvider;
import org.eclipse.xtext.naming.QualifiedName;
import org.eclipse.xtext.ui.label.DefaultEObjectLabelProvider;
import org.eclipse.xtext.xbase.lib.Extension;
import org.eclipse.xtext.xbase.lib.IterableExtensions;
import org.eclipse.xtext.xbase.lib.Procedures.Procedure2;
import org.eclipse.xtext.xbase.lib.StringExtensions;

/**
 * Provides labels for ElkGraph classes.
 */
@SuppressWarnings("all")
public class ElkGraphLabelProvider extends DefaultEObjectLabelProvider {
  @Inject
  @Extension
  private IQualifiedNameProvider _iQualifiedNameProvider;

  @Inject
  public ElkGraphLabelProvider(final AdapterFactoryLabelProvider delegate) {
    super(delegate);
  }

  public String image(final ElkNode node) {
    String _xifexpression = null;
    if (((!node.eIsProxy()) && (node.getParent() == null))) {
      _xifexpression = "elkgraph.gif";
    } else {
      _xifexpression = "elknode.gif";
    }
    return _xifexpression;
  }

  public String image(final ElkPort port) {
    return "elkport.gif";
  }

  public String text(final ElkEdge edge) {
    final StringBuilder result = new StringBuilder();
    boolean _isNullOrEmpty = StringExtensions.isNullOrEmpty(edge.getIdentifier());
    boolean _not = (!_isNullOrEmpty);
    if (_not) {
      result.append(edge.getIdentifier()).append(": ");
    }
    final Procedure2<ElkConnectableShape, Integer> _function = new Procedure2<ElkConnectableShape, Integer>() {
      @Override
      public void apply(final ElkConnectableShape s, final Integer i) {
        if (((i).intValue() > 0)) {
          result.append(", ");
        }
        String _elvis = null;
        QualifiedName _fullyQualifiedName = ElkGraphLabelProvider.this._iQualifiedNameProvider.getFullyQualifiedName(s);
        String _string = null;
        if (_fullyQualifiedName!=null) {
          _string=_fullyQualifiedName.toString();
        }
        if (_string != null) {
          _elvis = _string;
        } else {
          _elvis = "?";
        }
        result.append(_elvis);
      }
    };
    IterableExtensions.<ElkConnectableShape>forEach(edge.getSources(), _function);
    result.append(" → ");
    final Procedure2<ElkConnectableShape, Integer> _function_1 = new Procedure2<ElkConnectableShape, Integer>() {
      @Override
      public void apply(final ElkConnectableShape t, final Integer i) {
        if (((i).intValue() > 0)) {
          result.append(", ");
        }
        String _elvis = null;
        QualifiedName _fullyQualifiedName = ElkGraphLabelProvider.this._iQualifiedNameProvider.getFullyQualifiedName(t);
        String _string = null;
        if (_fullyQualifiedName!=null) {
          _string=_fullyQualifiedName.toString();
        }
        if (_string != null) {
          _elvis = _string;
        } else {
          _elvis = "?";
        }
        result.append(_elvis);
      }
    };
    IterableExtensions.<ElkConnectableShape>forEach(edge.getTargets(), _function_1);
    return result.toString();
  }

  public String image(final ElkEdge edge) {
    return "elkedge.gif";
  }

  public String text(final ElkLabel label) {
    final StringBuilder result = new StringBuilder();
    boolean _isNullOrEmpty = StringExtensions.isNullOrEmpty(label.getIdentifier());
    boolean _not = (!_isNullOrEmpty);
    if (_not) {
      result.append(label.getIdentifier()).append(": ");
    }
    StringBuilder _append = result.append("\"");
    String _elvis = null;
    String _text = label.getText();
    if (_text != null) {
      _elvis = _text;
    } else {
      _elvis = "";
    }
    _append.append(_elvis).append("\"");
    return result.toString();
  }

  public String image(final ElkLabel label) {
    return "elklabel.gif";
  }
}
