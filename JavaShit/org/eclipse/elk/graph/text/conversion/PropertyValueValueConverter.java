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

import com.google.common.base.Objects;
import com.google.inject.Inject;
import org.eclipse.elk.core.data.LayoutMetaDataService;
import org.eclipse.elk.core.data.LayoutOptionData;
import org.eclipse.elk.core.util.internal.LayoutOptionProxy;
import org.eclipse.elk.graph.impl.ElkPropertyToValueMapEntryImpl;
import org.eclipse.elk.graph.properties.IProperty;
import org.eclipse.elk.graph.properties.IPropertyValueProxy;
import org.eclipse.elk.graph.text.naming.ElkGraphQualifiedNameConverter;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.xtext.conversion.ValueConverterException;
import org.eclipse.xtext.conversion.impl.AbstractValueConverter;
import org.eclipse.xtext.naming.QualifiedName;
import org.eclipse.xtext.nodemodel.INode;
import org.eclipse.xtext.util.Strings;
import org.eclipse.xtext.xbase.lib.Exceptions;
import org.eclipse.xtext.xbase.lib.StringExtensions;

@SuppressWarnings("all")
public class PropertyValueValueConverter extends AbstractValueConverter<Object> {
  @Inject
  private ElkGraphQualifiedNameConverter qualifiedNameConverter;

  @Override
  public String toString(final Object value) throws ValueConverterException {
    Object _xifexpression = null;
    if ((value instanceof IPropertyValueProxy)) {
      _xifexpression = ((IPropertyValueProxy)value).toString();
    } else {
      _xifexpression = value;
    }
    final Object v = _xifexpression;
    if ((v == null)) {
      return "null";
    } else {
      if (((v instanceof Double) && (Math.floor((((Double) v)).doubleValue()) == (((Double) v)).doubleValue()))) {
        return Integer.toString(((Double) v).intValue());
      } else {
        if ((((v instanceof Boolean) || (v instanceof Number)) || (v instanceof Enum<?>))) {
          return v.toString();
        } else {
          if ((v instanceof String)) {
            if (((Objects.equal(v, "true") || Objects.equal(v, "false")) || Objects.equal(v, "null"))) {
              return ((String)v);
            }
            try {
              Double.parseDouble(((String)v));
              return ((String)v);
            } catch (final Throwable _t) {
              if (_t instanceof NumberFormatException) {
                return this.quoteIfNecessary(((String)v));
              } else {
                throw Exceptions.sneakyThrow(_t);
              }
            }
          } else {
            return this.quoteIfNecessary(v.toString());
          }
        }
      }
    }
  }

  private String quoteIfNecessary(final String s) {
    try {
      final QualifiedName qname = QualifiedName.create(Strings.split(s, ElkGraphQualifiedNameConverter.DELIMITER));
      return this.qualifiedNameConverter.toString(qname);
    } catch (final Throwable _t) {
      if (_t instanceof ValueConverterException) {
        return (("\"" + s) + "\"");
      } else {
        throw Exceptions.sneakyThrow(_t);
      }
    }
  }

  @Override
  public Object toValue(final String string, final INode node) throws ValueConverterException {
    if (((!StringExtensions.isNullOrEmpty(string)) && (!Objects.equal(string, "null")))) {
      final String unquotedString = this.unquoteIfNecessary(string);
      EObject _semanticElement = null;
      if (node!=null) {
        _semanticElement=node.getSemanticElement();
      }
      final EObject semanticElem = _semanticElement;
      if ((semanticElem instanceof ElkPropertyToValueMapEntryImpl)) {
        final LayoutOptionData option = this.toLayoutOption(((ElkPropertyToValueMapEntryImpl)semanticElem).getKey());
        Object _parseValue = null;
        if (option!=null) {
          _parseValue=option.parseValue(unquotedString);
        }
        final Object value = _parseValue;
        if ((value != null)) {
          return value;
        }
      }
      return new LayoutOptionProxy(unquotedString);
    }
    return null;
  }

  private String unquoteIfNecessary(final String s) {
    if (((s.length() >= 2) && ((s.startsWith("\"") && s.endsWith("\"")) || (s.startsWith("\'") && s.endsWith("\'"))))) {
      int _length = s.length();
      int _minus = (_length - 1);
      return s.substring(1, _minus);
    } else {
      if (((s.length() >= 1) && (Character.isJavaIdentifierStart(s.charAt(0)) || s.startsWith("^")))) {
        final QualifiedName qname = this.qualifiedNameConverter.toQualifiedName(s);
        return qname.toString(ElkGraphQualifiedNameConverter.DELIMITER);
      } else {
        return s;
      }
    }
  }

  private LayoutOptionData toLayoutOption(final IProperty<?> property) {
    if ((property instanceof LayoutOptionData)) {
      return ((LayoutOptionData)property);
    } else {
      if ((property != null)) {
        return LayoutMetaDataService.getInstance().getOptionData(property.getId());
      }
    }
    return null;
  }
}
