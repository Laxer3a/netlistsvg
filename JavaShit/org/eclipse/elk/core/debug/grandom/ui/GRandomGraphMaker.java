/**
 * Copyright (c) 2016, 2020 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.core.debug.grandom.ui;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.elk.core.debug.grandom.gRandom.Configuration;
import org.eclipse.elk.core.debug.grandom.gRandom.Formats;
import org.eclipse.elk.core.debug.grandom.gRandom.RandGraph;
import org.eclipse.elk.core.debug.grandom.generators.RandomGraphGenerator;
import org.eclipse.elk.graph.ElkNode;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.impl.ResourceSetImpl;
import org.eclipse.xtext.xbase.lib.Exceptions;

@SuppressWarnings("all")
public class GRandomGraphMaker {
  private final Iterable<Configuration> configs;

  private final String DEFAULT_NAME = "random";

  private final String DEFAULT_FORMAT = Formats.ELKT.toString();

  public GRandomGraphMaker(final RandGraph rdg) {
    this.configs = rdg.getConfigs();
  }

  public void gen(final IProject project) {
    try {
      final RandomGraphGenerator generator = new RandomGraphGenerator(null);
      for (final Configuration config : this.configs) {
        {
          String _xifexpression = null;
          boolean _exists = this.exists(config.getFilename());
          if (_exists) {
            _xifexpression = config.getFilename();
          } else {
            _xifexpression = this.DEFAULT_NAME;
          }
          final String filename = _xifexpression;
          Object _xifexpression_1 = null;
          boolean _exists_1 = this.exists(config.getFormat());
          if (_exists_1) {
            _xifexpression_1 = config.getFormat();
          } else {
            _xifexpression_1 = this.DEFAULT_FORMAT;
          }
          final Object format = ((Object)_xifexpression_1);
          final List<ElkNode> graphs = generator.generate(config);
          int fileNum = 0;
          for (int i = 0; (i < graphs.size()); i++) {
            {
              while (project.getFile((((filename + Integer.valueOf(fileNum)) + ".") + format)).exists()) {
                fileNum++;
              }
              final IFile f = project.getFile((((filename + Integer.valueOf(fileNum)) + ".") + format));
              this.serialize(graphs.get(i), f);
            }
          }
        }
      }
    } catch (Throwable _e) {
      throw Exceptions.sneakyThrow(_e);
    }
  }

  public List<ElkNode> loadGraph() {
    final ArrayList<ElkNode> graphs = new ArrayList<ElkNode>();
    final RandomGraphGenerator generator = new RandomGraphGenerator(null);
    for (final Configuration config : this.configs) {
      graphs.addAll(generator.generate(config));
    }
    return graphs;
  }

  private void serialize(final ElkNode graph, final IFile file) throws IOException, CoreException {
    final ResourceSetImpl resourceSet = new ResourceSetImpl();
    final Resource resource = resourceSet.createResource(URI.createURI(file.getLocationURI().toString()));
    resource.getContents().add(graph);
    resource.save(Collections.EMPTY_MAP);
    file.refreshLocal(1, null);
  }

  private boolean exists(final Object o) {
    return (o != null);
  }
}
