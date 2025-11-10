/**
 * Copyright (c) 2016 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.core.meta.jvmmodel;

import com.google.common.base.CharMatcher;
import com.google.common.base.Objects;
import com.google.common.collect.Iterables;
import java.io.InputStream;
import java.io.PrintWriter;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.util.Arrays;
import java.util.Collections;
import java.util.EnumSet;
import java.util.LinkedList;
import java.util.List;
import org.eclipse.elk.core.meta.MetaDataRuntimeModule;
import org.eclipse.elk.core.meta.metaData.MdAlgorithm;
import org.eclipse.elk.core.meta.metaData.MdBundle;
import org.eclipse.elk.core.meta.metaData.MdBundleMember;
import org.eclipse.elk.core.meta.metaData.MdCategory;
import org.eclipse.elk.core.meta.metaData.MdGraphFeature;
import org.eclipse.elk.core.meta.metaData.MdGroup;
import org.eclipse.elk.core.meta.metaData.MdModel;
import org.eclipse.elk.core.meta.metaData.MdOption;
import org.eclipse.elk.core.meta.metaData.MdOptionDependency;
import org.eclipse.elk.core.meta.metaData.MdOptionSupport;
import org.eclipse.elk.core.meta.metaData.MdOptionTargetType;
import org.eclipse.elk.graph.properties.GraphFeature;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.xtend2.lib.StringConcatenation;
import org.eclipse.xtext.common.types.JvmAnnotationReference;
import org.eclipse.xtext.common.types.JvmAnnotationType;
import org.eclipse.xtext.common.types.JvmDeclaredType;
import org.eclipse.xtext.common.types.JvmEnumerationType;
import org.eclipse.xtext.common.types.JvmField;
import org.eclipse.xtext.common.types.JvmParameterizedTypeReference;
import org.eclipse.xtext.common.types.JvmType;
import org.eclipse.xtext.common.types.JvmTypeReference;
import org.eclipse.xtext.generator.AbstractFileSystemAccess2;
import org.eclipse.xtext.generator.IFileSystemAccess;
import org.eclipse.xtext.nodemodel.util.NodeModelUtils;
import org.eclipse.xtext.xbase.XExpression;
import org.eclipse.xtext.xbase.compiler.JvmModelGenerator;
import org.eclipse.xtext.xbase.lib.CollectionLiterals;
import org.eclipse.xtext.xbase.lib.Conversions;
import org.eclipse.xtext.xbase.lib.Exceptions;
import org.eclipse.xtext.xbase.lib.Functions.Function1;
import org.eclipse.xtext.xbase.lib.IterableExtensions;
import org.eclipse.xtext.xbase.lib.ListExtensions;
import org.eclipse.xtext.xbase.lib.Pair;
import org.eclipse.xtext.xbase.lib.StringExtensions;

/**
 * Generates documentation from the source model that pertains to a *.melk file.
 * <p>
 * For each algorithm, option and group in the model, a Markdown file is created in the directory specified by
 * the vm argument {@code elk.metadata.documentation.outputPath}. This path usually points to the docs folder in the
 * git repository for elk, when the Oomph setup appends the vm argument to the eclipse.ini.
 * The docs folder in the master branch will be hosted on a Github page.
 * </p>
 * 
 * @author dag
 */
@SuppressWarnings("all")
public class MelkDocumentationGenerator extends JvmModelGenerator {
  /**
   * The {@code IFileSystemAccess} used to read additional documentation files stored within the Eclipse project that
   * contains the model.
   */
  private IFileSystemAccess fsa;

  /**
   * The place where the generated algorithm documentation is stored.
   */
  private Path algorithmsOutputPath;

  /**
   * The place where the generated layout option documentation is stored.
   */
  private Path optionsOutputPath;

  /**
   * The place where the generated layout option group documentation is stored.
   */
  private Path optionGroupsOutputPath;

  /**
   * The place where images are stored.
   */
  private Path imageOutputPath;

  /**
   * The directory containing additional documentation files.
   */
  private Path projectDocumentationSourceFolder;

  /**
   * The method {@code internalDoGenerate} is called for each {@link MdModel} derived from a *.melk file.
   * 
   * @param model
   *      the {@link MdModel} that documentation is generated for.
   * @param fsa
   *      the {@code IFileSystemAccess} used to read files
   */
  protected void _internalDoGenerate(final MdModel model, final IFileSystemAccess fsa) {
    this.fsa = fsa;
    final String propertyOutputPath = System.getProperty("elk.metadata.documentation.outputPath");
    if ((propertyOutputPath == null)) {
      return;
    }
    this.setupOutputPaths(propertyOutputPath);
    if (((model.getName() == null) || (model.getBundle() == null))) {
      return;
    }
    final MdBundle bundle = model.getBundle();
    final EList<MdBundleMember> members = bundle.getMembers();
    String _documentationFolder = bundle.getDocumentationFolder();
    boolean _tripleNotEquals = (_documentationFolder != null);
    if (_tripleNotEquals) {
      this.projectDocumentationSourceFolder = Paths.get(bundle.getDocumentationFolder());
    } else {
      this.projectDocumentationSourceFolder = Paths.get("docs");
    }
    Iterable<MdAlgorithm> _filter = Iterables.<MdAlgorithm>filter(members, MdAlgorithm.class);
    for (final MdAlgorithm algorithm : _filter) {
      this.writeDoc(algorithm, this.algorithmsOutputPath);
    }
    Iterable<MdOption> _allOptionDefinitions = this.getAllOptionDefinitions(members);
    for (final MdOption option : _allOptionDefinitions) {
      this.writeDoc(option, this.optionsOutputPath);
    }
    Iterable<MdGroup> _allGroupDefinitions = this.getAllGroupDefinitions(members);
    for (final MdGroup group : _allGroupDefinitions) {
      this.writeDoc(group, this.optionGroupsOutputPath);
    }
  }

  /**
   * Sets up the documentation output paths. Normally, we would try and purge the directories first to be sure that
   * there is no old content. However, in a typical automatic build this generator will be invoked multiple times. If
   * each invocation purged the output directories, there wouldn't be much documentation left.
   * 
   * @param docsOutputFolder the folder the documentation should be generated into.
   */
  private Path setupOutputPaths(final String docsOutputPathString) {
    try {
      Path _xblockexpression = null;
      {
        final Path docsOutputPath = Paths.get(docsOutputPathString);
        final Path referencePath = docsOutputPath.resolve("content").resolve("reference");
        this.algorithmsOutputPath = referencePath.resolve("algorithms");
        Files.createDirectories(this.algorithmsOutputPath);
        this.optionsOutputPath = referencePath.resolve("options");
        Files.createDirectories(this.optionsOutputPath);
        this.optionGroupsOutputPath = referencePath.resolve("groups");
        Files.createDirectories(this.optionGroupsOutputPath);
        this.imageOutputPath = docsOutputPath.resolve("static").resolve("img_gen");
        _xblockexpression = Files.createDirectories(this.imageOutputPath);
      }
      return _xblockexpression;
    } catch (Throwable _e) {
      throw Exceptions.sneakyThrow(_e);
    }
  }

  /**
   * This is a wrapper for {@link #writeDoc(String, String) writeDoc(String, String)}
   * that uses the full name of a member as filename.
   * 
   * @param member
   *      the {@link MdBundleMember} that documentation is generated for
   * @param outputPath
   *      the folder the bundle member's documentation is to be generated into
   */
  private void writeDoc(final MdBundleMember member, final Path outputPath) {
    final String fileName = this.toHugoIdentifier(this.getQualifiedName(member));
    this.writeDoc(fileName, outputPath, this.generateDoc(member));
  }

  /**
   * Creates a file in {@code outputPath} to store the generated Markdown in.
   * 
   * @param fileName
   *      name of the file
   * @param outputPath
   *      the folder the bundle member's documentation is to be generated into
   * @param documentation
   *      generated markdown string
   */
  private void writeDoc(final String fileName, final Path outputPath, final String documentation) {
    String file = fileName;
    boolean _endsWith = file.endsWith(".md");
    boolean _not = (!_endsWith);
    if (_not) {
      String _file = file;
      file = (_file + ".md");
    }
    PrintWriter out = null;
    try {
      String _string = outputPath.resolve(file).toString();
      PrintWriter _printWriter = new PrintWriter(_string);
      out = _printWriter;
      out.print(documentation);
    } catch (final Throwable _t) {
      if (_t instanceof Exception) {
        final Exception exception = (Exception)_t;
        exception.printStackTrace();
      } else {
        throw Exceptions.sneakyThrow(_t);
      }
    } finally {
      if (out!=null) {
        out.close();
      }
    }
  }

  /**
   * Copies images or any file from within the project to the place where the generated documentation is located.
   * 
   * @param path
   *      the path to the image relative to the eclipse project
   * @param newFilename
   *      file name for the copy (should be associable to the issuing member)
   */
  private void copyImageToOutputPath(final String path, final String newFileName) {
    if (((path == null) || (newFileName == null))) {
      return;
    }
    try {
      InputStream iStream = ((AbstractFileSystemAccess2) this.fsa).readBinaryFile(path, 
        MetaDataRuntimeModule.MelkOutputConfigurationProvider.AD_INPUT);
      Files.copy(iStream, this.imageOutputPath.resolve(newFileName), StandardCopyOption.REPLACE_EXISTING);
    } catch (final Throwable _t) {
      if (_t instanceof Exception) {
        final Exception exception = (Exception)_t;
        exception.printStackTrace();
      } else {
        throw Exceptions.sneakyThrow(_t);
      }
    }
  }

  /**
   * The dispatch method {@code generateDoc} is called for each {@link MdBundleMember} that is supposed to be
   * documented.
   * This generates a documentation String in Markdown format.
   * 
   * @param algorithm
   *      the {@link MdAlgorithm} from the {@link MdModel}
   * @return
   *      the Markdown String
   */
  private String _generateDoc(final MdAlgorithm algorithm) {
    StringConcatenation _builder = new StringConcatenation();
    _builder.append("---");
    _builder.newLine();
    _builder.append("title: \"");
    String _elvis = null;
    String _label = algorithm.getLabel();
    if (_label != null) {
      _elvis = _label;
    } else {
      String _name = algorithm.getName();
      _elvis = _name;
    }
    _builder.append(_elvis);
    _builder.append("\"");
    _builder.newLineIfNotEmpty();
    _builder.append("displayid: \"");
    String _qualifiedName = this.getQualifiedName(algorithm);
    _builder.append(_qualifiedName);
    _builder.append("\"");
    _builder.newLineIfNotEmpty();
    _builder.append("menu:");
    _builder.newLine();
    _builder.append("  ");
    _builder.append("main:");
    _builder.newLine();
    _builder.append("    ");
    _builder.append("identifier: \"alg-");
    String _hugoIdentifier = this.toHugoIdentifier(this.getQualifiedName(algorithm));
    _builder.append(_hugoIdentifier, "    ");
    _builder.append("\"");
    _builder.newLineIfNotEmpty();
    _builder.append("    ");
    _builder.append("parent: \"Algorithms\"");
    _builder.newLine();
    _builder.append("---");
    _builder.newLine();
    _builder.newLine();
    String doc = _builder.toString();
    String _previewImage = algorithm.getPreviewImage();
    boolean _tripleNotEquals = (_previewImage != null);
    if (_tripleNotEquals) {
      String _hugoIdentifier_1 = this.toHugoIdentifier(this.getQualifiedName(algorithm));
      String _plus = (_hugoIdentifier_1 + "_preview_");
      String _previewImage_1 = algorithm.getPreviewImage();
      int _lastIndexOf = algorithm.getPreviewImage().lastIndexOf("/");
      int _plus_1 = (_lastIndexOf + 1);
      String _substring = _previewImage_1.substring(_plus_1);
      final String newFileName = (_plus + _substring);
      this.copyImageToOutputPath(algorithm.getPreviewImage(), newFileName);
      String _doc = doc;
      doc = (_doc + (("{{< image src=\"" + newFileName) + "\" alt=\"Preview Image\" gen=\"1\" >}}\n\n"));
    }
    String _doc_1 = doc;
    StringConcatenation _builder_1 = new StringConcatenation();
    _builder_1.append("Property | Value");
    _builder_1.newLine();
    _builder_1.append("-------- | -----");
    _builder_1.newLine();
    _builder_1.append("*Identifier:* | `");
    String _qualifiedName_1 = this.getQualifiedName(algorithm);
    _builder_1.append(_qualifiedName_1);
    _builder_1.append("`");
    _builder_1.newLineIfNotEmpty();
    _builder_1.append("*Meta Data Provider:* | `");
    String _targetClass = this.getBundle(algorithm).getTargetClass();
    _builder_1.append(_targetClass);
    _builder_1.append("`");
    _builder_1.newLineIfNotEmpty();
    _builder_1.newLine();
    doc = (_doc_1 + _builder_1);
    boolean _isNullOrEmpty = StringExtensions.isNullOrEmpty(algorithm.getDescription());
    boolean _not = (!_isNullOrEmpty);
    if (_not) {
      String _doc_2 = doc;
      StringConcatenation _builder_2 = new StringConcatenation();
      _builder_2.append("## Description");
      _builder_2.newLine();
      _builder_2.newLine();
      String _trimNewlineTabsAndReduceToSingleSpace = this.trimNewlineTabsAndReduceToSingleSpace(algorithm.getDescription());
      _builder_2.append(_trimNewlineTabsAndReduceToSingleSpace);
      _builder_2.newLineIfNotEmpty();
      _builder_2.newLine();
      doc = (_doc_2 + _builder_2);
    }
    MdCategory _category = algorithm.getCategory();
    boolean _tripleNotEquals_1 = (_category != null);
    if (_tripleNotEquals_1) {
      String _doc_3 = doc;
      StringConcatenation _builder_3 = new StringConcatenation();
      _builder_3.append("## Category: ");
      String _elvis_1 = null;
      String _label_1 = algorithm.getCategory().getLabel();
      if (_label_1 != null) {
        _elvis_1 = _label_1;
      } else {
        String _name_1 = algorithm.getCategory().getName();
        _elvis_1 = _name_1;
      }
      _builder_3.append(_elvis_1);
      _builder_3.newLineIfNotEmpty();
      _builder_3.newLine();
      doc = (_doc_3 + _builder_3);
      boolean _isNullOrEmpty_1 = StringExtensions.isNullOrEmpty(algorithm.getCategory().getDescription());
      boolean _not_1 = (!_isNullOrEmpty_1);
      if (_not_1) {
        String _doc_4 = doc;
        StringConcatenation _builder_4 = new StringConcatenation();
        String _trimNewlineTabsAndReduceToSingleSpace_1 = this.trimNewlineTabsAndReduceToSingleSpace(algorithm.getCategory().getDescription());
        _builder_4.append(_trimNewlineTabsAndReduceToSingleSpace_1);
        _builder_4.newLineIfNotEmpty();
        _builder_4.newLine();
        doc = (_doc_4 + _builder_4);
      }
    }
    boolean _isEmpty = algorithm.getSupportedFeatures().isEmpty();
    boolean _not_2 = (!_isEmpty);
    if (_not_2) {
      String _doc_5 = doc;
      StringConcatenation _builder_5 = new StringConcatenation();
      _builder_5.append("## Supported Graph Features");
      _builder_5.newLine();
      _builder_5.newLine();
      _builder_5.append("Name | Description");
      _builder_5.newLine();
      _builder_5.append("----|----");
      _builder_5.newLine();
      doc = (_doc_5 + _builder_5);
      EList<MdGraphFeature> _supportedFeatures = algorithm.getSupportedFeatures();
      for (final MdGraphFeature supportedFeature : _supportedFeatures) {
        String _doc_6 = doc;
        final Function1<String, String> _function = new Function1<String, String>() {
          @Override
          public String apply(final String it) {
            return StringExtensions.toFirstUpper(it);
          }
        };
        String _join = IterableExtensions.join(ListExtensions.<String, String>map(((List<String>)Conversions.doWrapArray(supportedFeature.getLiteral().split("_"))), _function), " ");
        String _plus_2 = (_join + 
          " | ");
        String _description = GraphFeature.valueOf(supportedFeature.name()).getDescription();
        String _plus_3 = (_plus_2 + _description);
        String _plus_4 = (_plus_3 + "\n");
        doc = (_doc_6 + _plus_4);
      }
      String _doc_7 = doc;
      doc = (_doc_7 + "\n");
    }
    String _documentation = algorithm.getDocumentation();
    boolean _tripleNotEquals_2 = (_documentation != null);
    if (_tripleNotEquals_2) {
      String _doc_8 = doc;
      StringConcatenation _builder_6 = new StringConcatenation();
      _builder_6.append("## Additional Documentation");
      _builder_6.newLine();
      _builder_6.newLine();
      String _additionalDocumentation = this.additionalDocumentation(algorithm.getDocumentation(), this.toHugoIdentifier(this.getQualifiedName(algorithm)));
      _builder_6.append(_additionalDocumentation);
      _builder_6.newLineIfNotEmpty();
      _builder_6.newLine();
      doc = (_doc_8 + _builder_6);
    }
    boolean _isEmpty_1 = algorithm.getSupportedOptions().isEmpty();
    boolean _not_3 = (!_isEmpty_1);
    if (_not_3) {
      String _doc_9 = doc;
      StringConcatenation _builder_7 = new StringConcatenation();
      _builder_7.append("## Supported Options");
      _builder_7.newLine();
      _builder_7.newLine();
      _builder_7.append("Option | Default Value");
      _builder_7.newLine();
      _builder_7.append("----|----");
      _builder_7.newLine();
      doc = (_doc_9 + _builder_7);
      final Function1<MdOptionSupport, String> _function_1 = new Function1<MdOptionSupport, String>() {
        @Override
        public String apply(final MdOptionSupport it) {
          String _elvis = null;
          String _label = it.getOption().getLabel();
          if (_label != null) {
            _elvis = _label;
          } else {
            String _name = it.getOption().getName();
            _elvis = _name;
          }
          return _elvis.toLowerCase();
        }
      };
      List<MdOptionSupport> _sortBy = IterableExtensions.<MdOptionSupport, String>sortBy(algorithm.getSupportedOptions(), _function_1);
      for (final MdOptionSupport supportedOption : _sortBy) {
        {
          String optionFileName = this.toHugoIdentifier(this.getQualifiedName(supportedOption.getOption()));
          String _documentation_1 = supportedOption.getDocumentation();
          boolean _tripleNotEquals_3 = (_documentation_1 != null);
          if (_tripleNotEquals_3) {
            String _hugoIdentifier_2 = this.toHugoIdentifier(this.getQualifiedName(supportedOption.getOption()));
            String _plus_5 = (_hugoIdentifier_2 + "_");
            String _hugoIdentifier_3 = this.toHugoIdentifier(this.getQualifiedName(algorithm));
            String _plus_6 = (_plus_5 + _hugoIdentifier_3);
            optionFileName = _plus_6;
            MdOption _option = supportedOption.getOption();
            String _elvis_2 = null;
            String _label_2 = supportedOption.getOption().getLabel();
            if (_label_2 != null) {
              _elvis_2 = _label_2;
            } else {
              String _name_2 = supportedOption.getOption().getName();
              _elvis_2 = _name_2;
            }
            String _plus_7 = (_elvis_2 + " (");
            String _elvis_3 = null;
            String _label_3 = algorithm.getLabel();
            if (_label_3 != null) {
              _elvis_3 = _label_3;
            } else {
              String _name_3 = algorithm.getName();
              _elvis_3 = _name_3;
            }
            String _plus_8 = (_plus_7 + _elvis_3);
            String _plus_9 = (_plus_8 + ")");
            this.writeDoc(optionFileName, 
              this.optionsOutputPath, 
              this.generateDoc(_option, _plus_9, optionFileName, 
                this.additionalDocumentation(supportedOption.getDocumentation(), optionFileName)));
          }
          StringConcatenation _builder_8 = new StringConcatenation();
          _builder_8.append("{{< relref \"reference/options/");
          _builder_8.append(optionFileName);
          _builder_8.append(".md\" >}}");
          final String optionFileLink = _builder_8.toString();
          String _doc_10 = doc;
          StringConcatenation _builder_9 = new StringConcatenation();
          _builder_9.append("[");
          String _elvis_4 = null;
          String _label_4 = supportedOption.getOption().getLabel();
          if (_label_4 != null) {
            _elvis_4 = _label_4;
          } else {
            String _name_4 = supportedOption.getOption().getName();
            _elvis_4 = _name_4;
          }
          _builder_9.append(_elvis_4);
          _builder_9.append("](");
          _builder_9.append(optionFileLink);
          _builder_9.append(")");
          StringConcatenation _builder_10 = new StringConcatenation();
          _builder_10.append(" ");
          _builder_10.append("| `");
          String _elvis_5 = null;
          XExpression _value = supportedOption.getValue();
          String _text = null;
          if (_value!=null) {
            _text=this.getText(_value);
          }
          if (_text != null) {
            _elvis_5 = _text;
          } else {
            String _text_1 = this.getText(supportedOption.getOption().getDefaultValue());
            _elvis_5 = _text_1;
          }
          _builder_10.append(_elvis_5, " ");
          _builder_10.append("`");
          String _plus_10 = (_builder_9.toString() + _builder_10);
          String _plus_11 = (_plus_10 + 
            "\n");
          doc = (_doc_10 + _plus_11);
        }
      }
      String _doc_10 = doc;
      doc = (_doc_10 + "\n");
    }
    return doc;
  }

  /**
   * The dispatch method {@code generateDoc} is called for each {@link MdBundleMember} that is supposed to be
   * documented.
   * This generates a documentation String in Markdown format.
   * 
   * @param option
   *      the {@link MdOption} from the {@link MdModel}
   * @return
   *      the Markdown String
   */
  private String _generateDoc(final MdOption option) {
    String _elvis = null;
    String _label = option.getLabel();
    if (_label != null) {
      _elvis = _label;
    } else {
      String _name = option.getName();
      _elvis = _name;
    }
    return this.generateDoc(option, _elvis, 
      this.toHugoIdentifier(this.getQualifiedName(option)), 
      this.additionalDocumentation(option.getDocumentation(), this.toHugoIdentifier(this.getQualifiedName(option))));
  }

  /**
   * This method is wrapped by {@link #generateDoc(MdOption)}.
   * Here the title and additional documentation can be specified so that an algorithm that supports this option can
   * create an alternate version of this documentation, that is specific to the algorithm.
   * 
   * @param option
   *      the {@link MdOption} from the {@link MdModel}
   * @param title
   *      the title for the documentation page
   * @param id
   *      the identifier of the options page to be generated
   * @param additionalDoc
   *      a String of additional documentation that will be appended
   * @return
   *      the Markdown String
   * @see #additionalDocumentation
   */
  private String generateDoc(final MdOption option, final String title, final String id, final String additionalDoc) {
    StringConcatenation _builder = new StringConcatenation();
    _builder.append("---");
    _builder.newLine();
    _builder.append("title: \"");
    _builder.append(title);
    _builder.append("\"");
    _builder.newLineIfNotEmpty();
    _builder.append("displayid: \"");
    String _qualifiedName = this.getQualifiedName(option);
    _builder.append(_qualifiedName);
    _builder.append("\"");
    _builder.newLineIfNotEmpty();
    _builder.append("menu:");
    _builder.newLine();
    _builder.append("  ");
    _builder.append("main:");
    _builder.newLine();
    _builder.append("    ");
    _builder.append("identifier: \"option-");
    _builder.append(id, "    ");
    _builder.append("\"");
    _builder.newLineIfNotEmpty();
    _builder.append("    ");
    _builder.append("parent: \"LayoutOptions\"");
    _builder.newLine();
    _builder.append("---");
    _builder.newLine();
    _builder.newLine();
    String doc = _builder.toString();
    boolean _isDeprecated = option.isDeprecated();
    Pair<String, Boolean> _mappedTo = Pair.<String, Boolean>of("deprecated", Boolean.valueOf(_isDeprecated));
    boolean _isAdvanced = option.isAdvanced();
    Pair<String, Boolean> _mappedTo_1 = Pair.<String, Boolean>of("advanced", Boolean.valueOf(_isAdvanced));
    boolean _isProgrammatic = option.isProgrammatic();
    Pair<String, Boolean> _mappedTo_2 = Pair.<String, Boolean>of("programmatic", Boolean.valueOf(_isProgrammatic));
    boolean _isOutput = option.isOutput();
    Pair<String, Boolean> _mappedTo_3 = Pair.<String, Boolean>of("output", Boolean.valueOf(_isOutput));
    boolean _isGlobal = option.isGlobal();
    Pair<String, Boolean> _mappedTo_4 = Pair.<String, Boolean>of("global", Boolean.valueOf(_isGlobal));
    final Function1<Pair<String, Boolean>, Boolean> _function = new Function1<Pair<String, Boolean>, Boolean>() {
      @Override
      public Boolean apply(final Pair<String, Boolean> it) {
        return it.getValue();
      }
    };
    final Iterable<Pair<String, Boolean>> kinds = IterableExtensions.<Pair<String, Boolean>>filter(Collections.<Pair<String, Boolean>>unmodifiableList(CollectionLiterals.<Pair<String, Boolean>>newArrayList(_mappedTo, _mappedTo_1, _mappedTo_2, _mappedTo_3, _mappedTo_4)), _function);
    String _doc = doc;
    StringConcatenation _builder_1 = new StringConcatenation();
    _builder_1.newLine();
    _builder_1.append("Property | Value");
    _builder_1.newLine();
    _builder_1.append("-------- | -----");
    _builder_1.newLine();
    CharSequence _xifexpression = null;
    boolean _isEmpty = IterableExtensions.isEmpty(kinds);
    boolean _not = (!_isEmpty);
    if (_not) {
      StringConcatenation _builder_2 = new StringConcatenation();
      _builder_2.append("*Type:* | ");
      final Function1<Pair<String, Boolean>, String> _function_1 = new Function1<Pair<String, Boolean>, String>() {
        @Override
        public String apply(final Pair<String, Boolean> it) {
          return it.getKey();
        }
      };
      String _join = IterableExtensions.join(IterableExtensions.<Pair<String, Boolean>, String>map(kinds, _function_1), ", ");
      _builder_2.append(_join);
      _xifexpression = _builder_2;
    }
    _builder_1.append(_xifexpression);
    _builder_1.newLineIfNotEmpty();
    _builder_1.append("*Identifier:* | `");
    String _qualifiedName_1 = this.getQualifiedName(option);
    _builder_1.append(_qualifiedName_1);
    _builder_1.append("`");
    _builder_1.newLineIfNotEmpty();
    _builder_1.append("*Meta Data Provider:* | `");
    String _targetClass = this.getBundle(option).getTargetClass();
    _builder_1.append(_targetClass);
    _builder_1.append("`");
    _builder_1.newLineIfNotEmpty();
    _builder_1.append("*Value Type:* | `");
    JvmTypeReference _type = option.getType();
    String _identifier = null;
    if (_type!=null) {
      _identifier=_type.getIdentifier();
    }
    _builder_1.append(_identifier);
    _builder_1.append("`");
    String _xifexpression_1 = null;
    JvmTypeReference _type_1 = option.getType();
    JvmType _type_2 = null;
    if (_type_1!=null) {
      _type_2=_type_1.getType();
    }
    if ((_type_2 instanceof JvmEnumerationType)) {
      _xifexpression_1 = " (Enum)";
    }
    _builder_1.append(_xifexpression_1);
    _builder_1.newLineIfNotEmpty();
    String _xifexpression_2 = null;
    Iterable<? extends JvmField> _possibleValues = this.getPossibleValues(option.getType());
    boolean _tripleNotEquals = (_possibleValues != null);
    if (_tripleNotEquals) {
      final Function1<JvmField, String> _function_2 = new Function1<JvmField, String>() {
        @Override
        public String apply(final JvmField it) {
          String _simpleName = it.getSimpleName();
          String _plus = ("`" + _simpleName);
          String _plus_1 = (_plus + "`");
          EList<JvmAnnotationReference> _annotations = it.getAnnotations();
          List<JvmAnnotationReference> _sortBy = null;
          if (_annotations!=null) {
            final Function1<JvmAnnotationReference, String> _function = new Function1<JvmAnnotationReference, String>() {
              @Override
              public String apply(final JvmAnnotationReference it_1) {
                JvmAnnotationType _annotation = it_1.getAnnotation();
                String _simpleName = null;
                if (_annotation!=null) {
                  _simpleName=_annotation.getSimpleName();
                }
                return _simpleName;
              }
            };
            _sortBy=IterableExtensions.<JvmAnnotationReference, String>sortBy(_annotations, _function);
          }
          final Function1<JvmAnnotationReference, CharSequence> _function_1 = new Function1<JvmAnnotationReference, CharSequence>() {
            @Override
            public CharSequence apply(final JvmAnnotationReference it_1) {
              JvmAnnotationType _annotation = it_1.getAnnotation();
              String _simpleName = null;
              if (_annotation!=null) {
                _simpleName=_annotation.getSimpleName();
              }
              String _plus = ("*`@" + _simpleName);
              return (_plus + "`*");
            }
          };
          String _join = IterableExtensions.<JvmAnnotationReference>join(_sortBy, " (", " and ", ")", _function_1);
          return (_plus_1 + _join);
        }
      };
      String _join_1 = IterableExtensions.join(IterableExtensions.map(this.getPossibleValues(option.getType()), _function_2), "<br>");
      _xifexpression_2 = ("*Possible Values:* | " + _join_1);
    }
    _builder_1.append(_xifexpression_2);
    _builder_1.newLineIfNotEmpty();
    CharSequence _xifexpression_3 = null;
    XExpression _defaultValue = option.getDefaultValue();
    boolean _tripleNotEquals_1 = (_defaultValue != null);
    if (_tripleNotEquals_1) {
      StringConcatenation _builder_3 = new StringConcatenation();
      _builder_3.append("*Default Value:* | `");
      String _text = this.getText(option.getDefaultValue());
      _builder_3.append(_text);
      _builder_3.append("` (as defined in `");
      String _elvis = null;
      String _idPrefix = this.getBundle(option).getIdPrefix();
      if (_idPrefix != null) {
        _elvis = _idPrefix;
      } else {
        EObject _eContainer = this.getBundle(option).eContainer();
        String _name = ((MdModel) _eContainer).getName();
        _elvis = _name;
      }
      _builder_3.append(_elvis);
      _builder_3.append("`)");
      _xifexpression_3 = _builder_3;
    }
    _builder_1.append(_xifexpression_3);
    _builder_1.newLineIfNotEmpty();
    CharSequence _xifexpression_4 = null;
    XExpression _lowerBound = option.getLowerBound();
    boolean _tripleNotEquals_2 = (_lowerBound != null);
    if (_tripleNotEquals_2) {
      StringConcatenation _builder_4 = new StringConcatenation();
      _builder_4.append("*Lower Bound:* | `");
      String _text_1 = this.getText(option.getLowerBound());
      _builder_4.append(_text_1);
      _builder_4.append("`");
      _xifexpression_4 = _builder_4;
    }
    _builder_1.append(_xifexpression_4);
    _builder_1.newLineIfNotEmpty();
    CharSequence _xifexpression_5 = null;
    XExpression _upperBound = option.getUpperBound();
    boolean _tripleNotEquals_3 = (_upperBound != null);
    if (_tripleNotEquals_3) {
      StringConcatenation _builder_5 = new StringConcatenation();
      _builder_5.append("*Upper Bound:* | `");
      String _text_2 = this.getText(option.getUpperBound());
      _builder_5.append(_text_2);
      _builder_5.append("`");
      _xifexpression_5 = _builder_5;
    }
    _builder_1.append(_xifexpression_5);
    _builder_1.newLineIfNotEmpty();
    String _xifexpression_6 = null;
    boolean _isEmpty_1 = option.getTargets().isEmpty();
    boolean _not_1 = (!_isEmpty_1);
    if (_not_1) {
      final Function1<MdOptionTargetType, String> _function_3 = new Function1<MdOptionTargetType, String>() {
        @Override
        public String apply(final MdOptionTargetType it) {
          return it.getLiteral();
        }
      };
      String _join_2 = IterableExtensions.join(ListExtensions.<MdOptionTargetType, String>map(option.getTargets(), _function_3), ", ");
      _xifexpression_6 = ("*Applies To:* | " + _join_2);
    }
    _builder_1.append(_xifexpression_6);
    _builder_1.newLineIfNotEmpty();
    String _xifexpression_7 = null;
    boolean _isEmpty_2 = option.getLegacyIds().isEmpty();
    boolean _not_2 = (!_isEmpty_2);
    if (_not_2) {
      final Function1<String, String> _function_4 = new Function1<String, String>() {
        @Override
        public String apply(final String it) {
          return (("`" + it) + "`");
        }
      };
      String _join_3 = IterableExtensions.join(ListExtensions.<String, String>map(option.getLegacyIds(), _function_4), ", ");
      _xifexpression_7 = ("*Legacy Id:* | " + _join_3);
    }
    _builder_1.append(_xifexpression_7);
    _builder_1.newLineIfNotEmpty();
    String _xifexpression_8 = null;
    boolean _isEmpty_3 = option.getDependencies().isEmpty();
    boolean _not_3 = (!_isEmpty_3);
    if (_not_3) {
      final Function1<MdOptionDependency, String> _function_5 = new Function1<MdOptionDependency, String>() {
        @Override
        public String apply(final MdOptionDependency it) {
          return MelkDocumentationGenerator.this.toOptionDocsString(it);
        }
      };
      String _join_4 = IterableExtensions.join(ListExtensions.<MdOptionDependency, String>map(option.getDependencies(), _function_5), ", ");
      _xifexpression_8 = ("*Dependencies:* | " + _join_4);
    }
    _builder_1.append(_xifexpression_8);
    _builder_1.newLineIfNotEmpty();
    String _xifexpression_9 = null;
    boolean _isEmpty_4 = IterableExtensions.isEmpty(this.getGroups(option));
    boolean _not_4 = (!_isEmpty_4);
    if (_not_4) {
      final Function1<MdGroup, String> _function_6 = new Function1<MdGroup, String>() {
        @Override
        public String apply(final MdGroup it) {
          return MelkDocumentationGenerator.this.toOptionDocsString(it);
        }
      };
      String _join_5 = IterableExtensions.join(IterableExtensions.<MdGroup, String>map(this.getGroups(option), _function_6), " -> ");
      _xifexpression_9 = ("*Containing Group:* | " + _join_5);
    }
    _builder_1.append(_xifexpression_9);
    _builder_1.newLineIfNotEmpty();
    String _xifexpression_10 = null;
    String _description = option.getDescription();
    boolean _tripleNotEquals_4 = (_description != null);
    if (_tripleNotEquals_4) {
      String _trimNewlineTabsAndReduceToSingleSpace = this.trimNewlineTabsAndReduceToSingleSpace(option.getDescription());
      _xifexpression_10 = ("\n### Description\n\n" + _trimNewlineTabsAndReduceToSingleSpace);
    }
    _builder_1.append(_xifexpression_10);
    _builder_1.newLineIfNotEmpty();
    String _xifexpression_11 = null;
    boolean _isNullOrEmpty = StringExtensions.isNullOrEmpty(additionalDoc);
    boolean _not_5 = (!_isNullOrEmpty);
    if (_not_5) {
      _xifexpression_11 = ("\n## Additional Documentation\n\n" + additionalDoc);
    }
    _builder_1.append(_xifexpression_11);
    _builder_1.newLineIfNotEmpty();
    doc = (_doc + _builder_1);
    return doc;
  }

  /**
   * Generates a proper String representation for the given option dependency.
   */
  private String toOptionDocsString(final MdOptionDependency dependency) {
    String _qualifiedName = this.getQualifiedName(dependency.getTarget());
    String _plus = ("[" + _qualifiedName);
    String _plus_1 = (_plus + "]({{< relref \"reference/options/");
    String _hugoIdentifier = this.toHugoIdentifier(this.getQualifiedName(dependency.getTarget()));
    String _plus_2 = (_plus_1 + _hugoIdentifier);
    String result = (_plus_2 + ".md\" >}})");
    XExpression _value = dependency.getValue();
    boolean _tripleNotEquals = (_value != null);
    if (_tripleNotEquals) {
      String _result = result;
      String _text = this.getText(dependency.getValue());
      String _plus_3 = (" (`" + _text);
      String _plus_4 = (_plus_3 + "`)");
      result = (_result + _plus_4);
    }
    return result;
  }

  /**
   * Generates a proper String representation for the given group.
   */
  private String toOptionDocsString(final MdGroup group) {
    String _name = group.getName();
    String _plus = ("[" + _name);
    String _plus_1 = (_plus + "]({{< relref \"reference/groups/");
    String _hugoIdentifier = this.toHugoIdentifier(this.getQualifiedName(group));
    String _plus_2 = (_plus_1 + _hugoIdentifier);
    return (_plus_2 + ".md\" >}})");
  }

  /**
   * The dispatch method {@code generateDoc} is called for each {@link MdBundleMember} that is supposed to be
   * documented.
   * This generates a documentation String in Markdown format.
   * 
   * @param group
   *      the {@link MdGroup} from the {@link MdModel}
   * @return
   *      the Markdown String
   */
  private String _generateDoc(final MdGroup group) {
    final Function1<MdGroup, String> _function = new Function1<MdGroup, String>() {
      @Override
      public String apply(final MdGroup it) {
        return it.getName();
      }
    };
    String _join = IterableExtensions.join(IterableExtensions.<MdGroup, String>map(this.getGroups(group), _function), ".");
    final StringBuffer title = new StringBuffer(_join);
    int _length = title.length();
    boolean _equals = (_length == 0);
    if (_equals) {
      title.append(group.getName());
    } else {
      title.append(".").append(group.getName());
    }
    StringConcatenation _builder = new StringConcatenation();
    _builder.append("---");
    _builder.newLine();
    _builder.append("title: \"");
    String _string = title.toString();
    _builder.append(_string);
    _builder.append("\"");
    _builder.newLineIfNotEmpty();
    _builder.append("displayid: \"");
    String _qualifiedName = this.getQualifiedName(group);
    _builder.append(_qualifiedName);
    _builder.append("\"");
    _builder.newLineIfNotEmpty();
    _builder.append("menu:");
    _builder.newLine();
    _builder.append("  ");
    _builder.append("main:");
    _builder.newLine();
    _builder.append("    ");
    _builder.append("identifier: \"group-");
    String _hugoIdentifier = this.toHugoIdentifier(this.getQualifiedName(group));
    _builder.append(_hugoIdentifier, "    ");
    _builder.append("\"");
    _builder.newLineIfNotEmpty();
    _builder.append("    ");
    _builder.append("parent: \"LayoutOptionGroups\"");
    _builder.newLine();
    _builder.append("---");
    _builder.newLine();
    _builder.newLine();
    _builder.append("Property | Value");
    _builder.newLine();
    _builder.append("-------- | -----");
    _builder.newLine();
    _builder.append("*Identifier:* | `");
    String _qualifiedName_1 = this.getQualifiedName(group);
    _builder.append(_qualifiedName_1);
    _builder.append("`");
    _builder.newLineIfNotEmpty();
    _builder.newLine();
    String _xifexpression = null;
    boolean _isEmpty = IterableExtensions.isEmpty(Iterables.<MdOption>filter(group.getChildren(), MdOption.class));
    boolean _not = (!_isEmpty);
    if (_not) {
      final Function1<MdOption, String> _function_1 = new Function1<MdOption, String>() {
        @Override
        public String apply(final MdOption it) {
          String _elvis = null;
          String _label = it.getLabel();
          if (_label != null) {
            _elvis = _label;
          } else {
            String _name = it.getName();
            _elvis = _name;
          }
          String _plus = ("* [" + _elvis);
          String _plus_1 = (_plus + "]({{< relref \"reference/options/");
          String _hugoIdentifier = MelkDocumentationGenerator.this.toHugoIdentifier(MelkDocumentationGenerator.this.getQualifiedName(it));
          String _plus_2 = (_plus_1 + _hugoIdentifier);
          return (_plus_2 + ".md\" >}})");
        }
      };
      String _join_1 = IterableExtensions.join(IterableExtensions.<MdOption, String>map(Iterables.<MdOption>filter(group.getChildren(), MdOption.class), _function_1), "\n");
      _xifexpression = ("\n## Options\n\n" + _join_1);
    }
    _builder.append(_xifexpression);
    _builder.newLineIfNotEmpty();
    String _xifexpression_1 = null;
    boolean _isEmpty_1 = IterableExtensions.isEmpty(Iterables.<MdGroup>filter(group.getChildren(), MdGroup.class));
    boolean _not_1 = (!_isEmpty_1);
    if (_not_1) {
      final Function1<MdGroup, String> _function_2 = new Function1<MdGroup, String>() {
        @Override
        public String apply(final MdGroup it) {
          String _name = it.getName();
          String _plus = ("* [" + _name);
          String _plus_1 = (_plus + "]({{< relref \"reference/groups/");
          String _hugoIdentifier = MelkDocumentationGenerator.this.toHugoIdentifier(MelkDocumentationGenerator.this.getQualifiedName(it));
          String _plus_2 = (_plus_1 + _hugoIdentifier);
          return (_plus_2 + ".md\" >}})");
        }
      };
      String _join_2 = IterableExtensions.join(IterableExtensions.<MdGroup, String>map(Iterables.<MdGroup>filter(group.getChildren(), MdGroup.class), _function_2), "\n");
      _xifexpression_1 = ("\n## Subgroups\n\n" + _join_2);
    }
    _builder.append(_xifexpression_1);
    _builder.newLineIfNotEmpty();
    String _xifexpression_2 = null;
    String _documentation = group.getDocumentation();
    boolean _tripleNotEquals = (_documentation != null);
    if (_tripleNotEquals) {
      String _additionalDocumentation = this.additionalDocumentation(group.getDocumentation(), this.toHugoIdentifier(this.getQualifiedName(group)));
      _xifexpression_2 = ("\n## Additional Documentation\n\n" + _additionalDocumentation);
    }
    _builder.append(_xifexpression_2);
    _builder.newLineIfNotEmpty();
    String doc = _builder.toString();
    return doc;
  }

  /**
   * Extracts additional documentation from file or text.
   * A leading {@code @} signals that the following string is supposed to be interpreted as a file name.
   * The file has to be located at the path specified by the {@code documentationFolder} in the bundle section, that
   * defaults to "docs".
   * <p>
   * Images that are linked to in Markdown syntax are copied to the {@code outputPath} and their URLs are adjusted
   * accordingly.
   * </p>
   * 
   * @param documentation
   *      {@code @<file name>} or text in Markdown format
   * @param fileNamePrefix
   *      prefix of the file name for the copy to associate it with the member it belongs to
   * @return
   *      the Markdown String with replaced image references
   */
  private String additionalDocumentation(final String documentation, final String fileNamePrefix) {
    if ((documentation == null)) {
      return "";
    }
    String doc = "";
    boolean _startsWith = documentation.startsWith("@");
    boolean _not = (!_startsWith);
    if (_not) {
      doc = this.trimNewlineTabsAndReduceToSingleSpace(documentation);
    } else {
      try {
        doc = ((AbstractFileSystemAccess2) this.fsa).readTextFile(
          this.projectDocumentationSourceFolder.resolve(documentation.substring(1)).toString(), 
          MetaDataRuntimeModule.MelkOutputConfigurationProvider.AD_INPUT).toString();
      } catch (final Throwable _t) {
        if (_t instanceof Exception) {
          final Exception exception = (Exception)_t;
          exception.printStackTrace();
        } else {
          throw Exceptions.sneakyThrow(_t);
        }
      }
    }
    String res = "";
    int i = 0;
    int l = doc.length();
    while ((i < l)) {
      {
        String _res = res;
        char _charAt = doc.charAt(i);
        res = (_res + Character.valueOf(_charAt));
        i++;
        boolean _endsWith = res.endsWith("![");
        if (_endsWith) {
          while (((i < l) && (!res.endsWith("](")))) {
            {
              String _res_1 = res;
              char _charAt_1 = doc.charAt(i);
              res = (_res_1 + Character.valueOf(_charAt_1));
              i++;
            }
          }
          res = res.substring(0, res.lastIndexOf("!["));
          String path = "";
          int openedParens = 1;
          while (((i < l) && (openedParens > 0))) {
            {
              String _path = path;
              char _charAt_1 = doc.charAt(i);
              path = (_path + Character.valueOf(_charAt_1));
              i++;
              String _string = Character.valueOf(doc.charAt(i)).toString();
              boolean _matched = false;
              if (Objects.equal(_string, "(")) {
                _matched=true;
                openedParens++;
              }
              if (!_matched) {
                if (Objects.equal(_string, ")")) {
                  _matched=true;
                  openedParens--;
                }
              }
            }
          }
          String imgTitle = "";
          path = path.trim();
          boolean _endsWith_1 = path.endsWith("\"");
          if (_endsWith_1) {
            int _length = path.length();
            int _minus = (_length - 2);
            String _substring = path.substring(path.lastIndexOf("\"", _minus));
            String _plus = (" " + _substring);
            imgTitle = _plus;
            int _length_1 = path.length();
            int _minus_1 = (_length_1 - 2);
            path = path.substring(0, path.lastIndexOf("\"", _minus_1)).trim();
          }
          int _lastIndexOf = path.lastIndexOf("/");
          int _plus_1 = (_lastIndexOf + 1);
          String _substring_1 = path.substring(_plus_1);
          final String newFileName = ((fileNamePrefix + "_") + _substring_1);
          this.copyImageToOutputPath(this.projectDocumentationSourceFolder.resolve(path).toString(), newFileName);
          String _res_1 = res;
          res = (_res_1 + (((("{{< image src=\"" + newFileName) + "\" alt=\"") + imgTitle) + "\" gen=\"1\" >}}\n\n"));
          i++;
        }
      }
    }
    return res;
  }

  /**
   * The qualified name of a member consists of the bundles idPrefix, the containing groups and the members name
   * joined by dots.
   */
  private String getQualifiedName(final MdBundleMember member) {
    final MdBundle bundle = this.getBundle(member);
    EObject _eContainer = bundle.eContainer();
    final MdModel model = ((MdModel) _eContainer);
    String _elvis = null;
    String _idPrefix = bundle.getIdPrefix();
    if (_idPrefix != null) {
      _elvis = _idPrefix;
    } else {
      String _name = model.getName();
      _elvis = _name;
    }
    String prefix = _elvis;
    boolean _endsWith = prefix.endsWith(member.getName());
    if (_endsWith) {
      prefix = prefix.substring(0, prefix.lastIndexOf("."));
    }
    String _xifexpression = null;
    boolean _isEmpty = IterableExtensions.isEmpty(this.getGroups(member));
    if (_isEmpty) {
      _xifexpression = "";
    } else {
      _xifexpression = ".";
    }
    String _plus = (prefix + _xifexpression);
    final Function1<MdGroup, String> _function = new Function1<MdGroup, String>() {
      @Override
      public String apply(final MdGroup it) {
        return it.getName();
      }
    };
    String _join = IterableExtensions.join(IterableExtensions.<MdGroup, String>map(this.getGroups(member), _function), ".");
    String _plus_1 = (_plus + _join);
    String _plus_2 = (_plus_1 + ".");
    String _name_1 = member.getName();
    return (_plus_2 + _name_1);
  }

  /**
   * Turns the given option identifier into an identifier to be used by Hugo. This includes replacing dots by
   * hyphens, and ensuring that no identifier ends with "index" since that causes Hugo to do strange things.
   * 
   * @param optionName the option name to turn into an identifier fit for use with Hugo.
   * @return the Hugo-compatible identifier.
   */
  private String toHugoIdentifier(final String optionName) {
    boolean _endsWith = optionName.endsWith(".index");
    if (_endsWith) {
      return (optionName + "2").replace(".", "-");
    } else {
      return optionName.replace(".", "-");
    }
  }

  /**
   * Readable text representation of an XExpression.
   */
  private String getText(final XExpression exp) {
    if ((exp == null)) {
      return "<not defined>";
    }
    String text = NodeModelUtils.getNode(exp).getText().trim();
    int _lastIndexOf = text.lastIndexOf(".");
    int _plus = (_lastIndexOf + 1);
    final String fName = text.substring(_plus);
    if (((Character.isLowerCase(fName.charAt(0)) && (!Objects.equal(fName, text))) && (!fName.endsWith(")")))) {
      String _text = text;
      text = (_text + "()");
    }
    return text;
  }

  /**
   * Turns the string into something we can safely include in Markdown.
   */
  private String trimNewlineTabsAndReduceToSingleSpace(final String string) {
    return CharMatcher.breakingWhitespace().replaceFrom(string, " ").replaceAll(" +", " ");
  }

  /**
   * Possible values for enumeration types.
   */
  private Iterable<? extends JvmField> getPossibleValues(final JvmTypeReference type) {
    if ((type != null)) {
      final JvmType jvmType = type.getType();
      if ((jvmType instanceof JvmEnumerationType)) {
        final JvmEnumerationType enumType = ((JvmEnumerationType)jvmType);
        return enumType.getLiterals();
      } else {
        String _identifier = jvmType.getIdentifier();
        String _canonicalName = EnumSet.class.getCanonicalName();
        boolean _equals = Objects.equal(_identifier, _canonicalName);
        if (_equals) {
          JvmType _type = IterableExtensions.<JvmTypeReference>head(((JvmParameterizedTypeReference) type).getArguments()).getType();
          final JvmEnumerationType enumType_1 = ((JvmEnumerationType) _type);
          return enumType_1.getLiterals();
        }
      }
    }
    return null;
  }

  /**
   * Iterator for nested groups (flattened).
   */
  private Iterable<MdGroup> getAllGroupDefinitions(final Iterable<? extends MdBundleMember> elements) {
    final Iterable<MdGroup> groups = Iterables.<MdGroup>filter(elements, MdGroup.class);
    final Function1<MdGroup, Iterable<MdGroup>> _function = new Function1<MdGroup, Iterable<MdGroup>>() {
      @Override
      public Iterable<MdGroup> apply(final MdGroup it) {
        return MelkDocumentationGenerator.this.getAllGroupDefinitions(it.getChildren());
      }
    };
    return Iterables.<MdGroup>concat(groups, Iterables.<MdGroup>concat(IterableExtensions.<MdGroup, Iterable<MdGroup>>map(groups, _function)));
  }

  /**
   * Iterator for nested options (flattened).
   */
  private Iterable<MdOption> getAllOptionDefinitions(final Iterable<? extends MdBundleMember> elements) {
    final Function1<MdGroup, Iterable<MdOption>> _function = new Function1<MdGroup, Iterable<MdOption>>() {
      @Override
      public Iterable<MdOption> apply(final MdGroup it) {
        return MelkDocumentationGenerator.this.getAllOptionDefinitions(it.getChildren());
      }
    };
    return Iterables.<MdOption>concat(Iterables.<MdOption>filter(elements, MdOption.class), 
      Iterables.<MdOption>concat(IterableExtensions.<MdGroup, Iterable<MdOption>>map(Iterables.<MdGroup>filter(elements, MdGroup.class), _function)));
  }

  /**
   * Retrieve bundle associated with a member.
   */
  private MdBundle getBundle(final MdBundleMember member) {
    EObject parent = member.eContainer();
    while ((!(parent instanceof MdBundle))) {
      parent = parent.eContainer();
    }
    return ((MdBundle) parent);
  }

  /**
   * Groups containing this member.
   */
  private Iterable<MdGroup> getGroups(final MdBundleMember member) {
    final LinkedList<MdGroup> groups = new LinkedList<MdGroup>();
    EObject group = member.eContainer();
    while ((group instanceof MdGroup)) {
      {
        groups.addFirst(((MdGroup)group));
        group = ((MdGroup)group).eContainer();
      }
    }
    return groups;
  }

  public void internalDoGenerate(final EObject model, final IFileSystemAccess fsa) {
    if (model instanceof JvmDeclaredType) {
      _internalDoGenerate((JvmDeclaredType)model, fsa);
      return;
    } else if (model instanceof MdModel) {
      _internalDoGenerate((MdModel)model, fsa);
      return;
    } else if (model != null) {
      _internalDoGenerate(model, fsa);
      return;
    } else {
      throw new IllegalArgumentException("Unhandled parameter types: " +
        Arrays.<Object>asList(model, fsa).toString());
    }
  }

  private String generateDoc(final MdBundleMember group) {
    if (group instanceof MdGroup) {
      return _generateDoc((MdGroup)group);
    } else if (group instanceof MdOption) {
      return _generateDoc((MdOption)group);
    } else if (group instanceof MdAlgorithm) {
      return _generateDoc((MdAlgorithm)group);
    } else {
      throw new IllegalArgumentException("Unhandled parameter types: " +
        Arrays.<Object>asList(group).toString());
    }
  }
}
