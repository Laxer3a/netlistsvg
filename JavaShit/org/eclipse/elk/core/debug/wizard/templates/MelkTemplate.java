/**
 * Copyright (c) 2019 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.core.debug.wizard.templates;

import org.eclipse.xtend2.lib.StringConcatenation;

@SuppressWarnings("all")
public class MelkTemplate {
  public static String buildFileContent(final String projectPackage, final String layoutProviderPath, final String layoutProvider, final String idPrefix, final String algorithmName, final String label) {
    StringConcatenation _builder = new StringConcatenation();
    _builder.append("package ");
    _builder.append(projectPackage);
    _builder.newLineIfNotEmpty();
    _builder.newLine();
    _builder.append("import ");
    _builder.append(layoutProviderPath);
    _builder.append(".");
    _builder.append(layoutProvider);
    _builder.newLineIfNotEmpty();
    _builder.append("import org.eclipse.elk.core.math.ElkPadding");
    _builder.newLine();
    _builder.newLine();
    _builder.append("bundle {");
    _builder.newLine();
    _builder.append("    ");
    _builder.append("metadataClass ");
    _builder.append(algorithmName, "    ");
    _builder.append("MetadataProvider");
    _builder.newLineIfNotEmpty();
    _builder.append("    ");
    _builder.append("idPrefix ");
    _builder.append(idPrefix, "    ");
    _builder.newLineIfNotEmpty();
    _builder.append("}");
    _builder.newLine();
    _builder.newLine();
    _builder.append("option reverseInput : boolean {");
    _builder.newLine();
    _builder.append("    ");
    _builder.append("label \"Reverse Input\"");
    _builder.newLine();
    _builder.append("    ");
    _builder.append("description");
    _builder.newLine();
    _builder.append("        ");
    _builder.append("\"True if nodes should be placed in reverse order of their");
    _builder.newLine();
    _builder.append("        ");
    _builder.append("appearance in the graph.\"");
    _builder.newLine();
    _builder.append("    ");
    _builder.append("default = false");
    _builder.newLine();
    _builder.append("    ");
    _builder.append("targets parents");
    _builder.newLine();
    _builder.append("}");
    _builder.newLine();
    _builder.newLine();
    _builder.append("algorithm ");
    _builder.append(algorithmName);
    _builder.append("(");
    _builder.append(layoutProvider);
    _builder.append(") {");
    _builder.newLineIfNotEmpty();
    _builder.append("    ");
    _builder.append("label \"");
    _builder.append(label, "    ");
    _builder.append("\"");
    _builder.newLineIfNotEmpty();
    _builder.append("    ");
    _builder.append("description \"Please insert a short but informative description here\"");
    _builder.newLine();
    _builder.append("    ");
    _builder.append("metadataClass options.");
    _builder.append(algorithmName, "    ");
    _builder.append("Options");
    _builder.newLineIfNotEmpty();
    _builder.append("    ");
    _builder.append("supports reverseInput");
    _builder.newLine();
    _builder.append("    ");
    _builder.append("supports org.eclipse.elk.padding = new ElkPadding(10)");
    _builder.newLine();
    _builder.append("    ");
    _builder.append("supports org.eclipse.elk.spacing.edgeEdge = 5");
    _builder.newLine();
    _builder.append("    ");
    _builder.append("supports org.eclipse.elk.spacing.edgeNode = 10");
    _builder.newLine();
    _builder.append("    ");
    _builder.append("supports org.eclipse.elk.spacing.nodeNode = 10");
    _builder.newLine();
    _builder.append("}");
    return _builder.toString();
  }
}
