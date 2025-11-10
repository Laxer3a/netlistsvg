/**
 * Copyright (c) 2017 TypeFox GmbH (http://www.typefox.io) and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.text.ide;

import com.google.inject.Guice;
import com.google.inject.Injector;
import org.eclipse.elk.graph.text.ElkGraphRuntimeModule;
import org.eclipse.elk.graph.text.ElkGraphStandaloneSetup;
import org.eclipse.xtext.util.Modules2;

/**
 * Initialization support for running Xtext languages as language servers.
 */
@SuppressWarnings("all")
public class ElkGraphIdeSetup extends ElkGraphStandaloneSetup {
  @Override
  public Injector createInjector() {
    ElkGraphRuntimeModule _elkGraphRuntimeModule = new ElkGraphRuntimeModule();
    ElkGraphIdeModule _elkGraphIdeModule = new ElkGraphIdeModule();
    return Guice.createInjector(Modules2.mixin(_elkGraphRuntimeModule, _elkGraphIdeModule));
  }
}
