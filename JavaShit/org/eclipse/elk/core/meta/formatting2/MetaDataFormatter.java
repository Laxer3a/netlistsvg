/**
 * Copyright (c) 2018 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.core.meta.formatting2;

import com.google.inject.Inject;
import java.util.Arrays;
import org.eclipse.elk.core.meta.metaData.MdAlgorithm;
import org.eclipse.elk.core.meta.metaData.MdBundle;
import org.eclipse.elk.core.meta.metaData.MdBundleMember;
import org.eclipse.elk.core.meta.metaData.MdCategory;
import org.eclipse.elk.core.meta.metaData.MdGroup;
import org.eclipse.elk.core.meta.metaData.MdGroupOrOption;
import org.eclipse.elk.core.meta.metaData.MdOption;
import org.eclipse.elk.core.meta.metaData.MdOptionDependency;
import org.eclipse.elk.core.meta.metaData.MdOptionSupport;
import org.eclipse.elk.core.meta.services.MetaDataGrammarAccess;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.xtext.formatting2.AbstractFormatter2;
import org.eclipse.xtext.formatting2.IFormattableDocument;
import org.eclipse.xtext.formatting2.IHiddenRegionFormatter;
import org.eclipse.xtext.formatting2.regionaccess.ISemanticRegion;
import org.eclipse.xtext.resource.XtextResource;
import org.eclipse.xtext.xbase.lib.Extension;
import org.eclipse.xtext.xbase.lib.Procedures.Procedure1;

@SuppressWarnings("all")
public class MetaDataFormatter extends AbstractFormatter2 {
  private static final Procedure1<? super IHiddenRegionFormatter> no_space = new Procedure1<IHiddenRegionFormatter>() {
    @Override
    public void apply(final IHiddenRegionFormatter it) {
      it.noSpace();
    }
  };

  private static final Procedure1<? super IHiddenRegionFormatter> one_space = new Procedure1<IHiddenRegionFormatter>() {
    @Override
    public void apply(final IHiddenRegionFormatter it) {
      it.oneSpace();
    }
  };

  private static final Procedure1<? super IHiddenRegionFormatter> new_line = new Procedure1<IHiddenRegionFormatter>() {
    @Override
    public void apply(final IHiddenRegionFormatter it) {
      it.newLine();
    }
  };

  private static final Procedure1<? super IHiddenRegionFormatter> indention = new Procedure1<IHiddenRegionFormatter>() {
    @Override
    public void apply(final IHiddenRegionFormatter it) {
      it.indent();
    }
  };

  private static final Procedure1<? super IHiddenRegionFormatter> new_line_twice = new Procedure1<IHiddenRegionFormatter>() {
    @Override
    public void apply(final IHiddenRegionFormatter it) {
      it.setNewLines(2, 2, 2);
    }
  };

  @Inject
  @Extension
  private MetaDataGrammarAccess _metaDataGrammarAccess;

  protected void _format(final MdBundleMember bundleMember, @Extension final IFormattableDocument document) {
    if ((bundleMember instanceof MdAlgorithm)) {
      document.<MdAlgorithm>format(((MdAlgorithm)bundleMember));
    } else {
      if ((bundleMember instanceof MdCategory)) {
        document.<MdCategory>format(((MdCategory)bundleMember));
      } else {
        if ((bundleMember instanceof MdGroupOrOption)) {
          document.<MdGroupOrOption>format(((MdGroupOrOption)bundleMember));
        }
      }
    }
  }

  protected void _format(final MdGroupOrOption grOpt, @Extension final IFormattableDocument document) {
    if ((grOpt instanceof MdGroup)) {
      document.<MdGroup>format(((MdGroup)grOpt));
    } else {
      if ((grOpt instanceof MdOption)) {
        document.<MdOption>format(((MdOption)grOpt));
      }
    }
  }

  protected void _format(final MdBundle bundle, @Extension final IFormattableDocument document) {
    document.append(this.textRegionExtensions.regionFor(bundle).keyword(this._metaDataGrammarAccess.getMdBundleAccess().getBundleKeyword_1_0()), MetaDataFormatter.one_space);
    document.<ISemanticRegion, ISemanticRegion>interior(
      document.prepend(this.textRegionExtensions.regionFor(bundle).keyword(this._metaDataGrammarAccess.getMdBundleAccess().getLeftCurlyBracketKeyword_1_1()), MetaDataFormatter.one_space), 
      this.textRegionExtensions.regionFor(bundle).keyword(this._metaDataGrammarAccess.getMdBundleAccess().getRightCurlyBracketKeyword_1_3()), 
      MetaDataFormatter.indention);
    document.append(document.prepend(this.textRegionExtensions.regionFor(bundle).keyword(this._metaDataGrammarAccess.getMdBundleAccess().getLabelKeyword_1_2_0_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(bundle).keyword(this._metaDataGrammarAccess.getMdBundleAccess().getMetadataClassKeyword_1_2_1_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(bundle).keyword(this._metaDataGrammarAccess.getMdBundleAccess().getDocumentationFolderKeyword_1_2_2_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(bundle).keyword(this._metaDataGrammarAccess.getMdBundleAccess().getIdPrefixKeyword_1_2_3_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    EList<MdBundleMember> _members = bundle.getMembers();
    for (final MdBundleMember member : _members) {
      document.<MdBundleMember>format(document.<MdBundleMember>prepend(member, MetaDataFormatter.new_line_twice));
    }
  }

  protected void _format(final MdGroup group, @Extension final IFormattableDocument document) {
    document.append(this.textRegionExtensions.regionFor(group).keyword(this._metaDataGrammarAccess.getMdGroupAccess().getGroupKeyword_0()), MetaDataFormatter.one_space);
    document.<ISemanticRegion, ISemanticRegion>interior(
      document.prepend(this.textRegionExtensions.regionFor(group).keyword(this._metaDataGrammarAccess.getMdGroupAccess().getLeftCurlyBracketKeyword_2()), MetaDataFormatter.one_space), 
      document.prepend(this.textRegionExtensions.regionFor(group).keyword(this._metaDataGrammarAccess.getMdGroupAccess().getRightCurlyBracketKeyword_5()), MetaDataFormatter.new_line_twice), 
      MetaDataFormatter.indention);
    document.append(this.textRegionExtensions.regionFor(group).keyword(this._metaDataGrammarAccess.getMdGroupAccess().getDocumentationKeyword_3_0()), MetaDataFormatter.one_space);
    EList<MdGroupOrOption> _children = group.getChildren();
    for (final MdGroupOrOption child : _children) {
      document.<MdGroupOrOption>format(document.<MdGroupOrOption>prepend(child, MetaDataFormatter.new_line_twice));
    }
  }

  protected void _format(final MdOption option, @Extension final IFormattableDocument document) {
    document.append(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getDeprecatedDeprecatedKeyword_0_0()), MetaDataFormatter.one_space);
    document.append(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getAdvancedAdvancedKeyword_1_0_0()), MetaDataFormatter.one_space);
    document.append(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getProgrammaticProgrammaticKeyword_1_1_0()), MetaDataFormatter.one_space);
    document.append(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getOutputOutputKeyword_1_2_0()), MetaDataFormatter.one_space);
    document.append(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getGlobalGlobalKeyword_1_3_0()), MetaDataFormatter.one_space);
    document.append(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getOptionKeyword_2()), MetaDataFormatter.one_space);
    document.<ISemanticRegion, ISemanticRegion>interior(
      document.append(document.prepend(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getLeftCurlyBracketKeyword_5()), MetaDataFormatter.one_space), MetaDataFormatter.new_line), 
      this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getRightCurlyBracketKeyword_8()), 
      MetaDataFormatter.indention);
    document.append(document.prepend(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getLabelKeyword_6_0_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getDescriptionKeyword_6_1_0()), MetaDataFormatter.new_line), MetaDataFormatter.new_line);
    document.<ISemanticRegion, ISemanticRegion>interior(
      this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getDescriptionKeyword_6_1_0()), 
      this.textRegionExtensions.regionFor(option).assignment(this._metaDataGrammarAccess.getMdOptionAccess().getDescriptionAssignment_6_1_1()).getNextSemanticRegion(), 
      MetaDataFormatter.indention);
    document.append(document.prepend(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getDocumentationKeyword_6_2_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getDefaultKeyword_6_3_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getEqualsSignKeyword_6_3_1()), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getLowerBoundKeyword_6_4_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getEqualsSignKeyword_6_4_1()), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getUpperBoundKeyword_6_5_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getEqualsSignKeyword_6_5_1()), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getTargetsKeyword_6_6_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getCommaKeyword_6_6_2_0()), MetaDataFormatter.no_space), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getLegacyIdsKeyword_6_7_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(option).keyword(this._metaDataGrammarAccess.getMdOptionAccess().getCommaKeyword_6_7_2_0()), MetaDataFormatter.no_space), MetaDataFormatter.one_space);
    EList<MdOptionDependency> _dependencies = option.getDependencies();
    for (final MdOptionDependency dependencies : _dependencies) {
      document.<MdOptionDependency>append(dependencies, MetaDataFormatter.new_line);
    }
  }

  protected void _format(final MdOptionDependency optionDependency, @Extension final IFormattableDocument document) {
    document.append(this.textRegionExtensions.regionFor(optionDependency).keyword(this._metaDataGrammarAccess.getMdOptionDependencyAccess().getRequiresKeyword_0()), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(optionDependency).keyword(this._metaDataGrammarAccess.getMdOptionDependencyAccess().getEqualsSignEqualsSignKeyword_2_0()), MetaDataFormatter.one_space), MetaDataFormatter.one_space);
  }

  protected void _format(final MdAlgorithm algorithm, @Extension final IFormattableDocument document) {
    document.append(this.textRegionExtensions.regionFor(algorithm).keyword(this._metaDataGrammarAccess.getMdAlgorithmAccess().getDeprecatedDeprecatedKeyword_0_0()), MetaDataFormatter.one_space);
    document.append(this.textRegionExtensions.regionFor(algorithm).keyword(this._metaDataGrammarAccess.getMdAlgorithmAccess().getAlgorithmKeyword_1()), MetaDataFormatter.one_space);
    document.prepend(this.textRegionExtensions.regionFor(algorithm).keyword(this._metaDataGrammarAccess.getMdAlgorithmAccess().getLeftParenthesisKeyword_3()), MetaDataFormatter.no_space);
    document.<ISemanticRegion, ISemanticRegion>interior(
      document.append(document.prepend(this.textRegionExtensions.regionFor(algorithm).keyword(this._metaDataGrammarAccess.getMdAlgorithmAccess().getLeftCurlyBracketKeyword_7()), MetaDataFormatter.one_space), 
        MetaDataFormatter.new_line), 
      this.textRegionExtensions.regionFor(algorithm).keyword(this._metaDataGrammarAccess.getMdAlgorithmAccess().getRightCurlyBracketKeyword_10()), 
      MetaDataFormatter.indention);
    document.append(document.prepend(this.textRegionExtensions.regionFor(algorithm).keyword(this._metaDataGrammarAccess.getMdAlgorithmAccess().getLabelKeyword_8_0_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(algorithm).keyword(this._metaDataGrammarAccess.getMdAlgorithmAccess().getMetadataClassKeyword_8_1_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(algorithm).keyword(this._metaDataGrammarAccess.getMdAlgorithmAccess().getDescriptionKeyword_8_2_0()), MetaDataFormatter.new_line), MetaDataFormatter.new_line);
    document.<ISemanticRegion, ISemanticRegion>interior(
      this.textRegionExtensions.regionFor(algorithm).keyword(this._metaDataGrammarAccess.getMdAlgorithmAccess().getDescriptionKeyword_8_2_0()), 
      this.textRegionExtensions.regionFor(algorithm).assignment(this._metaDataGrammarAccess.getMdAlgorithmAccess().getDescriptionAssignment_8_2_1()).getNextSemanticRegion(), 
      MetaDataFormatter.indention);
    document.append(document.prepend(this.textRegionExtensions.regionFor(algorithm).keyword(this._metaDataGrammarAccess.getMdAlgorithmAccess().getDocumentationKeyword_8_3_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(algorithm).keyword(this._metaDataGrammarAccess.getMdAlgorithmAccess().getCategoryKeyword_8_4_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(algorithm).keyword(this._metaDataGrammarAccess.getMdAlgorithmAccess().getPreviewKeyword_8_5_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(algorithm).keyword(this._metaDataGrammarAccess.getMdAlgorithmAccess().getFeaturesKeyword_8_6_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(algorithm).keyword(this._metaDataGrammarAccess.getMdAlgorithmAccess().getValidatorKeyword_8_7_0()), MetaDataFormatter.one_space), MetaDataFormatter.one_space);
    EList<MdOptionSupport> _supportedOptions = algorithm.getSupportedOptions();
    for (final MdOptionSupport option : _supportedOptions) {
      document.<MdOptionSupport>format(document.<MdOptionSupport>append(option, MetaDataFormatter.new_line));
    }
  }

  protected void _format(final MdCategory category, @Extension final IFormattableDocument document) {
    document.append(this.textRegionExtensions.regionFor(category).keyword(this._metaDataGrammarAccess.getMdCategoryAccess().getDeprecatedDeprecatedKeyword_0_0()), MetaDataFormatter.one_space);
    document.append(this.textRegionExtensions.regionFor(category).keyword(this._metaDataGrammarAccess.getMdCategoryAccess().getCategoryKeyword_1()), MetaDataFormatter.one_space);
    document.<ISemanticRegion, ISemanticRegion>interior(
      document.append(document.prepend(this.textRegionExtensions.regionFor(category).keyword(this._metaDataGrammarAccess.getMdCategoryAccess().getLeftCurlyBracketKeyword_3()), MetaDataFormatter.one_space), MetaDataFormatter.new_line), 
      this.textRegionExtensions.regionFor(category).keyword(this._metaDataGrammarAccess.getMdCategoryAccess().getRightCurlyBracketKeyword_5()), 
      MetaDataFormatter.indention);
    document.append(document.prepend(this.textRegionExtensions.regionFor(category).keyword(this._metaDataGrammarAccess.getMdCategoryAccess().getLabelKeyword_4_0_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(category).keyword(this._metaDataGrammarAccess.getMdCategoryAccess().getDescriptionKeyword_4_1_0()), MetaDataFormatter.new_line), MetaDataFormatter.new_line);
    document.<ISemanticRegion, ISemanticRegion>interior(
      this.textRegionExtensions.regionFor(category).keyword(this._metaDataGrammarAccess.getMdCategoryAccess().getDescriptionKeyword_4_1_0()), 
      this.textRegionExtensions.regionFor(category).assignment(this._metaDataGrammarAccess.getMdCategoryAccess().getDescriptionAssignment_4_1_1()).getNextSemanticRegion(), 
      MetaDataFormatter.indention);
    document.append(document.prepend(this.textRegionExtensions.regionFor(category).keyword(this._metaDataGrammarAccess.getMdCategoryAccess().getDocumentationKeyword_4_2_0()), MetaDataFormatter.new_line), MetaDataFormatter.one_space);
  }

  protected void _format(final MdOptionSupport optionSupport, @Extension final IFormattableDocument document) {
    document.append(this.textRegionExtensions.regionFor(optionSupport).keyword(this._metaDataGrammarAccess.getMdOptionSupportAccess().getSupportsKeyword_0()), MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(optionSupport).keyword(this._metaDataGrammarAccess.getMdOptionSupportAccess().getEqualsSignKeyword_2_0()), MetaDataFormatter.one_space), 
      MetaDataFormatter.one_space);
    document.append(document.prepend(this.textRegionExtensions.regionFor(optionSupport).keyword(this._metaDataGrammarAccess.getMdOptionSupportAccess().getDocumentationKeyword_3_0()), MetaDataFormatter.new_line), 
      MetaDataFormatter.one_space);
  }

  public void format(final Object group, final IFormattableDocument document) {
    if (group instanceof MdGroup) {
      _format((MdGroup)group, document);
      return;
    } else if (group instanceof MdOption) {
      _format((MdOption)group, document);
      return;
    } else if (group instanceof XtextResource) {
      _format((XtextResource)group, document);
      return;
    } else if (group instanceof MdAlgorithm) {
      _format((MdAlgorithm)group, document);
      return;
    } else if (group instanceof MdCategory) {
      _format((MdCategory)group, document);
      return;
    } else if (group instanceof MdGroupOrOption) {
      _format((MdGroupOrOption)group, document);
      return;
    } else if (group instanceof MdBundle) {
      _format((MdBundle)group, document);
      return;
    } else if (group instanceof MdBundleMember) {
      _format((MdBundleMember)group, document);
      return;
    } else if (group instanceof MdOptionDependency) {
      _format((MdOptionDependency)group, document);
      return;
    } else if (group instanceof MdOptionSupport) {
      _format((MdOptionSupport)group, document);
      return;
    } else if (group instanceof EObject) {
      _format((EObject)group, document);
      return;
    } else if (group == null) {
      _format((Void)null, document);
      return;
    } else if (group != null) {
      _format(group, document);
      return;
    } else {
      throw new IllegalArgumentException("Unhandled parameter types: " +
        Arrays.<Object>asList(group, document).toString());
    }
  }
}
