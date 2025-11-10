/**
 * Copyright (c) 2018 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.core.debug.grandom.validation;

import com.google.common.base.Objects;
import java.util.ArrayList;
import org.eclipse.elk.core.debug.grandom.gRandom.Configuration;
import org.eclipse.elk.core.debug.grandom.gRandom.Form;
import org.eclipse.elk.core.debug.grandom.gRandom.GRandomPackage;
import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.xtext.validation.Check;
import org.eclipse.xtext.xbase.lib.CollectionLiterals;
import org.eclipse.xtext.xbase.lib.Functions.Function1;
import org.eclipse.xtext.xbase.lib.Pair;

/**
 * This class contains custom validation rules.
 * 
 * See https://www.eclipse.org/Xtext/documentation/303_runtime_concepts.html#validation
 */
@SuppressWarnings("all")
public class GRandomValidator extends AbstractGRandomValidator {
  private static final ArrayList<Pair<Function1<Configuration, Boolean>, Pair<String, EAttribute>>> ERRORS = CollectionLiterals.<Pair<Function1<Configuration, Boolean>, Pair<String, EAttribute>>>newArrayList(
    Pair.<Function1<Configuration, Boolean>, Pair<String, EAttribute>>of(new Function1<Configuration, Boolean>() {
      @Override
      public Boolean apply(final Configuration c) {
        return Boolean.valueOf(((!Objects.equal(c.getForm(), Form.TREES)) && c.isMD()));
      }
    }, 
      Pair.<String, EAttribute>of("maxDegree only defined on trees.", GRandomPackage.Literals.CONFIGURATION__MD)), 
    Pair.<Function1<Configuration, Boolean>, Pair<String, EAttribute>>of(new Function1<Configuration, Boolean>() {
      @Override
      public Boolean apply(final Configuration c) {
        return Boolean.valueOf(((!Objects.equal(c.getForm(), Form.TREES)) && c.isMW()));
      }
    }, 
      Pair.<String, EAttribute>of("maxWidth only defined on trees.", GRandomPackage.Literals.CONFIGURATION__MW)), 
    Pair.<Function1<Configuration, Boolean>, Pair<String, EAttribute>>of(new Function1<Configuration, Boolean>() {
      @Override
      public Boolean apply(final Configuration c) {
        return Boolean.valueOf(((!Objects.equal(c.getForm(), Form.BIPARTITE)) && c.isPF()));
      }
    }, 
      Pair.<String, EAttribute>of("Partition Fraction only defined on bipartite graphs.", 
        GRandomPackage.Literals.CONFIGURATION__PF)));

  @Check
  public void optionsRestrictions(final Configuration conf) {
    for (final Pair<Function1<Configuration, Boolean>, Pair<String, EAttribute>> p : GRandomValidator.ERRORS) {
      Boolean _apply = p.getKey().apply(conf);
      if ((_apply).booleanValue()) {
        this.error(p.getValue().getKey(), p.getValue().getValue());
      }
    }
  }
}
