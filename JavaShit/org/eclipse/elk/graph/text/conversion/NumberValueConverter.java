/**
 * Copyright (c) 2016 TypeFox GmbH (http://www.typefox.io) and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.text.conversion;

import org.eclipse.xtext.conversion.ValueConverterException;
import org.eclipse.xtext.conversion.impl.AbstractValueConverter;
import org.eclipse.xtext.nodemodel.INode;
import org.eclipse.xtext.xbase.lib.Exceptions;
import org.eclipse.xtext.xbase.lib.StringExtensions;

@SuppressWarnings("all")
public class NumberValueConverter extends AbstractValueConverter<Double> {
  @Override
  public String toString(final Double value) throws ValueConverterException {
    if ((value == null)) {
      throw new ValueConverterException("Double value may not be null.", null, null);
    } else {
      double _floor = Math.floor((value).doubleValue());
      boolean _equals = (_floor == (value).doubleValue());
      if (_equals) {
        return Integer.toString(value.intValue());
      } else {
        return value.toString();
      }
    }
  }

  @Override
  public Double toValue(final String string, final INode node) throws ValueConverterException {
    boolean _isNullOrEmpty = StringExtensions.isNullOrEmpty(string);
    if (_isNullOrEmpty) {
      throw new ValueConverterException("Cannot convert empty string to a double value.", node, null);
    }
    try {
      return Double.valueOf(string);
    } catch (final Throwable _t) {
      if (_t instanceof NumberFormatException) {
        final NumberFormatException e = (NumberFormatException)_t;
        throw new ValueConverterException((("Cannot convert \'" + string) + "\' to a double value."), node, e);
      } else {
        throw Exceptions.sneakyThrow(_t);
      }
    }
  }
}
