/**
 * Copyright (c) 2020 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.json.text.ide;

import com.google.inject.Guice;
import com.google.inject.Injector;
import org.eclipse.elk.graph.json.text.ElkGraphJsonRuntimeModule;
import org.eclipse.elk.graph.json.text.ElkGraphJsonStandaloneSetup;
import org.eclipse.xtext.util.Modules2;

/**
 * Initialization support for running Xtext languages as language servers.
 */
@SuppressWarnings("all")
public class ElkGraphJsonIdeSetup extends ElkGraphJsonStandaloneSetup {
  @Override
  public Injector createInjector() {
    ElkGraphJsonRuntimeModule _elkGraphJsonRuntimeModule = new ElkGraphJsonRuntimeModule();
    ElkGraphJsonIdeModule _elkGraphJsonIdeModule = new ElkGraphJsonIdeModule();
    return Guice.createInjector(Modules2.mixin(_elkGraphJsonRuntimeModule, _elkGraphJsonIdeModule));
  }
}
