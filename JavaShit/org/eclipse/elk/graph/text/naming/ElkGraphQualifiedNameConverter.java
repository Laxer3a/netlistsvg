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

import com.google.inject.Inject;
import com.google.inject.Provider;
import java.util.List;
import org.eclipse.elk.graph.text.services.ElkGraphGrammarAccess;
import org.eclipse.xtext.conversion.impl.IDValueConverter;
import org.eclipse.xtext.naming.IQualifiedNameConverter;
import org.eclipse.xtext.naming.QualifiedName;
import org.eclipse.xtext.util.Strings;
import org.eclipse.xtext.xbase.lib.Functions.Function1;
import org.eclipse.xtext.xbase.lib.IterableExtensions;
import org.eclipse.xtext.xbase.lib.ListExtensions;
import org.eclipse.xtext.xbase.lib.ObjectExtensions;
import org.eclipse.xtext.xbase.lib.Procedures.Procedure1;

@SuppressWarnings("all")
public class ElkGraphQualifiedNameConverter implements IQualifiedNameConverter {
  public static final String DELIMITER = ".";

  private IDValueConverter idValueConverter;

  @Inject
  public void initialize(final Provider<IDValueConverter> idValueConverterProvider, final ElkGraphGrammarAccess grammarAccess) {
    IDValueConverter _get = idValueConverterProvider.get();
    final Procedure1<IDValueConverter> _function = new Procedure1<IDValueConverter>() {
      @Override
      public void apply(final IDValueConverter it) {
        it.setRule(grammarAccess.getIDRule());
      }
    };
    IDValueConverter _doubleArrow = ObjectExtensions.<IDValueConverter>operator_doubleArrow(_get, _function);
    this.idValueConverter = _doubleArrow;
  }

  @Override
  public QualifiedName toQualifiedName(final String qualifiedNameAsText) {
    if ((qualifiedNameAsText == null)) {
      throw new IllegalArgumentException("Qualified name cannot be null");
    }
    boolean _isEmpty = qualifiedNameAsText.isEmpty();
    if (_isEmpty) {
      throw new IllegalArgumentException("Qualified name cannot be empty");
    }
    final Function1<String, String> _function = new Function1<String, String>() {
      @Override
      public String apply(final String it) {
        return ElkGraphQualifiedNameConverter.this.idValueConverter.toValue(it, null);
      }
    };
    final List<String> segs = ListExtensions.<String, String>map(Strings.split(qualifiedNameAsText, ElkGraphQualifiedNameConverter.DELIMITER), _function);
    return QualifiedName.create(segs);
  }

  @Override
  public String toString(final QualifiedName name) {
    if ((name == null)) {
      throw new IllegalArgumentException("Qualified name cannot be null");
    }
    final Function1<String, String> _function = new Function1<String, String>() {
      @Override
      public String apply(final String it) {
        return ElkGraphQualifiedNameConverter.this.idValueConverter.toString(it);
      }
    };
    return IterableExtensions.join(ListExtensions.<String, String>map(name.getSegments(), _function), ElkGraphQualifiedNameConverter.DELIMITER);
  }
}
