/**
 * Copyright (c) 2017 TypeFox GmbH (http://www.typefox.io) and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.json.text.ui;

import com.google.common.base.Objects;
import com.google.inject.Guice;
import com.google.inject.Injector;
import org.apache.log4j.Logger;
import org.eclipse.elk.graph.json.text.ide.ElkGraphJsonIdeModule;
import org.eclipse.elk.graph.json.text.ui.internal.TextActivator;
import org.eclipse.xtext.util.Modules2;
import org.eclipse.xtext.xbase.lib.Exceptions;

@SuppressWarnings("all")
public class ElkGraphJsonTextUiActivator extends TextActivator {
  protected com.google.inject.Module getIdeModule(final String grammar) {
    boolean _equals = Objects.equal(grammar, TextActivator.ORG_ECLIPSE_ELK_GRAPH_JSON_TEXT_ELKGRAPHJSON);
    if (_equals) {
      return new ElkGraphJsonIdeModule();
    }
    throw new IllegalArgumentException(grammar);
  }

  @Override
  protected Injector createInjector(final String language) {
    try {
      final com.google.inject.Module runtimeModule = this.getRuntimeModule(language);
      final com.google.inject.Module ideModule = this.getIdeModule(language);
      final com.google.inject.Module sharedStateModule = this.getSharedStateModule();
      final com.google.inject.Module uiModule = this.getUiModule(language);
      final com.google.inject.Module mergedModule = Modules2.mixin(runtimeModule, ideModule, sharedStateModule, uiModule);
      return Guice.createInjector(mergedModule);
    } catch (final Throwable _t) {
      if (_t instanceof Exception) {
        final Exception e = (Exception)_t;
        final Logger logger = Logger.getLogger(ElkGraphJsonTextUiActivator.class);
        logger.error(("Failed to create injector for " + language), e);
        throw new RuntimeException(("Failed to create injector for " + language), e);
      } else {
        throw Exceptions.sneakyThrow(_t);
      }
    }
  }
}
