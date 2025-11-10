/**
 * Copyright (c) 2017 TypeFox GmbH (http://www.typefox.io) and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.text.serializer;

import com.google.inject.Inject;
import java.util.Map;
import org.eclipse.elk.graph.ElkGraphPackage;
import org.eclipse.elk.graph.impl.ElkPropertyToValueMapEntryImpl;
import org.eclipse.elk.graph.properties.IProperty;
import org.eclipse.elk.graph.properties.IPropertyValueProxy;
import org.eclipse.elk.graph.text.services.ElkGraphGrammarAccess;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.xtext.conversion.ValueConverterException;
import org.eclipse.xtext.conversion.impl.IDValueConverter;
import org.eclipse.xtext.serializer.ISerializationContext;
import org.eclipse.xtext.serializer.acceptor.SequenceFeeder;
import org.eclipse.xtext.util.Strings;
import org.eclipse.xtext.xbase.lib.Exceptions;
import org.eclipse.xtext.xbase.lib.Functions.Function1;
import org.eclipse.xtext.xbase.lib.ListExtensions;

@SuppressWarnings("all")
public class ElkGraphSemanticSequencer extends AbstractElkGraphSemanticSequencer {
  @Inject
  private ElkGraphGrammarAccess grammarAccess;

  private IDValueConverter idValueConverter;

  @Override
  protected void sequence_Property(final ISerializationContext context, final Map.Entry semanticObject) {
    if ((semanticObject instanceof ElkPropertyToValueMapEntryImpl)) {
      final IProperty<?> key = ((ElkPropertyToValueMapEntryImpl)semanticObject).getKey();
      final Object value = ((ElkPropertyToValueMapEntryImpl)semanticObject).getValue();
      if (((value instanceof IPropertyValueProxy) && (key != null))) {
        final Object resolvedValue = ((IPropertyValueProxy) value).resolveValue(key);
        if ((resolvedValue != null)) {
          ((ElkPropertyToValueMapEntryImpl)semanticObject).setValue(resolvedValue);
        }
      }
      if (((this.errorAcceptor != null) && (key == null))) {
        this.errorAcceptor.accept(this.diagnosticProvider.createFeatureValueMissing(((EObject)semanticObject), ElkGraphPackage.Literals.ELK_PROPERTY_TO_VALUE_MAP_ENTRY__KEY));
      }
      final SequenceFeeder feeder = this.createSequencerFeeder(context, ((EObject)semanticObject));
      feeder.accept(this.grammarAccess.getPropertyAccess().getKeyPropertyKeyParserRuleCall_0_0(), key);
      if ((value instanceof Boolean)) {
        feeder.accept(this.grammarAccess.getPropertyAccess().getValueBooleanValueParserRuleCall_2_3_0(), value);
      } else {
        if ((value instanceof Number)) {
          feeder.accept(this.grammarAccess.getPropertyAccess().getValueNumberValueParserRuleCall_2_2_0(), value);
        } else {
          if ((value != null)) {
            final String string = value.toString();
            boolean _isQuotingNecessary = this.isQuotingNecessary(string);
            if (_isQuotingNecessary) {
              feeder.accept(this.grammarAccess.getPropertyAccess().getValueStringValueParserRuleCall_2_0_0(), value);
            } else {
              feeder.accept(this.grammarAccess.getPropertyAccess().getValueQualifiedIdValueParserRuleCall_2_1_0(), value);
            }
          }
        }
      }
      feeder.finish();
    } else {
      super.sequence_Property(context, semanticObject);
    }
  }

  private boolean isQuotingNecessary(final String s) {
    try {
      final Function1<String, String> _function = new Function1<String, String>() {
        @Override
        public String apply(final String it) {
          return ElkGraphSemanticSequencer.this.idValueConverter.toString(it);
        }
      };
      /* ListExtensions.<String, String>map(Strings.split(s, "."), _function); */
      return false;
    } catch (final Throwable _t) {
      if (_t instanceof ValueConverterException) {
        return true;
      } else {
        throw Exceptions.sneakyThrow(_t);
      }
    }
  }
}
