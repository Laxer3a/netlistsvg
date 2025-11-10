/**
 * Copyright (c) 2016 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.core.meta.ide;

import com.google.inject.Guice;
import com.google.inject.Injector;
import org.eclipse.elk.core.meta.MetaDataRuntimeModule;
import org.eclipse.elk.core.meta.MetaDataStandaloneSetup;
import org.eclipse.xtext.util.Modules2;

/**
 * Initialization support for running Xtext languages as language servers.
 */
@SuppressWarnings("all")
public class MetaDataIdeSetup extends MetaDataStandaloneSetup {
  @Override
  public Injector createInjector() {
    MetaDataRuntimeModule _metaDataRuntimeModule = new MetaDataRuntimeModule();
    MetaDataIdeModule _metaDataIdeModule = new MetaDataIdeModule();
    return Guice.createInjector(Modules2.mixin(_metaDataRuntimeModule, _metaDataIdeModule));
  }
}
