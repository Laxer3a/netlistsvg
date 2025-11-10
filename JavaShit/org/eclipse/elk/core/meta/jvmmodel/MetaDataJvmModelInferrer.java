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

import com.google.common.base.Objects;
import com.google.common.collect.Iterables;
import com.google.inject.Inject;
import java.util.Arrays;
import java.util.EnumSet;
import java.util.LinkedList;
import java.util.List;
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
import org.eclipse.elk.graph.properties.IProperty;
import org.eclipse.elk.graph.properties.Property;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.xtend2.lib.StringConcatenation;
import org.eclipse.xtend2.lib.StringConcatenationClient;
import org.eclipse.xtext.common.types.JvmAnnotationReference;
import org.eclipse.xtext.common.types.JvmDeclaredType;
import org.eclipse.xtext.common.types.JvmEnumerationType;
import org.eclipse.xtext.common.types.JvmField;
import org.eclipse.xtext.common.types.JvmFormalParameter;
import org.eclipse.xtext.common.types.JvmGenericType;
import org.eclipse.xtext.common.types.JvmMember;
import org.eclipse.xtext.common.types.JvmOperation;
import org.eclipse.xtext.common.types.JvmParameterizedTypeReference;
import org.eclipse.xtext.common.types.JvmPrimitiveType;
import org.eclipse.xtext.common.types.JvmType;
import org.eclipse.xtext.common.types.JvmTypeReference;
import org.eclipse.xtext.common.types.JvmVisibility;
import org.eclipse.xtext.common.types.util.Primitives;
import org.eclipse.xtext.util.Strings;
import org.eclipse.xtext.xbase.XExpression;
import org.eclipse.xtext.xbase.jvmmodel.AbstractModelInferrer;
import org.eclipse.xtext.xbase.jvmmodel.IJvmDeclaredTypeAcceptor;
import org.eclipse.xtext.xbase.jvmmodel.JvmTypesBuilder;
import org.eclipse.xtext.xbase.lib.Conversions;
import org.eclipse.xtext.xbase.lib.Extension;
import org.eclipse.xtext.xbase.lib.Functions.Function1;
import org.eclipse.xtext.xbase.lib.IterableExtensions;
import org.eclipse.xtext.xbase.lib.ListExtensions;
import org.eclipse.xtext.xbase.lib.Procedures.Procedure1;
import org.eclipse.xtext.xbase.lib.StringExtensions;

/**
 * Infers a JVM model from the source model.
 * 
 * <p>The JVM model should contain all elements that would appear in the Java code which is generated from
 * the source model. Other models link against the JVM model rather than the source model.</p>
 */
@SuppressWarnings("all")
public class MetaDataJvmModelInferrer extends AbstractModelInferrer {
  /**
   * Convenience API to build and initialize JVM types and their members.
   */
  @Inject
  @Extension
  private JvmTypesBuilder _jvmTypesBuilder;

  /**
   * Convenience API to wrap primitives.
   */
  @Inject
  @Extension
  private Primitives _primitives;

  /**
   * The dispatch method {@code infer} is called for each instance of the
   * given element's type that is contained in a resource.
   * 
   * @param model
   *            the model to create one or more {@link JvmDeclaredType declared types} from.
   * @param acceptor
   *            each created {@link JvmDeclaredType type} without a container should be passed to the acceptor
   *            in order to get attached to the current resource. The acceptor's
   *            {@link IJvmDeclaredTypeAcceptor#accept(org.eclipse.xtext.common.types.JvmDeclaredType) accept(..)}
   *            method takes the constructed empty type for the pre-indexing phase. This one is further
   *            initialized in the indexing phase using passed closure.
   * @param isPreIndexingPhase
   *            whether the method is called in a pre-indexing phase, i.e.
   *            when the global index is not yet fully updated. You must not
   *            rely on linking using the index if isPreIndexingPhase is
   *            <code>true</code>.
   */
  protected void _infer(final MdModel model, final IJvmDeclaredTypeAcceptor acceptor, final boolean isPreIndexingPhase) {
    if (((model.getName() == null) || (model.getBundle() == null))) {
      return;
    }
    final MdBundle bundle = model.getBundle();
    final Procedure1<JvmGenericType> _function = new Procedure1<JvmGenericType>() {
      @Override
      public void apply(final JvmGenericType it) {
        EList<JvmTypeReference> _superTypes = it.getSuperTypes();
        JvmTypeReference _typeRef = MetaDataJvmModelInferrer.this._typeReferenceBuilder.typeRef("org.eclipse.elk.core.data.ILayoutMetaDataProvider");
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmTypeReference>operator_add(_superTypes, _typeRef);
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setFileHeader(it, MetaDataJvmModelInferrer.this._jvmTypesBuilder.getDocumentation(model));
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setDocumentation(it, MetaDataJvmModelInferrer.this._jvmTypesBuilder.getDocumentation(bundle));
        Iterable<MdOption> _allOptionDefinitions = MetaDataJvmModelInferrer.this.getAllOptionDefinitions(bundle.getMembers());
        for (final MdOption property : _allOptionDefinitions) {
          {
            final JvmField constant = MetaDataJvmModelInferrer.this.toOptionConstant(property);
            XExpression _defaultValue = property.getDefaultValue();
            boolean _tripleNotEquals = (_defaultValue != null);
            if (_tripleNotEquals) {
              EList<JvmMember> _members = it.getMembers();
              JvmField _optionDefault = MetaDataJvmModelInferrer.this.toOptionDefault(property);
              MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmField>operator_add(_members, _optionDefault);
            }
            XExpression _lowerBound = property.getLowerBound();
            boolean _tripleNotEquals_1 = (_lowerBound != null);
            if (_tripleNotEquals_1) {
              EList<JvmMember> _members_1 = it.getMembers();
              JvmField _optionLowerBound = MetaDataJvmModelInferrer.this.toOptionLowerBound(property);
              MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmField>operator_add(_members_1, _optionLowerBound);
            }
            XExpression _upperBound = property.getUpperBound();
            boolean _tripleNotEquals_2 = (_upperBound != null);
            if (_tripleNotEquals_2) {
              EList<JvmMember> _members_2 = it.getMembers();
              JvmField _optionUpperBound = MetaDataJvmModelInferrer.this.toOptionUpperBound(property);
              MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmField>operator_add(_members_2, _optionUpperBound);
            }
            EList<JvmMember> _members_3 = it.getMembers();
            MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmField>operator_add(_members_3, constant);
          }
        }
        Iterable<MdOption> _allOptionDefinitions_1 = MetaDataJvmModelInferrer.this.getAllOptionDefinitions(bundle.getMembers());
        for (final MdOption property_1 : _allOptionDefinitions_1) {
          EList<MdOptionDependency> _dependencies = property_1.getDependencies();
          for (final MdOptionDependency dependency : _dependencies) {
            XExpression _value = dependency.getValue();
            boolean _tripleNotEquals = (_value != null);
            if (_tripleNotEquals) {
              EList<JvmMember> _members = it.getMembers();
              JvmField _dependencyValue = MetaDataJvmModelInferrer.this.toDependencyValue(dependency);
              MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmField>operator_add(_members, _dependencyValue);
            }
          }
        }
        EList<JvmMember> _members_1 = it.getMembers();
        final Procedure1<JvmOperation> _function = new Procedure1<JvmOperation>() {
          @Override
          public void apply(final JvmOperation it_1) {
            EList<JvmFormalParameter> _parameters = it_1.getParameters();
            JvmFormalParameter _parameter = MetaDataJvmModelInferrer.this._jvmTypesBuilder.toParameter(bundle, "registry", MetaDataJvmModelInferrer.this._typeReferenceBuilder.typeRef("org.eclipse.elk.core.data.ILayoutMetaDataProvider.Registry"));
            MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmFormalParameter>operator_add(_parameters, _parameter);
            StringConcatenationClient _client = new StringConcatenationClient() {
              @Override
              protected void appendTo(StringConcatenationClient.TargetStringConcatenation _builder) {
                StringConcatenationClient _registerLayoutOptions = MetaDataJvmModelInferrer.this.registerLayoutOptions(bundle);
                _builder.append(_registerLayoutOptions);
                _builder.newLineIfNotEmpty();
                StringConcatenationClient _registerLayoutCategories = MetaDataJvmModelInferrer.this.registerLayoutCategories(bundle);
                _builder.append(_registerLayoutCategories);
                _builder.newLineIfNotEmpty();
                StringConcatenationClient _registerLayoutAlgorithms = MetaDataJvmModelInferrer.this.registerLayoutAlgorithms(bundle);
                _builder.append(_registerLayoutAlgorithms);
                _builder.newLineIfNotEmpty();
              }
            };
            MetaDataJvmModelInferrer.this._jvmTypesBuilder.setBody(it_1, _client);
          }
        };
        JvmOperation _method = MetaDataJvmModelInferrer.this._jvmTypesBuilder.toMethod(bundle, "apply", MetaDataJvmModelInferrer.this._typeReferenceBuilder.typeRef(void.class), _function);
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmOperation>operator_add(_members_1, _method);
      }
    };
    acceptor.<JvmGenericType>accept(this._jvmTypesBuilder.toClass(bundle, this.getQualifiedTargetClass(bundle)), _function);
    Iterable<MdAlgorithm> _filter = Iterables.<MdAlgorithm>filter(bundle.getMembers(), MdAlgorithm.class);
    for (final MdAlgorithm algorithm : _filter) {
      this.inferAlgorithm(model, algorithm, acceptor, isPreIndexingPhase);
    }
  }

  private JvmField toOptionConstant(final MdOption option) {
    JvmTypeReference _elvis = null;
    JvmTypeReference _type = option.getType();
    if (_type != null) {
      _elvis = _type;
    } else {
      JvmTypeReference _typeRef = this._typeReferenceBuilder.typeRef(Object.class);
      _elvis = _typeRef;
    }
    final Procedure1<JvmField> _function = new Procedure1<JvmField>() {
      @Override
      public void apply(final JvmField it) {
        it.setVisibility(JvmVisibility.PUBLIC);
        it.setStatic(true);
        it.setFinal(true);
        StringConcatenationClient _client = new StringConcatenationClient() {
          @Override
          protected void appendTo(StringConcatenationClient.TargetStringConcatenation _builder) {
            _builder.append("new ");
            _builder.append(Property.class);
            _builder.append("<");
            JvmTypeReference _elvis = null;
            JvmTypeReference _asWrapperTypeIfPrimitive = MetaDataJvmModelInferrer.this._primitives.asWrapperTypeIfPrimitive(option.getType());
            if (_asWrapperTypeIfPrimitive != null) {
              _elvis = _asWrapperTypeIfPrimitive;
            } else {
              JvmTypeReference _typeRef = MetaDataJvmModelInferrer.this._typeReferenceBuilder.typeRef(Object.class);
              _elvis = _typeRef;
            }
            _builder.append(_elvis);
            _builder.append(">(");
            _builder.newLineIfNotEmpty();
            _builder.append("        ");
            String _codeString = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.getQualifiedName(option));
            _builder.append(_codeString, "        ");
            {
              boolean _hasDefaultOrBounds = MetaDataJvmModelInferrer.this.hasDefaultOrBounds(option);
              if (_hasDefaultOrBounds) {
                _builder.append(",");
                _builder.newLineIfNotEmpty();
                _builder.append("        ");
                {
                  XExpression _defaultValue = option.getDefaultValue();
                  boolean _tripleNotEquals = (_defaultValue != null);
                  if (_tripleNotEquals) {
                    String _defaultConstantName = MetaDataJvmModelInferrer.this.getDefaultConstantName(option);
                    _builder.append(_defaultConstantName, "        ");
                  } else {
                    _builder.append("null");
                  }
                }
                _builder.append(",");
                _builder.newLineIfNotEmpty();
                _builder.append("        ");
                {
                  XExpression _lowerBound = option.getLowerBound();
                  boolean _tripleNotEquals_1 = (_lowerBound != null);
                  if (_tripleNotEquals_1) {
                    String _lowerBoundConstantName = MetaDataJvmModelInferrer.this.getLowerBoundConstantName(option);
                    _builder.append(_lowerBoundConstantName, "        ");
                  } else {
                    _builder.append("null");
                  }
                }
                _builder.append(",");
                _builder.newLineIfNotEmpty();
                _builder.append("        ");
                {
                  XExpression _upperBound = option.getUpperBound();
                  boolean _tripleNotEquals_2 = (_upperBound != null);
                  if (_tripleNotEquals_2) {
                    String _upperBoundConstantName = MetaDataJvmModelInferrer.this.getUpperBoundConstantName(option);
                    _builder.append(_upperBoundConstantName, "        ");
                  } else {
                    _builder.append("null");
                  }
                }
              }
            }
            _builder.append(")");
          }
        };
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setInitializer(it, _client);
        boolean _isDeprecated = option.isDeprecated();
        if (_isDeprecated) {
          EList<JvmAnnotationReference> _annotations = it.getAnnotations();
          JvmAnnotationReference _annotationRef = MetaDataJvmModelInferrer.this._annotationTypesBuilder.annotationRef(Deprecated.class);
          MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmAnnotationReference>operator_add(_annotations, _annotationRef);
          it.setDeprecated(true);
        }
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setDocumentation(it, MetaDataJvmModelInferrer.this.trimLines(option.getDescription()));
      }
    };
    return this._jvmTypesBuilder.toField(option, 
      this.getConstantName(option), 
      this._typeReferenceBuilder.typeRef(IProperty.class, _elvis), _function);
  }

  private boolean hasDefaultOrBounds(final MdOption option) {
    return (((option.getDefaultValue() != null) || (option.getLowerBound() != null)) || (option.getUpperBound() != null));
  }

  private JvmField toOptionDefault(final MdOption option) {
    JvmTypeReference _elvis = null;
    JvmTypeReference _cloneWithProxies = this._jvmTypesBuilder.cloneWithProxies(option.getType());
    if (_cloneWithProxies != null) {
      _elvis = _cloneWithProxies;
    } else {
      JvmTypeReference _typeRef = this._typeReferenceBuilder.typeRef(Object.class);
      _elvis = _typeRef;
    }
    final JvmTypeReference optionType = _elvis;
    final Procedure1<JvmField> _function = new Procedure1<JvmField>() {
      @Override
      public void apply(final JvmField it) {
        it.setVisibility(JvmVisibility.PRIVATE);
        it.setStatic(true);
        it.setFinal(true);
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setInitializer(it, option.getDefaultValue());
        StringConcatenation _builder = new StringConcatenation();
        _builder.append("Default value for {@link #");
        String _constantName = MetaDataJvmModelInferrer.this.getConstantName(option);
        _builder.append(_constantName);
        _builder.append("}.");
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setDocumentation(it, _builder.toString());
      }
    };
    return this._jvmTypesBuilder.toField(option, this.getDefaultConstantName(option), optionType, _function);
  }

  private JvmField toOptionLowerBound(final MdOption option) {
    JvmTypeReference _elvis = null;
    JvmTypeReference _asWrapperTypeIfPrimitive = this._primitives.asWrapperTypeIfPrimitive(option.getType());
    JvmTypeReference _wildcardSuper = null;
    if (_asWrapperTypeIfPrimitive!=null) {
      _wildcardSuper=this._typeReferenceBuilder.wildcardSuper(_asWrapperTypeIfPrimitive);
    }
    if (_wildcardSuper != null) {
      _elvis = _wildcardSuper;
    } else {
      JvmTypeReference _wildcard = this._typeReferenceBuilder.wildcard();
      _elvis = _wildcard;
    }
    final JvmTypeReference optionType = this._typeReferenceBuilder.typeRef(Comparable.class, _elvis);
    final Procedure1<JvmField> _function = new Procedure1<JvmField>() {
      @Override
      public void apply(final JvmField it) {
        it.setVisibility(JvmVisibility.PRIVATE);
        it.setStatic(true);
        it.setFinal(true);
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setInitializer(it, option.getLowerBound());
        StringConcatenation _builder = new StringConcatenation();
        _builder.append("Lower bound value for {@link #");
        String _constantName = MetaDataJvmModelInferrer.this.getConstantName(option);
        _builder.append(_constantName);
        _builder.append("}.");
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setDocumentation(it, _builder.toString());
      }
    };
    return this._jvmTypesBuilder.toField(option, this.getLowerBoundConstantName(option), optionType, _function);
  }

  private JvmField toOptionUpperBound(final MdOption option) {
    JvmTypeReference _elvis = null;
    JvmTypeReference _asWrapperTypeIfPrimitive = this._primitives.asWrapperTypeIfPrimitive(option.getType());
    JvmTypeReference _wildcardSuper = null;
    if (_asWrapperTypeIfPrimitive!=null) {
      _wildcardSuper=this._typeReferenceBuilder.wildcardSuper(_asWrapperTypeIfPrimitive);
    }
    if (_wildcardSuper != null) {
      _elvis = _wildcardSuper;
    } else {
      JvmTypeReference _wildcard = this._typeReferenceBuilder.wildcard();
      _elvis = _wildcard;
    }
    final JvmTypeReference optionType = this._typeReferenceBuilder.typeRef(Comparable.class, _elvis);
    final Procedure1<JvmField> _function = new Procedure1<JvmField>() {
      @Override
      public void apply(final JvmField it) {
        it.setVisibility(JvmVisibility.PRIVATE);
        it.setStatic(true);
        it.setFinal(true);
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setInitializer(it, option.getUpperBound());
        StringConcatenation _builder = new StringConcatenation();
        _builder.append("Upper bound value for {@link #");
        String _constantName = MetaDataJvmModelInferrer.this.getConstantName(option);
        _builder.append(_constantName);
        _builder.append("}.");
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setDocumentation(it, _builder.toString());
      }
    };
    return this._jvmTypesBuilder.toField(option, this.getUpperBoundConstantName(option), optionType, _function);
  }

  private JvmField toDependencyValue(final MdOptionDependency dependency) {
    EObject _eContainer = dependency.eContainer();
    final MdOption source = ((MdOption) _eContainer);
    JvmTypeReference _elvis = null;
    JvmTypeReference _cloneWithProxies = this._jvmTypesBuilder.cloneWithProxies(dependency.getTarget().getType());
    if (_cloneWithProxies != null) {
      _elvis = _cloneWithProxies;
    } else {
      JvmTypeReference _typeRef = this._typeReferenceBuilder.typeRef(Object.class);
      _elvis = _typeRef;
    }
    final JvmTypeReference optionType = _elvis;
    final Procedure1<JvmField> _function = new Procedure1<JvmField>() {
      @Override
      public void apply(final JvmField it) {
        it.setVisibility(JvmVisibility.PRIVATE);
        it.setStatic(true);
        it.setFinal(true);
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setInitializer(it, dependency.getValue());
        StringConcatenation _builder = new StringConcatenation();
        _builder.append("Required value for dependency between {@link #");
        String _constantName = MetaDataJvmModelInferrer.this.getConstantName(source);
        _builder.append(_constantName);
        _builder.append("} and {@link #");
        String _constantName_1 = MetaDataJvmModelInferrer.this.getConstantName(dependency.getTarget());
        _builder.append(_constantName_1);
        _builder.append("}.");
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setDocumentation(it, _builder.toString());
      }
    };
    return this._jvmTypesBuilder.toField(dependency, this.getDependencyConstantName(dependency), optionType, _function);
  }

  private StringConcatenationClient registerLayoutOptions(final MdBundle bundle) {
    StringConcatenationClient _client = new StringConcatenationClient() {
      @Override
      protected void appendTo(StringConcatenationClient.TargetStringConcatenation _builder) {
        {
          Iterable<MdOption> _allOptionDefinitions = MetaDataJvmModelInferrer.this.getAllOptionDefinitions(bundle.getMembers());
          for(final MdOption option : _allOptionDefinitions) {
            _builder.append("registry.register(new ");
            JvmTypeReference _LayoutOptionData = MetaDataJvmModelInferrer.this.LayoutOptionData();
            _builder.append(_LayoutOptionData);
            _builder.append(".Builder()");
            _builder.newLineIfNotEmpty();
            _builder.append("    ");
            _builder.append(".id(");
            String _codeString = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.getQualifiedName(option));
            _builder.append(_codeString, "    ");
            _builder.append(")");
            _builder.newLineIfNotEmpty();
            _builder.append("    ");
            _builder.append(".group(");
            final Function1<MdGroup, String> _function = new Function1<MdGroup, String>() {
              @Override
              public String apply(final MdGroup it) {
                return it.getName();
              }
            };
            String _codeString_1 = MetaDataJvmModelInferrer.this.toCodeString(IterableExtensions.join(IterableExtensions.<MdGroup, String>map(MetaDataJvmModelInferrer.this.getGroups(option), _function), "."));
            _builder.append(_codeString_1, "    ");
            _builder.append(")");
            _builder.newLineIfNotEmpty();
            _builder.append("    ");
            _builder.append(".name(");
            String _elvis = null;
            String _label = option.getLabel();
            if (_label != null) {
              _elvis = _label;
            } else {
              String _name = option.getName();
              _elvis = _name;
            }
            String _codeString_2 = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.shrinkWhiteSpace(_elvis));
            _builder.append(_codeString_2, "    ");
            _builder.append(")");
            _builder.newLineIfNotEmpty();
            _builder.append("    ");
            _builder.append(".description(");
            String _codeString_3 = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.shrinkWhiteSpace(option.getDescription()));
            _builder.append(_codeString_3, "    ");
            _builder.append(")");
            _builder.newLineIfNotEmpty();
            {
              XExpression _defaultValue = option.getDefaultValue();
              boolean _tripleNotEquals = (_defaultValue != null);
              if (_tripleNotEquals) {
                _builder.append("    ");
                _builder.append(".defaultValue(");
                String _defaultConstantName = MetaDataJvmModelInferrer.this.getDefaultConstantName(option);
                _builder.append(_defaultConstantName, "    ");
                _builder.append(")");
                _builder.newLineIfNotEmpty();
              }
            }
            {
              XExpression _lowerBound = option.getLowerBound();
              boolean _tripleNotEquals_1 = (_lowerBound != null);
              if (_tripleNotEquals_1) {
                _builder.append("    ");
                _builder.append(".lowerBound(");
                String _lowerBoundConstantName = MetaDataJvmModelInferrer.this.getLowerBoundConstantName(option);
                _builder.append(_lowerBoundConstantName, "    ");
                _builder.append(")");
                _builder.newLineIfNotEmpty();
              }
            }
            {
              XExpression _upperBound = option.getUpperBound();
              boolean _tripleNotEquals_2 = (_upperBound != null);
              if (_tripleNotEquals_2) {
                _builder.append("    ");
                _builder.append(".upperBound(");
                String _upperBoundConstantName = MetaDataJvmModelInferrer.this.getUpperBoundConstantName(option);
                _builder.append(_upperBoundConstantName, "    ");
                _builder.append(")");
                _builder.newLineIfNotEmpty();
              }
            }
            _builder.append("    ");
            _builder.append(".type(");
            JvmTypeReference _LayoutOptionData_1 = MetaDataJvmModelInferrer.this.LayoutOptionData();
            _builder.append(_LayoutOptionData_1, "    ");
            _builder.append(".Type.");
            String _optionType = MetaDataJvmModelInferrer.this.getOptionType(option);
            _builder.append(_optionType, "    ");
            _builder.append(")");
            _builder.newLineIfNotEmpty();
            _builder.append("    ");
            _builder.append(".optionClass(");
            JvmTypeReference _optionTypeClass = MetaDataJvmModelInferrer.this.getOptionTypeClass(option);
            _builder.append(_optionTypeClass, "    ");
            _builder.append(".class)");
            _builder.newLineIfNotEmpty();
            {
              boolean _isEmpty = option.getTargets().isEmpty();
              boolean _not = (!_isEmpty);
              if (_not) {
                _builder.append("    ");
                _builder.append(".targets(");
                _builder.append(EnumSet.class, "    ");
                _builder.append(".of(");
                {
                  EList<MdOptionTargetType> _targets = option.getTargets();
                  boolean _hasElements = false;
                  for(final MdOptionTargetType t : _targets) {
                    if (!_hasElements) {
                      _hasElements = true;
                    } else {
                      _builder.appendImmediate(", ", "    ");
                    }
                    JvmTypeReference _LayoutOptionData_2 = MetaDataJvmModelInferrer.this.LayoutOptionData();
                    _builder.append(_LayoutOptionData_2, "    ");
                    _builder.append(".Target.");
                    String _upperCase = t.toString().toUpperCase();
                    _builder.append(_upperCase, "    ");
                  }
                }
                _builder.append("))");
                _builder.newLineIfNotEmpty();
              }
            }
            {
              if (((option.isProgrammatic() || option.isOutput()) || option.isGlobal())) {
                _builder.append("    ");
                _builder.append(".visibility(");
                JvmTypeReference _LayoutOptionData_3 = MetaDataJvmModelInferrer.this.LayoutOptionData();
                _builder.append(_LayoutOptionData_3, "    ");
                _builder.append(".Visibility.HIDDEN)");
                _builder.newLineIfNotEmpty();
              } else {
                boolean _isAdvanced = option.isAdvanced();
                if (_isAdvanced) {
                  _builder.append("    ");
                  _builder.append(".visibility(");
                  JvmTypeReference _LayoutOptionData_4 = MetaDataJvmModelInferrer.this.LayoutOptionData();
                  _builder.append(_LayoutOptionData_4, "    ");
                  _builder.append(".Visibility.ADVANCED)");
                  _builder.newLineIfNotEmpty();
                } else {
                  _builder.append("    ");
                  _builder.append(".visibility(");
                  JvmTypeReference _LayoutOptionData_5 = MetaDataJvmModelInferrer.this.LayoutOptionData();
                  _builder.append(_LayoutOptionData_5, "    ");
                  _builder.append(".Visibility.VISIBLE)");
                  _builder.newLineIfNotEmpty();
                }
              }
            }
            {
              boolean _isEmpty_1 = option.getLegacyIds().isEmpty();
              boolean _not_1 = (!_isEmpty_1);
              if (_not_1) {
                _builder.append("    ");
                _builder.append(".legacyIds(");
                final Function1<String, String> _function_1 = new Function1<String, String>() {
                  @Override
                  public String apply(final String it) {
                    return (("\"" + it) + "\"");
                  }
                };
                String _join = IterableExtensions.join(ListExtensions.<String, String>map(option.getLegacyIds(), _function_1), ", ");
                _builder.append(_join, "    ");
                _builder.append(")");
                _builder.newLineIfNotEmpty();
              }
            }
            _builder.append("    ");
            _builder.append(".create()");
            _builder.newLine();
            _builder.append(");");
            _builder.newLine();
            {
              EList<MdOptionDependency> _dependencies = option.getDependencies();
              for(final MdOptionDependency dependency : _dependencies) {
                _builder.append("registry.addDependency(");
                _builder.newLine();
                _builder.append("    ");
                String _codeString_4 = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.getQualifiedName(option));
                _builder.append(_codeString_4, "    ");
                _builder.append(",");
                _builder.newLineIfNotEmpty();
                _builder.append("    ");
                String _codeString_5 = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.getQualifiedName(dependency.getTarget()));
                _builder.append(_codeString_5, "    ");
                _builder.append(",");
                _builder.newLineIfNotEmpty();
                {
                  XExpression _value = dependency.getValue();
                  boolean _tripleEquals = (_value == null);
                  if (_tripleEquals) {
                    _builder.append("    ");
                    _builder.append("null");
                    _builder.newLine();
                  } else {
                    _builder.append("    ");
                    String _dependencyConstantName = MetaDataJvmModelInferrer.this.getDependencyConstantName(dependency);
                    _builder.append(_dependencyConstantName, "    ");
                    _builder.newLineIfNotEmpty();
                  }
                }
                _builder.append(");");
                _builder.newLine();
              }
            }
          }
        }
      }
    };
    return _client;
  }

  private JvmTypeReference LayoutOptionData() {
    return this._typeReferenceBuilder.typeRef("org.eclipse.elk.core.data.LayoutOptionData");
  }

  private StringConcatenationClient registerLayoutCategories(final MdBundle bundle) {
    StringConcatenationClient _client = new StringConcatenationClient() {
      @Override
      protected void appendTo(StringConcatenationClient.TargetStringConcatenation _builder) {
        {
          Iterable<MdCategory> _filter = Iterables.<MdCategory>filter(bundle.getMembers(), MdCategory.class);
          for(final MdCategory category : _filter) {
            _builder.append("registry.register(new ");
            JvmTypeReference _LayoutCategoryData = MetaDataJvmModelInferrer.this.LayoutCategoryData();
            _builder.append(_LayoutCategoryData);
            _builder.append(".Builder()");
            _builder.newLineIfNotEmpty();
            _builder.append("    ");
            _builder.append(".id(");
            String _codeString = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.getQualifiedName(category));
            _builder.append(_codeString, "    ");
            _builder.append(")");
            _builder.newLineIfNotEmpty();
            _builder.append("    ");
            _builder.append(".name(");
            String _elvis = null;
            String _label = category.getLabel();
            if (_label != null) {
              _elvis = _label;
            } else {
              String _name = category.getName();
              _elvis = _name;
            }
            String _codeString_1 = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.shrinkWhiteSpace(_elvis));
            _builder.append(_codeString_1, "    ");
            _builder.append(")");
            _builder.newLineIfNotEmpty();
            _builder.append("    ");
            _builder.append(".description(");
            String _codeString_2 = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.shrinkWhiteSpace(category.getDescription()));
            _builder.append(_codeString_2, "    ");
            _builder.append(")");
            _builder.newLineIfNotEmpty();
            _builder.append("    ");
            _builder.append(".create()");
            _builder.newLine();
            _builder.append(");");
            _builder.newLine();
          }
        }
      }
    };
    return _client;
  }

  private JvmTypeReference LayoutCategoryData() {
    return this._typeReferenceBuilder.typeRef("org.eclipse.elk.core.data.LayoutCategoryData");
  }

  private StringConcatenationClient registerLayoutAlgorithms(final MdBundle bundle) {
    StringConcatenationClient _client = new StringConcatenationClient() {
      @Override
      protected void appendTo(StringConcatenationClient.TargetStringConcatenation _builder) {
        {
          Iterable<MdAlgorithm> _filter = Iterables.<MdAlgorithm>filter(bundle.getMembers(), MdAlgorithm.class);
          for(final MdAlgorithm algorithm : _filter) {
            _builder.append("new ");
            String _qualifiedTargetClass = MetaDataJvmModelInferrer.this.getQualifiedTargetClass(algorithm);
            _builder.append(_qualifiedTargetClass);
            _builder.append("().apply(registry);");
            _builder.newLineIfNotEmpty();
          }
        }
      }
    };
    return _client;
  }

  /**
   * Generates code for the given algorithm's metadata provider class.
   * 
   * @param model
   *            the model the algorithm was declared in. Used for the file header.
   * @param algorithm
   *            the algorithm to create one or more {@link JvmDeclaredType declared types} from.
   * @param acceptor
   *            each created {@link JvmDeclaredType type} without a container should be passed to the acceptor
   *            in order to get attached to the current resource. The acceptor's
   *            {@link IJvmDeclaredTypeAcceptor#accept(org.eclipse.xtext.common.types.JvmDeclaredType) accept(..)}
   *            method takes the constructed empty type for the pre-indexing phase. This one is further
   *            initialized in the indexing phase using passed closure.
   * @param isPreIndexingPhase
   *            whether the method is called in a pre-indexing phase, i.e.
   *            when the global index is not yet fully updated. You must not
   *            rely on linking using the index if isPreIndexingPhase is
   *            <code>true</code>.
   */
  private void inferAlgorithm(final MdModel model, final MdAlgorithm algorithm, final IJvmDeclaredTypeAcceptor acceptor, final boolean isPreIndexingPhase) {
    String _name = algorithm.getName();
    boolean _tripleEquals = (_name == null);
    if (_tripleEquals) {
      return;
    }
    final Procedure1<JvmGenericType> _function = new Procedure1<JvmGenericType>() {
      @Override
      public void apply(final JvmGenericType it) {
        EList<JvmTypeReference> _superTypes = it.getSuperTypes();
        JvmTypeReference _typeRef = MetaDataJvmModelInferrer.this._typeReferenceBuilder.typeRef("org.eclipse.elk.core.data.ILayoutMetaDataProvider");
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmTypeReference>operator_add(_superTypes, _typeRef);
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setFileHeader(it, MetaDataJvmModelInferrer.this._jvmTypesBuilder.getDocumentation(model));
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setDocumentation(it, algorithm.getDocumentation());
        EList<JvmMember> _members = it.getMembers();
        JvmField _algorithmId = MetaDataJvmModelInferrer.this.toAlgorithmId(algorithm);
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmField>operator_add(_members, _algorithmId);
        EList<MdOptionSupport> _supportedOptions = algorithm.getSupportedOptions();
        for (final MdOptionSupport support : _supportedOptions) {
          {
            XExpression _value = support.getValue();
            boolean _tripleNotEquals = (_value != null);
            if (_tripleNotEquals) {
              EList<JvmMember> _members_1 = it.getMembers();
              JvmField _supportedOptionDefault = MetaDataJvmModelInferrer.this.toSupportedOptionDefault(support);
              MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmField>operator_add(_members_1, _supportedOptionDefault);
            }
            EList<JvmMember> _members_2 = it.getMembers();
            JvmField _supportedOptionConstant = MetaDataJvmModelInferrer.this.toSupportedOptionConstant(support);
            MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmField>operator_add(_members_2, _supportedOptionConstant);
          }
        }
        EList<JvmMember> _members_1 = it.getMembers();
        final Procedure1<JvmGenericType> _function = new Procedure1<JvmGenericType>() {
          @Override
          public void apply(final JvmGenericType it_1) {
            EList<JvmTypeReference> _superTypes = it_1.getSuperTypes();
            JvmTypeReference _typeRef = MetaDataJvmModelInferrer.this._typeReferenceBuilder.typeRef("org.eclipse.elk.core.util.IFactory<AbstractLayoutProvider>");
            MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmTypeReference>operator_add(_superTypes, _typeRef);
            it_1.setStatic(true);
            StringConcatenation _builder = new StringConcatenation();
            _builder.append("Layouter-specific algorithm factory.");
            MetaDataJvmModelInferrer.this._jvmTypesBuilder.setDocumentation(it_1, _builder.toString());
            EList<JvmMember> _members = it_1.getMembers();
            final Procedure1<JvmOperation> _function = new Procedure1<JvmOperation>() {
              @Override
              public void apply(final JvmOperation it_2) {
                it_2.setVisibility(JvmVisibility.PUBLIC);
                StringConcatenationClient _client = new StringConcatenationClient() {
                  @Override
                  protected void appendTo(StringConcatenationClient.TargetStringConcatenation _builder) {
                    _builder.append("AbstractLayoutProvider provider = new ");
                    JvmTypeReference _provider = algorithm.getProvider();
                    _builder.append(_provider);
                    _builder.append("();");
                    _builder.newLineIfNotEmpty();
                    _builder.append("provider.initialize(\"");
                    String _parameter = algorithm.getParameter();
                    _builder.append(_parameter);
                    _builder.append("\");");
                    _builder.newLineIfNotEmpty();
                    _builder.append("return provider;");
                    _builder.newLine();
                  }
                };
                MetaDataJvmModelInferrer.this._jvmTypesBuilder.setBody(it_2, _client);
              }
            };
            JvmOperation _method = MetaDataJvmModelInferrer.this._jvmTypesBuilder.toMethod(algorithm, "create", MetaDataJvmModelInferrer.this._typeReferenceBuilder.typeRef("org.eclipse.elk.core.AbstractLayoutProvider"), _function);
            MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmOperation>operator_add(_members, _method);
            EList<JvmMember> _members_1 = it_1.getMembers();
            final Procedure1<JvmOperation> _function_1 = new Procedure1<JvmOperation>() {
              @Override
              public void apply(final JvmOperation it_2) {
                EList<JvmFormalParameter> _parameters = it_2.getParameters();
                JvmFormalParameter _parameter = MetaDataJvmModelInferrer.this._jvmTypesBuilder.toParameter(algorithm, "obj", MetaDataJvmModelInferrer.this._typeReferenceBuilder.typeRef("org.eclipse.elk.core.AbstractLayoutProvider"));
                MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmFormalParameter>operator_add(_parameters, _parameter);
                StringConcatenationClient _client = new StringConcatenationClient() {
                  @Override
                  protected void appendTo(StringConcatenationClient.TargetStringConcatenation _builder) {
                    _builder.append("obj.dispose();");
                    _builder.newLine();
                  }
                };
                MetaDataJvmModelInferrer.this._jvmTypesBuilder.setBody(it_2, _client);
              }
            };
            JvmOperation _method_1 = MetaDataJvmModelInferrer.this._jvmTypesBuilder.toMethod(algorithm, "destroy", MetaDataJvmModelInferrer.this._typeReferenceBuilder.typeRef(void.class), _function_1);
            MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmOperation>operator_add(_members_1, _method_1);
          }
        };
        JvmGenericType _class = MetaDataJvmModelInferrer.this._jvmTypesBuilder.toClass(algorithm, MetaDataJvmModelInferrer.this.toFactoryClass(algorithm), _function);
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmGenericType>operator_add(_members_1, _class);
        EList<JvmMember> _members_2 = it.getMembers();
        final Procedure1<JvmOperation> _function_1 = new Procedure1<JvmOperation>() {
          @Override
          public void apply(final JvmOperation it_1) {
            EList<JvmFormalParameter> _parameters = it_1.getParameters();
            JvmFormalParameter _parameter = MetaDataJvmModelInferrer.this._jvmTypesBuilder.toParameter(algorithm, "registry", MetaDataJvmModelInferrer.this._typeReferenceBuilder.typeRef("org.eclipse.elk.core.data.ILayoutMetaDataProvider.Registry"));
            MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmFormalParameter>operator_add(_parameters, _parameter);
            StringConcatenationClient _client = new StringConcatenationClient() {
              @Override
              protected void appendTo(StringConcatenationClient.TargetStringConcatenation _builder) {
                StringConcatenationClient _registerLayoutAlgorithm = MetaDataJvmModelInferrer.this.registerLayoutAlgorithm(algorithm);
                _builder.append(_registerLayoutAlgorithm);
                _builder.newLineIfNotEmpty();
              }
            };
            MetaDataJvmModelInferrer.this._jvmTypesBuilder.setBody(it_1, _client);
          }
        };
        JvmOperation _method = MetaDataJvmModelInferrer.this._jvmTypesBuilder.toMethod(algorithm, "apply", MetaDataJvmModelInferrer.this._typeReferenceBuilder.typeRef(void.class), _function_1);
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmOperation>operator_add(_members_2, _method);
      }
    };
    acceptor.<JvmGenericType>accept(this._jvmTypesBuilder.toClass(algorithm, this.getQualifiedTargetClass(algorithm)), _function);
  }

  private String toFactoryClass(final MdAlgorithm algorithm) {
    String _firstUpper = StringExtensions.toFirstUpper(algorithm.getName());
    return (_firstUpper + "Factory");
  }

  private JvmField toAlgorithmId(final MdAlgorithm algorithm) {
    final Procedure1<JvmField> _function = new Procedure1<JvmField>() {
      @Override
      public void apply(final JvmField it) {
        it.setVisibility(JvmVisibility.PUBLIC);
        it.setStatic(true);
        it.setFinal(true);
        StringConcatenationClient _client = new StringConcatenationClient() {
          @Override
          protected void appendTo(StringConcatenationClient.TargetStringConcatenation _builder) {
            String _codeString = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.getQualifiedName(algorithm));
            _builder.append(_codeString);
          }
        };
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setInitializer(it, _client);
        StringConcatenation _builder = new StringConcatenation();
        _builder.append("The id of the ");
        String _label = algorithm.getLabel();
        _builder.append(_label);
        _builder.append(" algorithm.");
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setDocumentation(it, _builder.toString());
      }
    };
    return this._jvmTypesBuilder.toField(algorithm, "ALGORITHM_ID", this._typeReferenceBuilder.typeRef(String.class), _function);
  }

  private JvmField toSupportedOptionDefault(final MdOptionSupport support) {
    EObject _eContainer = support.eContainer();
    final MdAlgorithm algorithm = ((MdAlgorithm) _eContainer);
    JvmTypeReference _elvis = null;
    JvmTypeReference _cloneWithProxies = this._jvmTypesBuilder.cloneWithProxies(support.getOption().getType());
    if (_cloneWithProxies != null) {
      _elvis = _cloneWithProxies;
    } else {
      JvmTypeReference _typeRef = this._typeReferenceBuilder.typeRef(Object.class);
      _elvis = _typeRef;
    }
    final JvmTypeReference optionType = _elvis;
    final Procedure1<JvmField> _function = new Procedure1<JvmField>() {
      @Override
      public void apply(final JvmField it) {
        it.setVisibility(JvmVisibility.PRIVATE);
        it.setStatic(true);
        it.setFinal(true);
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setInitializer(it, support.getValue());
        StringConcatenation _builder = new StringConcatenation();
        _builder.append("Default value for {@link #");
        String _constantName = MetaDataJvmModelInferrer.this.getConstantName(support.getOption());
        _builder.append(_constantName);
        _builder.append("} with algorithm \"");
        String _elvis = null;
        String _label = algorithm.getLabel();
        if (_label != null) {
          _elvis = _label;
        } else {
          String _name = algorithm.getName();
          _elvis = _name;
        }
        _builder.append(_elvis);
        _builder.append("\".");
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setDocumentation(it, _builder.toString());
      }
    };
    return this._jvmTypesBuilder.toField(support, this.getDefaultConstantName(support.getOption()), optionType, _function);
  }

  private JvmField toSupportedOptionConstant(final MdOptionSupport support) {
    JvmTypeReference _elvis = null;
    JvmTypeReference _type = support.getOption().getType();
    if (_type != null) {
      _elvis = _type;
    } else {
      JvmTypeReference _typeRef = this._typeReferenceBuilder.typeRef(Object.class);
      _elvis = _typeRef;
    }
    final Procedure1<JvmField> _function = new Procedure1<JvmField>() {
      @Override
      public void apply(final JvmField it) {
        it.setVisibility(JvmVisibility.PUBLIC);
        it.setStatic(true);
        it.setFinal(true);
        StringConcatenationClient _xifexpression = null;
        XExpression _value = support.getValue();
        boolean _tripleEquals = (_value == null);
        if (_tripleEquals) {
          StringConcatenationClient _client = new StringConcatenationClient() {
            @Override
            protected void appendTo(StringConcatenationClient.TargetStringConcatenation _builder) {
              JvmTypeReference _typeRef = MetaDataJvmModelInferrer.this._typeReferenceBuilder.typeRef(MetaDataJvmModelInferrer.this.getQualifiedTargetClass(MetaDataJvmModelInferrer.this.getBundle(support.getOption())));
              _builder.append(_typeRef);
              _builder.append(".");
              String _constantName = MetaDataJvmModelInferrer.this.getConstantName(support.getOption());
              _builder.append(_constantName);
            }
          };
          _xifexpression = _client;
        } else {
          StringConcatenationClient _client_1 = new StringConcatenationClient() {
            @Override
            protected void appendTo(StringConcatenationClient.TargetStringConcatenation _builder) {
              _builder.append("new ");
              _builder.append(Property.class);
              _builder.append("<");
              JvmTypeReference _elvis = null;
              JvmTypeReference _asWrapperTypeIfPrimitive = MetaDataJvmModelInferrer.this._primitives.asWrapperTypeIfPrimitive(support.getOption().getType());
              if (_asWrapperTypeIfPrimitive != null) {
                _elvis = _asWrapperTypeIfPrimitive;
              } else {
                JvmTypeReference _typeRef = MetaDataJvmModelInferrer.this._typeReferenceBuilder.typeRef(Object.class);
                _elvis = _typeRef;
              }
              _builder.append(_elvis);
              _builder.append(">(");
              _builder.newLineIfNotEmpty();
              _builder.append("                            ");
              JvmTypeReference _typeRef_1 = MetaDataJvmModelInferrer.this._typeReferenceBuilder.typeRef(MetaDataJvmModelInferrer.this.getQualifiedTargetClass(MetaDataJvmModelInferrer.this.getBundle(support.getOption())));
              _builder.append(_typeRef_1, "                            ");
              _builder.append(".");
              String _constantName = MetaDataJvmModelInferrer.this.getConstantName(support.getOption());
              _builder.append(_constantName, "                            ");
              _builder.append(",");
              _builder.newLineIfNotEmpty();
              _builder.append("                            ");
              String _defaultConstantName = MetaDataJvmModelInferrer.this.getDefaultConstantName(support.getOption());
              _builder.append(_defaultConstantName, "                            ");
              _builder.append(")");
            }
          };
          _xifexpression = _client_1;
        }
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setInitializer(it, _xifexpression);
        boolean _isDeprecated = support.getOption().isDeprecated();
        if (_isDeprecated) {
          EList<JvmAnnotationReference> _annotations = it.getAnnotations();
          JvmAnnotationReference _annotationRef = MetaDataJvmModelInferrer.this._annotationTypesBuilder.annotationRef(Deprecated.class);
          MetaDataJvmModelInferrer.this._jvmTypesBuilder.<JvmAnnotationReference>operator_add(_annotations, _annotationRef);
          it.setDeprecated(true);
        }
        String _trimLines = MetaDataJvmModelInferrer.this.trimLines(support.getOption().getDescription());
        String _xifexpression_1 = null;
        boolean _isNullOrEmpty = StringExtensions.isNullOrEmpty(support.getDocumentation());
        boolean _not = (!_isNullOrEmpty);
        if (_not) {
          String _trimLines_1 = MetaDataJvmModelInferrer.this.trimLines(support.getDocumentation());
          _xifexpression_1 = ("\n<h3>Algorithm Specific Details</h3>\n" + _trimLines_1);
        } else {
          _xifexpression_1 = "";
        }
        String _plus = (_trimLines + _xifexpression_1);
        MetaDataJvmModelInferrer.this._jvmTypesBuilder.setDocumentation(it, _plus);
      }
    };
    return this._jvmTypesBuilder.toField(support, 
      this.getConstantName(support.getOption()), 
      this._typeReferenceBuilder.typeRef(IProperty.class, _elvis), _function);
  }

  private StringConcatenationClient registerLayoutAlgorithm(final MdAlgorithm algorithm) {
    StringConcatenationClient _client = new StringConcatenationClient() {
      @Override
      protected void appendTo(StringConcatenationClient.TargetStringConcatenation _builder) {
        _builder.append("registry.register(new ");
        JvmTypeReference _LayoutAlgorithmData = MetaDataJvmModelInferrer.this.LayoutAlgorithmData();
        _builder.append(_LayoutAlgorithmData);
        _builder.append(".Builder()");
        _builder.newLineIfNotEmpty();
        _builder.append("    ");
        _builder.append(".id(");
        String _codeString = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.getQualifiedName(algorithm));
        _builder.append(_codeString, "    ");
        _builder.append(")");
        _builder.newLineIfNotEmpty();
        _builder.append("    ");
        _builder.append(".name(");
        String _elvis = null;
        String _label = algorithm.getLabel();
        if (_label != null) {
          _elvis = _label;
        } else {
          String _name = algorithm.getName();
          _elvis = _name;
        }
        String _codeString_1 = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.shrinkWhiteSpace(_elvis));
        _builder.append(_codeString_1, "    ");
        _builder.append(")");
        _builder.newLineIfNotEmpty();
        _builder.append("    ");
        _builder.append(".description(");
        String _codeString_2 = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.shrinkWhiteSpace(algorithm.getDescription()));
        _builder.append(_codeString_2, "    ");
        _builder.append(")");
        _builder.newLineIfNotEmpty();
        _builder.append("    ");
        _builder.append(".providerFactory(new ");
        String _factoryClass = MetaDataJvmModelInferrer.this.toFactoryClass(algorithm);
        _builder.append(_factoryClass, "    ");
        _builder.append("())");
        _builder.newLineIfNotEmpty();
        {
          MdCategory _category = algorithm.getCategory();
          boolean _tripleNotEquals = (_category != null);
          if (_tripleNotEquals) {
            _builder.append("    ");
            _builder.append(".category(");
            String _codeString_3 = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.getQualifiedName(algorithm.getCategory()));
            _builder.append(_codeString_3, "    ");
            _builder.append(")");
            _builder.newLineIfNotEmpty();
          }
        }
        {
          MdBundle _bundle = MetaDataJvmModelInferrer.this.getBundle(algorithm);
          boolean _tripleNotEquals_1 = (_bundle != null);
          if (_tripleNotEquals_1) {
            _builder.append("    ");
            _builder.append(".melkBundleName(");
            String _codeString_4 = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.getBundle(algorithm).getLabel());
            _builder.append(_codeString_4, "    ");
            _builder.append(")");
            _builder.newLineIfNotEmpty();
          }
        }
        {
          MdModel _model = MetaDataJvmModelInferrer.this.getModel(algorithm);
          String _definingBundleId = null;
          if (_model!=null) {
            _definingBundleId=MetaDataJvmModelInferrer.this.getDefiningBundleId(_model);
          }
          boolean _tripleNotEquals_2 = (_definingBundleId != null);
          if (_tripleNotEquals_2) {
            _builder.append("    ");
            _builder.append(".definingBundleId(");
            String _codeString_5 = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.getDefiningBundleId(MetaDataJvmModelInferrer.this.getModel(algorithm)));
            _builder.append(_codeString_5, "    ");
            _builder.append(")");
            _builder.newLineIfNotEmpty();
          }
        }
        {
          String _previewImage = algorithm.getPreviewImage();
          boolean _tripleNotEquals_3 = (_previewImage != null);
          if (_tripleNotEquals_3) {
            _builder.append("    ");
            _builder.append(".imagePath(");
            String _codeString_6 = MetaDataJvmModelInferrer.this.toCodeString(algorithm.getPreviewImage());
            _builder.append(_codeString_6, "    ");
            _builder.append(")");
            _builder.newLineIfNotEmpty();
          }
        }
        {
          boolean _isEmpty = algorithm.getSupportedFeatures().isEmpty();
          boolean _not = (!_isEmpty);
          if (_not) {
            _builder.append("    ");
            _builder.append(".supportedFeatures(");
            _builder.append(EnumSet.class, "    ");
            _builder.append(".of(");
            {
              EList<MdGraphFeature> _supportedFeatures = algorithm.getSupportedFeatures();
              boolean _hasElements = false;
              for(final MdGraphFeature f : _supportedFeatures) {
                if (!_hasElements) {
                  _hasElements = true;
                } else {
                  _builder.appendImmediate(", ", "    ");
                }
                JvmTypeReference _typeRef = MetaDataJvmModelInferrer.this._typeReferenceBuilder.typeRef("org.eclipse.elk.graph.properties.GraphFeature");
                _builder.append(_typeRef, "    ");
                _builder.append(".");
                String _upperCase = f.toString().toUpperCase();
                _builder.append(_upperCase, "    ");
              }
            }
            _builder.append("))");
            _builder.newLineIfNotEmpty();
          }
        }
        {
          JvmTypeReference _validator = algorithm.getValidator();
          boolean _tripleNotEquals_4 = (_validator != null);
          if (_tripleNotEquals_4) {
            _builder.append("    ");
            _builder.append(".validatorClass(");
            JvmTypeReference _validator_1 = algorithm.getValidator();
            _builder.append(_validator_1, "    ");
            _builder.append(".class)");
            _builder.newLineIfNotEmpty();
          }
        }
        _builder.append("    ");
        _builder.append(".create()");
        _builder.newLine();
        _builder.append(");");
        _builder.newLine();
        {
          EList<MdOptionSupport> _supportedOptions = algorithm.getSupportedOptions();
          for(final MdOptionSupport support : _supportedOptions) {
            _builder.append("registry.addOptionSupport(");
            _builder.newLine();
            _builder.append("    ");
            String _codeString_7 = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.getQualifiedName(algorithm));
            _builder.append(_codeString_7, "    ");
            _builder.append(",");
            _builder.newLineIfNotEmpty();
            _builder.append("    ");
            String _codeString_8 = MetaDataJvmModelInferrer.this.toCodeString(MetaDataJvmModelInferrer.this.getQualifiedName(support.getOption()));
            _builder.append(_codeString_8, "    ");
            _builder.append(",");
            _builder.newLineIfNotEmpty();
            {
              XExpression _value = support.getValue();
              boolean _tripleEquals = (_value == null);
              if (_tripleEquals) {
                _builder.append("    ");
                String _constantName = MetaDataJvmModelInferrer.this.getConstantName(support.getOption());
                _builder.append(_constantName, "    ");
                _builder.append(".getDefault()");
                _builder.newLineIfNotEmpty();
              } else {
                _builder.append("    ");
                String _defaultConstantName = MetaDataJvmModelInferrer.this.getDefaultConstantName(support.getOption());
                _builder.append(_defaultConstantName, "    ");
                _builder.newLineIfNotEmpty();
              }
            }
            _builder.append(");");
            _builder.newLine();
          }
        }
      }
    };
    return _client;
  }

  private JvmTypeReference LayoutAlgorithmData() {
    return this._typeReferenceBuilder.typeRef("org.eclipse.elk.core.data.LayoutAlgorithmData");
  }

  private Iterable<MdOption> getAllOptionDefinitions(final Iterable<? extends MdBundleMember> elements) {
    final Function1<MdGroup, Iterable<MdOption>> _function = new Function1<MdGroup, Iterable<MdOption>>() {
      @Override
      public Iterable<MdOption> apply(final MdGroup it) {
        return MetaDataJvmModelInferrer.this.getAllOptionDefinitions(it.getChildren());
      }
    };
    return Iterables.<MdOption>concat(
      Iterables.<MdOption>filter(elements, MdOption.class), 
      Iterables.<MdOption>concat(IterableExtensions.<MdGroup, Iterable<MdOption>>map(Iterables.<MdGroup>filter(elements, MdGroup.class), _function)));
  }

  private String getQualifiedTargetClass(final MdBundle bundle) {
    EObject _eContainer = bundle.eContainer();
    final MdModel model = ((MdModel) _eContainer);
    String _elvis = null;
    String _targetClass = bundle.getTargetClass();
    if (_targetClass != null) {
      _elvis = _targetClass;
    } else {
      _elvis = "Metadata";
    }
    final String bundleClass = _elvis;
    String _name = model.getName();
    String _plus = (_name + ".");
    return (_plus + bundleClass);
  }

  private String getQualifiedTargetClass(final MdAlgorithm algorithm) {
    EObject _eContainer = this.getBundle(algorithm).eContainer();
    final MdModel model = ((MdModel) _eContainer);
    String _elvis = null;
    String _targetClass = algorithm.getTargetClass();
    if (_targetClass != null) {
      _elvis = _targetClass;
    } else {
      String _firstUpper = StringExtensions.toFirstUpper(algorithm.getName());
      String _plus = (_firstUpper + "Metadata");
      _elvis = _plus;
    }
    final String algorithmClass = _elvis;
    String _name = model.getName();
    String _plus_1 = (_name + ".");
    return (_plus_1 + algorithmClass);
  }

  private Iterable<MdGroup> getGroups(final MdBundleMember member) {
    LinkedList<MdGroup> _xblockexpression = null;
    {
      final LinkedList<MdGroup> groups = new LinkedList<MdGroup>();
      EObject group = member.eContainer();
      while ((group instanceof MdGroup)) {
        {
          groups.addFirst(((MdGroup)group));
          group = ((MdGroup)group).eContainer();
        }
      }
      _xblockexpression = groups;
    }
    return _xblockexpression;
  }

  private String getOptionType(final MdOption option) {
    JvmTypeReference _type = option.getType();
    JvmType _type_1 = null;
    if (_type!=null) {
      _type_1=_type.getType();
    }
    final JvmType jvmType = _type_1;
    boolean _matched = false;
    if (jvmType instanceof JvmPrimitiveType) {
      _matched=true;
      String _identifier = ((JvmPrimitiveType)jvmType).getIdentifier();
      boolean _matched_1 = false;
      String _name = boolean.class.getName();
      if (Objects.equal(_identifier, _name)) {
        _matched_1=true;
        return "BOOLEAN";
      }
      if (!_matched_1) {
        String _name_1 = int.class.getName();
        if (Objects.equal(_identifier, _name_1)) {
          _matched_1=true;
          return "INT";
        }
      }
      if (!_matched_1) {
        String _name_2 = double.class.getName();
        if (Objects.equal(_identifier, _name_2)) {
          _matched_1=true;
          return "DOUBLE";
        }
      }
      if (!_matched_1) {
        String _name_3 = float.class.getName();
        if (Objects.equal(_identifier, _name_3)) {
          _matched_1=true;
          return "DOUBLE";
        }
      }
    }
    if (!_matched) {
      if (jvmType instanceof JvmGenericType) {
        _matched=true;
        String _identifier = ((JvmGenericType)jvmType).getIdentifier();
        boolean _matched_1 = false;
        String _canonicalName = Boolean.class.getCanonicalName();
        if (Objects.equal(_identifier, _canonicalName)) {
          _matched_1=true;
          return "BOOLEAN";
        }
        if (!_matched_1) {
          String _canonicalName_1 = Integer.class.getCanonicalName();
          if (Objects.equal(_identifier, _canonicalName_1)) {
            _matched_1=true;
            return "INT";
          }
        }
        if (!_matched_1) {
          String _canonicalName_2 = Double.class.getCanonicalName();
          if (Objects.equal(_identifier, _canonicalName_2)) {
            _matched_1=true;
            return "DOUBLE";
          }
        }
        if (!_matched_1) {
          String _canonicalName_3 = Float.class.getCanonicalName();
          if (Objects.equal(_identifier, _canonicalName_3)) {
            _matched_1=true;
            return "DOUBLE";
          }
        }
        if (!_matched_1) {
          String _canonicalName_4 = String.class.getCanonicalName();
          if (Objects.equal(_identifier, _canonicalName_4)) {
            _matched_1=true;
            return "STRING";
          }
        }
        if (!_matched_1) {
          String _canonicalName_5 = EnumSet.class.getCanonicalName();
          if (Objects.equal(_identifier, _canonicalName_5)) {
            _matched_1=true;
            return "ENUMSET";
          }
        }
        return "OBJECT";
      }
    }
    if (!_matched) {
      if (jvmType instanceof JvmEnumerationType) {
        _matched=true;
        return "ENUM";
      }
    }
    return "UNDEFINED";
  }

  private JvmTypeReference getOptionTypeClass(final MdOption property) {
    JvmTypeReference _xifexpression = null;
    JvmTypeReference _type = property.getType();
    boolean _tripleNotEquals = (_type != null);
    if (_tripleNotEquals) {
      JvmTypeReference _xblockexpression = null;
      {
        JvmType _type_1 = property.getType().getType();
        if ((_type_1 instanceof JvmPrimitiveType)) {
          JvmType _type_2 = property.getType().getType();
          final JvmPrimitiveType primitiveType = ((JvmPrimitiveType) _type_2);
          String _simpleName = primitiveType.getSimpleName();
          boolean _equals = Objects.equal(_simpleName, "float");
          if (_equals) {
            return this._typeReferenceBuilder.typeRef(Double.class);
          } else {
            String _simpleName_1 = primitiveType.getSimpleName();
            boolean _equals_1 = Objects.equal(_simpleName_1, "long");
            if (_equals_1) {
              return this._typeReferenceBuilder.typeRef(Integer.class);
            }
          }
        } else {
          JvmType _type_3 = property.getType().getType();
          if ((_type_3 instanceof JvmGenericType)) {
            JvmType _type_4 = property.getType().getType();
            final JvmGenericType genericType = ((JvmGenericType) _type_4);
            String _identifier = genericType.getIdentifier();
            boolean _equals_2 = Objects.equal(_identifier, "java.lang.Float");
            if (_equals_2) {
              return this._typeReferenceBuilder.typeRef(Double.class);
            } else {
              String _identifier_1 = genericType.getIdentifier();
              boolean _equals_3 = Objects.equal(_identifier_1, "java.lang.Long");
              if (_equals_3) {
                return this._typeReferenceBuilder.typeRef(Integer.class);
              } else {
                String _identifier_2 = genericType.getIdentifier();
                boolean _equals_4 = Objects.equal(_identifier_2, "java.util.EnumSet");
                if (_equals_4) {
                  JvmTypeReference _type_5 = property.getType();
                  final JvmParameterizedTypeReference outer = ((JvmParameterizedTypeReference) _type_5);
                  return this._jvmTypesBuilder.cloneWithProxies(IterableExtensions.<JvmTypeReference>head(outer.getArguments()));
                } else {
                  String _identifier_3 = genericType.getIdentifier();
                  boolean _equals_5 = Objects.equal(_identifier_3, "java.util.List");
                  if (_equals_5) {
                    return this._typeReferenceBuilder.typeRef(List.class);
                  }
                }
              }
            }
          }
        }
        _xblockexpression = this._primitives.asWrapperTypeIfPrimitive(this._jvmTypesBuilder.cloneWithProxies(property.getType()));
      }
      _xifexpression = _xblockexpression;
    } else {
      return this._typeReferenceBuilder.typeRef(Void.class);
    }
    return _xifexpression;
  }

  private MdBundle getBundle(final MdBundleMember member) {
    EObject parent = member.eContainer();
    while ((!(parent instanceof MdBundle))) {
      parent = parent.eContainer();
    }
    return ((MdBundle) parent);
  }

  private MdModel getModel(final MdBundleMember member) {
    EObject parent = member.eContainer();
    while ((!(parent instanceof MdModel))) {
      parent = parent.eContainer();
    }
    return ((MdModel) parent);
  }

  private String getDefiningBundleId(final MdModel model) {
    final URI uri = model.eResource().getURI();
    final int srcIndex = ((List<String>)Conversions.doWrapArray(uri.segments())).indexOf("src");
    if ((srcIndex >= 1)) {
      return uri.segments()[(srcIndex - 1)];
    }
    return null;
  }

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
    String _name_1 = member.getName();
    int _lastIndexOf = prefix.lastIndexOf(".");
    int _plus = (_lastIndexOf + 1);
    String _substring = prefix.substring(_plus, prefix.length());
    boolean _equals = Objects.equal(_name_1, _substring);
    if (_equals) {
      prefix = prefix.substring(0, prefix.lastIndexOf("."));
    }
    String _xifexpression = null;
    boolean _isEmpty = IterableExtensions.isEmpty(this.getGroups(member));
    if (_isEmpty) {
      _xifexpression = "";
    } else {
      _xifexpression = ".";
    }
    String _plus_1 = (prefix + _xifexpression);
    final Function1<MdGroup, String> _function = new Function1<MdGroup, String>() {
      @Override
      public String apply(final MdGroup it) {
        return it.getName();
      }
    };
    String _join = IterableExtensions.join(IterableExtensions.<MdGroup, String>map(this.getGroups(member), _function), ".");
    String _plus_2 = (_plus_1 + _join);
    String _plus_3 = (_plus_2 + ".");
    String _name_2 = member.getName();
    return (_plus_3 + _name_2);
  }

  private String getConstantName(final MdBundleMember member) {
    final String name = member.getName();
    if ((name != null)) {
      final StringBuilder result = new StringBuilder();
      final Function1<MdGroup, String> _function = new Function1<MdGroup, String>() {
        @Override
        public String apply(final MdGroup it) {
          return MetaDataJvmModelInferrer.this.toUpperCaseWithUnderscores(it.getName());
        }
      };
      result.append(IterableExtensions.join(IterableExtensions.<MdGroup, String>map(this.getGroups(member), _function), "_"));
      int _length = result.length();
      boolean _greaterThan = (_length > 0);
      if (_greaterThan) {
        result.append("_");
      }
      result.append(this.toUpperCaseWithUnderscores(name));
      return result.toString();
    }
    return null;
  }

  private String toUpperCaseWithUnderscores(final String str) {
    final StringBuilder result = new StringBuilder();
    for (int i = 0; (i < str.length()); i++) {
      {
        final char c = str.charAt(i);
        if ((Character.isUpperCase(c) && (i > 0))) {
          result.append("_");
        }
        result.append(Character.toUpperCase(c));
      }
    }
    return result.toString();
  }

  private String getDefaultConstantName(final MdOption option) {
    String _constantName = this.getConstantName(option);
    return (_constantName + "_DEFAULT");
  }

  private String getLowerBoundConstantName(final MdOption option) {
    String _constantName = this.getConstantName(option);
    return (_constantName + "_LOWER_BOUND");
  }

  private String getUpperBoundConstantName(final MdOption option) {
    String _constantName = this.getConstantName(option);
    return (_constantName + "_UPPER_BOUND");
  }

  private String getDependencyConstantName(final MdOptionDependency dependency) {
    String _xblockexpression = null;
    {
      EObject _eContainer = dependency.eContainer();
      final MdOption option = ((MdOption) _eContainer);
      String _constantName = this.getConstantName(option);
      String _plus = (_constantName + "_DEP_");
      String _constantName_1 = this.getConstantName(dependency.getTarget());
      String _plus_1 = (_plus + _constantName_1);
      String _plus_2 = (_plus_1 + "_");
      int _indexOf = option.getDependencies().indexOf(dependency);
      _xblockexpression = (_plus_2 + Integer.valueOf(_indexOf));
    }
    return _xblockexpression;
  }

  private String toCodeString(final String s) {
    if ((s == null)) {
      return "null";
    } else {
      StringConcatenation _builder = new StringConcatenation();
      _builder.append("\"");
      String _convertToJavaString = Strings.convertToJavaString(s);
      _builder.append(_convertToJavaString);
      _builder.append("\"");
      return _builder.toString();
    }
  }

  private String shrinkWhiteSpace(final String s) {
    if ((s == null)) {
      return null;
    }
    final StringBuilder result = new StringBuilder();
    boolean shrink = true;
    for (int i = 0; (i < s.length()); i++) {
      {
        final char c = s.charAt(i);
        boolean _isWhitespace = Character.isWhitespace(c);
        if (_isWhitespace) {
          if ((!shrink)) {
            result.append(" ");
          }
          shrink = true;
        } else {
          result.append(c);
          shrink = false;
        }
      }
    }
    if (((result.length() > 0) && Character.isWhitespace(result.charAt((result.length() - 1))))) {
      int _length = result.length();
      int _minus = (_length - 1);
      result.deleteCharAt(_minus);
    }
    return result.toString();
  }

  private String trimLines(final String s) {
    String _xifexpression = null;
    if ((s == null)) {
      _xifexpression = null;
    } else {
      final Function1<String, String> _function = new Function1<String, String>() {
        @Override
        public String apply(final String it) {
          return it.trim();
        }
      };
      _xifexpression = IterableExtensions.join(ListExtensions.<String, String>map(((List<String>)Conversions.doWrapArray(s.split("\r?\n"))), _function), "\n");
    }
    return _xifexpression;
  }

  public void infer(final EObject model, final IJvmDeclaredTypeAcceptor acceptor, final boolean isPreIndexingPhase) {
    if (model instanceof MdModel) {
      _infer((MdModel)model, acceptor, isPreIndexingPhase);
      return;
    } else if (model != null) {
      _infer(model, acceptor, isPreIndexingPhase);
      return;
    } else {
      throw new IllegalArgumentException("Unhandled parameter types: " +
        Arrays.<Object>asList(model, acceptor, isPreIndexingPhase).toString());
    }
  }
}
