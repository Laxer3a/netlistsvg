/**
 * Copyright (c) 2016 TypeFox GmbH (http://www.typefox.io) and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.json.text.conversion;

import com.google.inject.Inject;
import com.google.inject.Provider;
import java.util.List;
import org.eclipse.elk.core.data.LayoutMetaDataService;
import org.eclipse.elk.core.data.LayoutOptionData;
import org.eclipse.elk.graph.json.text.services.ElkGraphJsonGrammarAccess;
import org.eclipse.elk.graph.properties.IProperty;
import org.eclipse.elk.graph.properties.Property;
import org.eclipse.xtext.conversion.ValueConverterException;
import org.eclipse.xtext.conversion.impl.AbstractValueConverter;
import org.eclipse.xtext.conversion.impl.IDValueConverter;
import org.eclipse.xtext.nodemodel.INode;
import org.eclipse.xtext.util.Strings;
import org.eclipse.xtext.xbase.lib.Functions.Function1;
import org.eclipse.xtext.xbase.lib.IterableExtensions;
import org.eclipse.xtext.xbase.lib.ListExtensions;
import org.eclipse.xtext.xbase.lib.ObjectExtensions;
import org.eclipse.xtext.xbase.lib.Procedures.Procedure1;
import org.eclipse.xtext.xbase.lib.StringExtensions;

/**
 * Note that this converter is derived from ElkText's. The main difference
 * is that all values but numbers and bools are in double-quotes.
 */
@SuppressWarnings("all")
public class PropertyKeyValueConverter extends AbstractValueConverter<IProperty<?>> {
  private IDValueConverter idValueConverter;

  @Inject
  public void initialize(final Provider<IDValueConverter> idValueConverterProvider, final ElkGraphJsonGrammarAccess grammarAccess) {
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
  public String toString(final IProperty<?> value) throws ValueConverterException {
    if ((value == null)) {
      throw new ValueConverterException("IProperty value may not be null.", null, null);
    }
    final LayoutMetaDataService metaDataService = LayoutMetaDataService.getInstance();
    final List<String> split = Strings.split(value.getId(), ".");
    String suffix = null;
    int _size = split.size();
    int i = (_size - 1);
    while ((i >= 0)) {
      {
        if ((suffix == null)) {
          int _minusMinus = i--;
          suffix = this.idValueConverter.toString(split.get(_minusMinus));
        } else {
          int _minusMinus_1 = i--;
          String _string = this.idValueConverter.toString(split.get(_minusMinus_1));
          String _plus = (_string + ".");
          String _plus_1 = (_plus + suffix);
          suffix = _plus_1;
        }
        final LayoutOptionData knownOption = metaDataService.getOptionDataBySuffix(suffix);
        if (((knownOption != null) && (StringExtensions.isNullOrEmpty(knownOption.getGroup()) || suffix.contains(knownOption.getGroup())))) {
          return suffix;
        }
      }
    }
    final Function1<String, String> _function = new Function1<String, String>() {
      @Override
      public String apply(final String it) {
        return PropertyKeyValueConverter.this.idValueConverter.toString(it);
      }
    };
    String _join = IterableExtensions.join(ListExtensions.<String, String>map(split, _function), ".");
    String _plus = ("\"" + _join);
    return (_plus + "\"");
  }

  @Override
  public IProperty<?> toValue(final String string, final INode node) throws ValueConverterException {
    boolean _isNullOrEmpty = StringExtensions.isNullOrEmpty(string);
    if (_isNullOrEmpty) {
      throw new ValueConverterException("Cannot convert empty string to a property identifier.", node, null);
    }
    final Function1<String, String> _function = new Function1<String, String>() {
      @Override
      public String apply(final String it) {
        return PropertyKeyValueConverter.this.idValueConverter.toValue(it, node);
      }
    };
    final String idSuffix = IterableExtensions.join(ListExtensions.<String, String>map(Strings.split(this.unquoteIfNecessary(string), "."), _function), ".");
    final LayoutOptionData optionData = LayoutMetaDataService.getInstance().getOptionDataBySuffix(idSuffix);
    if ((optionData == null)) {
      return new Property<Object>(string);
    }
    return optionData;
  }

  private String unquoteIfNecessary(final String s) {
    if (((s.length() >= 2) && ((s.startsWith("\"") && s.endsWith("\"")) || (s.startsWith("\'") && s.endsWith("\'"))))) {
      int _length = s.length();
      int _minus = (_length - 1);
      return s.substring(1, _minus);
    } else {
      return s;
    }
  }
}
