/**
 * Copyright (c) 2020 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.json.text.conversion;

import com.google.inject.Inject;
import org.eclipse.elk.graph.properties.IProperty;
import org.eclipse.xtext.common.services.DefaultTerminalConverters;
import org.eclipse.xtext.conversion.IValueConverter;
import org.eclipse.xtext.conversion.ValueConverter;

@SuppressWarnings("all")
public class ElkGraphJsonValueConverterService extends DefaultTerminalConverters {
  @Inject
  private NumberValueConverter numberValueConverter;

  @ValueConverter(rule = "Number")
  public IValueConverter<Double> Number() {
    return this.numberValueConverter;
  }

  @Inject
  private PropertyKeyValueConverter propertyKeyValueConverter;

  @ValueConverter(rule = "PropertyKey")
  public IValueConverter<IProperty<?>> PropertyKey() {
    return this.propertyKeyValueConverter;
  }

  @Inject
  private PropertyValueValueConverter propertyValueValueConverter;

  @ValueConverter(rule = "StringValue")
  public IValueConverter<Object> StringValue() {
    return this.propertyValueValueConverter;
  }

  @ValueConverter(rule = "NumberValue")
  public IValueConverter<Object> NumberValue() {
    return this.propertyValueValueConverter;
  }

  @ValueConverter(rule = "BooleanValue")
  public IValueConverter<Object> BooleanValue() {
    return this.propertyValueValueConverter;
  }
}
