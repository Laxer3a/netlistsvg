/**
 * Copyright (c) 2020 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.json.text;

import org.eclipse.elk.graph.json.text.conversion.ElkGraphJsonValueConverterService;
import org.eclipse.elk.graph.json.text.naming.ElkGraphJsonQualifiedNameProvider;
import org.eclipse.elk.graph.json.text.serializer.ElkGraphJsonTransientValueService;
import org.eclipse.xtext.conversion.IValueConverterService;
import org.eclipse.xtext.naming.IQualifiedNameProvider;
import org.eclipse.xtext.serializer.sequencer.ITransientValueService;

/**
 * Use this class to register components to be used at runtime / without the Equinox extension registry.
 */
@SuppressWarnings("all")
public class ElkGraphJsonRuntimeModule extends AbstractElkGraphJsonRuntimeModule {
  @Override
  public Class<? extends IValueConverterService> bindIValueConverterService() {
    return ElkGraphJsonValueConverterService.class;
  }

  @Override
  public Class<? extends IQualifiedNameProvider> bindIQualifiedNameProvider() {
    return ElkGraphJsonQualifiedNameProvider.class;
  }

  public Class<? extends ITransientValueService> bindITransientValueService2() {
    return ElkGraphJsonTransientValueService.class;
  }
}
