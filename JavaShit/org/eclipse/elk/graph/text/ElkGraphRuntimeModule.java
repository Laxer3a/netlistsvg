/**
 * Copyright (c) 2016 TypeFox GmbH (http://www.typefox.io) and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.text;

import org.eclipse.elk.graph.text.conversion.ElkGraphValueConverterService;
import org.eclipse.elk.graph.text.linking.ElkGraphLinker;
import org.eclipse.elk.graph.text.naming.ElkGraphQualifiedNameConverter;
import org.eclipse.elk.graph.text.naming.ElkGraphQualifiedNameProvider;
import org.eclipse.elk.graph.text.serializer.ElkGraphTransientValueService;
import org.eclipse.xtext.conversion.IValueConverterService;
import org.eclipse.xtext.linking.ILinker;
import org.eclipse.xtext.naming.IQualifiedNameConverter;
import org.eclipse.xtext.naming.IQualifiedNameProvider;
import org.eclipse.xtext.serializer.sequencer.ITransientValueService;

/**
 * Use this class to register components for the ElkGraph language.
 */
@SuppressWarnings("all")
public class ElkGraphRuntimeModule extends AbstractElkGraphRuntimeModule {
  @Override
  public Class<? extends IValueConverterService> bindIValueConverterService() {
    return ElkGraphValueConverterService.class;
  }

  @Override
  public Class<? extends IQualifiedNameProvider> bindIQualifiedNameProvider() {
    return ElkGraphQualifiedNameProvider.class;
  }

  public Class<? extends IQualifiedNameConverter> bindIQualifiedNameConverter() {
    return ElkGraphQualifiedNameConverter.class;
  }

  public Class<? extends ITransientValueService> bindITransientValueService2() {
    return ElkGraphTransientValueService.class;
  }

  @Override
  public Class<? extends ILinker> bindILinker() {
    return ElkGraphLinker.class;
  }
}
