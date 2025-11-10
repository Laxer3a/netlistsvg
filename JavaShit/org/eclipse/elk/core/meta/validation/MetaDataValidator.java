/**
 * Copyright (c) 2016 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.core.meta.validation;

import java.util.Map;
import org.eclipse.elk.core.meta.metaData.MdAlgorithm;
import org.eclipse.elk.core.meta.metaData.MdBundle;
import org.eclipse.elk.core.meta.metaData.MdBundleMember;
import org.eclipse.elk.core.meta.metaData.MdCategory;
import org.eclipse.elk.core.meta.metaData.MdGroup;
import org.eclipse.elk.core.meta.metaData.MdOption;
import org.eclipse.elk.core.meta.metaData.MetaDataPackage;
import org.eclipse.xtext.validation.Check;
import org.eclipse.xtext.xbase.lib.CollectionLiterals;

/**
 * This class contains custom validation rules.
 * 
 * See https://www.eclipse.org/Xtext/documentation/303_runtime_concepts.html#validation
 */
@SuppressWarnings("all")
public class MetaDataValidator extends AbstractMetaDataValidator {
  @Check
  public void checkDuplicateMemberId(final MdBundle bundle) {
    this.checkDuplicateIds(bundle.getMembers());
  }

  private void checkDuplicateIds(final Iterable<? extends MdBundleMember> elements) {
    final Map<String, MdAlgorithm> algorithmIds = CollectionLiterals.<String, MdAlgorithm>newHashMap();
    final Map<String, MdCategory> categoryIds = CollectionLiterals.<String, MdCategory>newHashMap();
    final Map<String, MdOption> propertyIds = CollectionLiterals.<String, MdOption>newHashMap();
    final Map<String, MdGroup> groupIds = CollectionLiterals.<String, MdGroup>newHashMap();
    for (final MdBundleMember element : elements) {
      boolean _matched = false;
      if (element instanceof MdAlgorithm) {
        _matched=true;
        this.<MdAlgorithm>checkExistsAndRemember(algorithmIds, ((MdAlgorithm)element));
      }
      if (!_matched) {
        if (element instanceof MdCategory) {
          _matched=true;
          this.<MdCategory>checkExistsAndRemember(categoryIds, ((MdCategory)element));
        }
      }
      if (!_matched) {
        if (element instanceof MdGroup) {
          _matched=true;
          this.<MdGroup>checkExistsAndRemember(groupIds, ((MdGroup)element));
          this.checkDuplicateIds(((MdGroup)element).getChildren());
        }
      }
      if (!_matched) {
        if (element instanceof MdOption) {
          _matched=true;
          this.<MdOption>checkExistsAndRemember(propertyIds, ((MdOption)element));
        }
      }
    }
  }

  private <T extends MdBundleMember> void checkExistsAndRemember(final Map<String, T> map, final T element) {
    boolean _containsKey = map.containsKey(element.getName());
    if (_containsKey) {
      final T otherMember = map.get(element.getName());
      if ((otherMember != null)) {
        this.duplicateId(otherMember);
        map.put(element.getName(), null);
      }
      this.duplicateId(element);
    } else {
      map.put(element.getName(), element);
    }
  }

  private void duplicateId(final MdBundleMember member) {
    String _name = member.getName();
    String _plus = ("The id \'" + _name);
    String _plus_1 = (_plus + "\' is already used.");
    this.error(_plus_1, member, MetaDataPackage.Literals.MD_BUNDLE_MEMBER__NAME);
  }
}
