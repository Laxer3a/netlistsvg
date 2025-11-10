/**
 * Copyright (c) 2016 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.core.meta;

import com.google.inject.Binder;
import java.util.Set;
import org.eclipse.elk.core.meta.jvmmodel.MelkDocumentationGenerator;
import org.eclipse.xtext.generator.IGenerator;
import org.eclipse.xtext.generator.IOutputConfigurationProvider;
import org.eclipse.xtext.generator.OutputConfiguration;
import org.eclipse.xtext.generator.OutputConfigurationProvider;

/**
 * Use this class to register components to be used at runtime / without the Equinox extension registry.
 */
@SuppressWarnings("all")
public class MetaDataRuntimeModule extends AbstractMetaDataRuntimeModule {
  public static class MelkOutputConfigurationProvider extends OutputConfigurationProvider {
    public static String AD_INPUT = "AD_INPUT";

    @Override
    public Set<OutputConfiguration> getOutputConfigurations() {
      Set<OutputConfiguration> _xblockexpression = null;
      {
        final Set<OutputConfiguration> configurations = super.getOutputConfigurations();
        final OutputConfiguration input = new OutputConfiguration(MetaDataRuntimeModule.MelkOutputConfigurationProvider.AD_INPUT);
        input.setDescription("Additional Documentation Input Folder");
        input.setOutputDirectory("./");
        configurations.add(input);
        _xblockexpression = configurations;
      }
      return _xblockexpression;
    }
  }

  @Override
  public Class<? extends IGenerator> bindIGenerator() {
    return MelkDocumentationGenerator.class;
  }

  @Override
  public void configure(final Binder binder) {
    super.configure(binder);
    binder.<IOutputConfigurationProvider>bind(IOutputConfigurationProvider.class).to(MetaDataRuntimeModule.MelkOutputConfigurationProvider.class);
  }
}
