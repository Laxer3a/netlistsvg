/**
 * Copyright (c) 2017, 2019 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 *     Miro Spönemann - converted to Xtend
 */
package org.eclipse.elk.graph.json.text.ui;

import com.google.common.base.Objects;
import com.google.common.collect.Iterables;
import com.google.common.collect.Iterators;
import com.google.inject.Inject;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.function.Consumer;
import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.elk.core.data.LayoutAlgorithmData;
import org.eclipse.elk.core.data.LayoutMetaDataService;
import org.eclipse.elk.core.data.LayoutOptionData;
import org.eclipse.elk.core.options.CoreOptions;
import org.eclipse.elk.core.service.util.ProgressMonitorAdapter;
import org.eclipse.elk.core.util.IElkProgressMonitor;
import org.eclipse.elk.core.validation.GraphIssue;
import org.eclipse.elk.core.validation.LayoutOptionValidator;
import org.eclipse.elk.graph.EMapPropertyHolder;
import org.eclipse.elk.graph.ElkEdge;
import org.eclipse.elk.graph.ElkEdgeSection;
import org.eclipse.elk.graph.ElkGraphElement;
import org.eclipse.elk.graph.ElkLabel;
import org.eclipse.elk.graph.ElkNode;
import org.eclipse.elk.graph.ElkPort;
import org.eclipse.elk.graph.impl.ElkPropertyToValueMapEntryImpl;
import org.eclipse.elk.graph.properties.IProperty;
import org.eclipse.elk.graph.properties.IPropertyValueProxy;
import org.eclipse.elk.graph.util.GraphIdentifierGenerator;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.impl.ResourceSetImpl;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.handlers.HandlerUtil;
import org.eclipse.ui.statushandlers.StatusManager;
import org.eclipse.xtend2.lib.StringConcatenation;
import org.eclipse.xtext.EcoreUtil2;
import org.eclipse.xtext.xbase.lib.CollectionLiterals;
import org.eclipse.xtext.xbase.lib.Exceptions;
import org.eclipse.xtext.xbase.lib.Functions.Function1;
import org.eclipse.xtext.xbase.lib.IterableExtensions;
import org.eclipse.xtext.xbase.lib.IteratorExtensions;
import org.eclipse.xtext.xbase.lib.Procedures.Procedure1;
import org.eclipse.xtext.xbase.lib.StringExtensions;

/**
 * Copied from {@link org.eclipse.elk.graph.text.ui.ConvertGraphHandler}.
 * 
 * A command handler that can be used to convert a graph from the textual format to the XMI format
 * and back. The default implementation only copies input models and makes certain modifications if
 * the target format is the textual format.
 * 
 * <p>Note that plug-ins containing subclasses of this handler must define their own convert graphs
 * command. Only then can they declare menu contributions that reference their new command, which
 * in turn defines their new subclass as command handler.</p>
 */
@SuppressWarnings("all")
public class ConvertGraphHandler extends AbstractHandler {
  /**
   * Parameter id for the target file extension.
   */
  private static final String PARAM_TARGET_EXT = "org.eclipse.elk.graph.json.text.convert.targetExtension";

  /**
   * Textual graph file extension.
   */
  private static final String EXT_ELK_TEXT = "elkj";

  /**
   * XMI graph file extension.
   */
  private static final String EXT_ELK_XMI = "elkg";

  /**
   * The target file extension.
   */
  private String targetExtension;

  @Inject
  private LayoutOptionValidator layoutOptionValidator;

  @Override
  public final Object execute(final ExecutionEvent event) throws ExecutionException {
    this.targetExtension = event.getParameter(ConvertGraphHandler.PARAM_TARGET_EXT);
    boolean _isValidTargetExtension = this.isValidTargetExtension(this.targetExtension);
    boolean _not = (!_isValidTargetExtension);
    if (_not) {
      StringConcatenation _builder = new StringConcatenation();
      _builder.append("Invalid target extension: ");
      _builder.append(this.targetExtension);
      final Status status = new Status(IStatus.ERROR, 
        ElkGraphJsonUiModule.PLUGIN_ID, _builder.toString());
      StatusManager.getManager().handle(status, (StatusManager.SHOW + StatusManager.LOG));
      return null;
    }
    final ISelection selection = HandlerUtil.getCurrentSelection(event);
    if ((selection instanceof IStructuredSelection)) {
      final Iterable<IFile> files = Iterables.<IFile>filter(((IStructuredSelection)selection).toList(), IFile.class);
      final Job job = new Job("Convert models") {
        @Override
        protected IStatus run(final IProgressMonitor monitor) {
          final ProgressMonitorAdapter elkMonitor = new ProgressMonitorAdapter(monitor);
          elkMonitor.begin("Convert models", IterableExtensions.size(files));
          final Consumer<IFile> _function = new Consumer<IFile>() {
            @Override
            public void accept(final IFile file) {
              boolean _isCanceled = elkMonitor.isCanceled();
              boolean _not = (!_isCanceled);
              if (_not) {
                ConvertGraphHandler.this.convert(file, elkMonitor.subTask(1));
              }
            }
          };
          files.forEach(_function);
          elkMonitor.done();
          return Status.OK_STATUS;
        }
      };
      job.setUser(true);
      job.schedule();
    }
    return null;
  }

  private boolean isValidTargetExtension(final String targetExtension) {
    return (Objects.equal(targetExtension, ConvertGraphHandler.EXT_ELK_TEXT) || Objects.equal(targetExtension, ConvertGraphHandler.EXT_ELK_XMI));
  }

  /**
   * Process a source file.
   */
  public void convert(final IFile file, final IElkProgressMonitor monitor) {
    StringConcatenation _builder = new StringConcatenation();
    _builder.append("Convert ");
    String _name = file.getName();
    _builder.append(_name);
    monitor.begin(_builder.toString(), 3);
    try {
      final EList<EObject> sourceModels = this.readModel(URI.createPlatformResourceURI(file.getFullPath().toString(), false));
      monitor.worked(1);
      final Function1<EObject, EObject> _function = new Function1<EObject, EObject>() {
        @Override
        public EObject apply(final EObject it) {
          return ConvertGraphHandler.this.transform(it);
        }
      };
      final List<EObject> targetModels = IterableExtensions.<EObject>toList(IterableExtensions.<EObject, EObject>map(((Iterable<EObject>) sourceModels), _function));
      monitor.worked(1);
      this.saveModel(targetModels, this.createTarget(file));
    } catch (final Throwable _t) {
      if (_t instanceof Exception) {
        final Exception exception = (Exception)_t;
        final Status status = new Status(IStatus.ERROR, ElkGraphJsonUiModule.PLUGIN_ID, 
          "Error while converting the selected graph.", exception);
        StatusManager.getManager().handle(status, (StatusManager.SHOW + StatusManager.LOG));
      } else {
        throw Exceptions.sneakyThrow(_t);
      }
    } finally {
      monitor.done();
    }
  }

  /**
   * Transform the graph before it is written to the new file format.
   */
  protected EObject transform(final EObject model) {
    final EObject copy = EcoreUtil.<EObject>copy(model);
    if ((this.targetExtension.equals(ConvertGraphHandler.EXT_ELK_TEXT) && (copy instanceof ElkNode))) {
      final ElkNode graph = ((ElkNode) copy);
      GraphIdentifierGenerator.forGraph(graph).assertValid().assertExists().assertUnique().execute();
    }
    return copy;
  }

  /**
   * Create a target file URI from a source file.
   */
  private URI createTarget(final IFile file) {
    final IPath basePath = file.getFullPath();
    final String name = basePath.removeFileExtension().lastSegment();
    StringConcatenation _builder = new StringConcatenation();
    _builder.append(name);
    _builder.append(".");
    _builder.append(this.targetExtension);
    IPath targetPath = new Path(_builder.toString());
    int i = 0;
    final IContainer parent = file.getParent();
    while (parent.exists(targetPath)) {
      StringConcatenation _builder_1 = new StringConcatenation();
      _builder_1.append(name);
      int _plusPlus = i++;
      _builder_1.append(_plusPlus);
      _builder_1.append(".");
      _builder_1.append(this.targetExtension);
      Path _path = new Path(_builder_1.toString());
      targetPath = _path;
    }
    targetPath = parent.getFullPath().append(targetPath);
    return URI.createPlatformResourceURI(targetPath.toString(), false);
  }

  /**
   * Read the model contents from the given URI.
   */
  private EList<EObject> readModel(final URI uri) {
    final ResourceSetImpl resourceSet = new ResourceSetImpl();
    final Resource resource = resourceSet.getResource(uri, true);
    return resource.getContents();
  }

  /**
   * Save a collection of models to the given URI.
   */
  private void saveModel(final List<EObject> models, final URI uri) throws IOException {
    final ResourceSetImpl resourceSet = new ResourceSetImpl();
    final Resource resource = resourceSet.createResource(uri);
    resource.getContents().addAll(models);
    resource.save(Collections.EMPTY_MAP);
  }

  protected void removeInvalidProperties(final ElkNode graph) {
    final ArrayList<ElkPropertyToValueMapEntryImpl> toRemove = CollectionLiterals.<ElkPropertyToValueMapEntryImpl>newArrayList();
    final Procedure1<EMapPropertyHolder> _function = new Procedure1<EMapPropertyHolder>() {
      @Override
      public void apply(final EMapPropertyHolder element) {
        final Consumer<Map.Entry<IProperty<?>, Object>> _function = new Consumer<Map.Entry<IProperty<?>, Object>>() {
          @Override
          public void accept(final Map.Entry<IProperty<?>, Object> entry) {
            boolean _isValid = ConvertGraphHandler.this.isValid(entry.getKey(), entry.getValue(), element);
            boolean _not = (!_isValid);
            if (_not) {
              toRemove.add(((ElkPropertyToValueMapEntryImpl) entry));
            }
          }
        };
        element.getProperties().forEach(_function);
      }
    };
    IteratorExtensions.<EMapPropertyHolder>forEach(Iterators.<EMapPropertyHolder>filter(graph.eAllContents(), EMapPropertyHolder.class), _function);
    final Consumer<ElkPropertyToValueMapEntryImpl> _function_1 = new Consumer<ElkPropertyToValueMapEntryImpl>() {
      @Override
      public void accept(final ElkPropertyToValueMapEntryImpl it) {
        EcoreUtil.remove(it);
      }
    };
    toRemove.forEach(_function_1);
  }

  private static final List<? extends IProperty<?>> EXCLUDED_OPTIONS = Collections.<IProperty<? extends Object>>unmodifiableList(CollectionLiterals.<IProperty<? extends Object>>newArrayList(CoreOptions.POSITION, CoreOptions.BEND_POINTS, CoreOptions.JUNCTION_POINTS));

  private boolean isValid(final IProperty<?> property, final Object value, final EMapPropertyHolder container) {
    LayoutOptionData _xifexpression = null;
    if ((property instanceof LayoutOptionData)) {
      _xifexpression = ((LayoutOptionData)property);
    } else {
      _xifexpression = LayoutMetaDataService.getInstance().getOptionData(property.getId());
    }
    final LayoutOptionData option = _xifexpression;
    if ((option == null)) {
      return false;
    }
    boolean _contains = ConvertGraphHandler.EXCLUDED_OPTIONS.contains(option);
    if (_contains) {
      return false;
    }
    Object _xifexpression_1 = null;
    if ((value instanceof String)) {
      _xifexpression_1 = option.parseValue(((String)value));
    } else {
      Object _xifexpression_2 = null;
      if ((value instanceof IPropertyValueProxy)) {
        _xifexpression_2 = ((IPropertyValueProxy)value).<Object>resolveValue(option);
      } else {
        _xifexpression_2 = value;
      }
      _xifexpression_1 = _xifexpression_2;
    }
    final Object parsedValue = _xifexpression_1;
    if ((parsedValue == null)) {
      return false;
    }
    if ((container instanceof ElkGraphElement)) {
      boolean _matched = false;
      if (container instanceof ElkNode) {
        _matched=true;
        boolean _not = (!(option.getTargets().contains(LayoutOptionData.Target.NODES) || ((!((ElkNode)container).getChildren().isEmpty()) && option.getTargets().contains(LayoutOptionData.Target.PARENTS))));
        if (_not) {
          return false;
        }
      }
      if (!_matched) {
        if (container instanceof ElkEdge) {
          _matched=true;
          boolean _contains_1 = option.getTargets().contains(LayoutOptionData.Target.EDGES);
          boolean _not = (!_contains_1);
          if (_not) {
            return false;
          }
        }
      }
      if (!_matched) {
        if (container instanceof ElkPort) {
          _matched=true;
          boolean _contains_1 = option.getTargets().contains(LayoutOptionData.Target.PORTS);
          boolean _not = (!_contains_1);
          if (_not) {
            return false;
          }
        }
      }
      if (!_matched) {
        if (container instanceof ElkLabel) {
          _matched=true;
          boolean _contains_1 = option.getTargets().contains(LayoutOptionData.Target.LABELS);
          boolean _not = (!_contains_1);
          if (_not) {
            return false;
          }
        }
      }
      if (((container instanceof ElkNode) && option.getTargets().contains(LayoutOptionData.Target.NODES))) {
        boolean _isSupportedByAlgorithm = this.isSupportedByAlgorithm(option, ((ElkNode) container).getParent());
        boolean _not = (!_isSupportedByAlgorithm);
        if (_not) {
          return false;
        }
      } else {
        boolean _isSupportedByAlgorithm_1 = this.isSupportedByAlgorithm(option, ((ElkGraphElement)container));
        boolean _not_1 = (!_isSupportedByAlgorithm_1);
        if (_not_1) {
          return false;
        }
      }
      final List<GraphIssue> issues = this.layoutOptionValidator.checkProperty(option, parsedValue, ((ElkGraphElement)container));
      boolean _isEmpty = issues.isEmpty();
      boolean _not_2 = (!_isEmpty);
      if (_not_2) {
        return false;
      }
      return true;
    } else {
      return false;
    }
  }

  private boolean isSupportedByAlgorithm(final LayoutOptionData option, final ElkGraphElement element) {
    boolean _equals = Objects.equal(option, CoreOptions.ALGORITHM);
    if (_equals) {
      return true;
    }
    LayoutAlgorithmData algorithm = ConvertGraphHandler.getAlgorithm(element);
    if (((algorithm != null) && algorithm.knowsOption(option))) {
      return true;
    }
    ElkNode parent = EcoreUtil2.<ElkNode>getContainerOfType(element, ElkNode.class);
    while ((parent != null)) {
      {
        algorithm = ConvertGraphHandler.getAlgorithm(parent);
        if (((algorithm != null) && algorithm.knowsOption(option))) {
          return true;
        }
        parent = parent.getParent();
      }
    }
    return false;
  }

  protected void removeUnnecessaryLayouts(final ElkNode graph) {
    final ArrayList<ElkEdgeSection> toRemove = CollectionLiterals.<ElkEdgeSection>newArrayList();
    final Procedure1<ElkEdge> _function = new Procedure1<ElkEdge>() {
      @Override
      public void apply(final ElkEdge edge) {
        EList<ElkEdgeSection> _sections = edge.getSections();
        Iterables.<ElkEdgeSection>addAll(toRemove, _sections);
      }
    };
    IteratorExtensions.<ElkEdge>forEach(Iterators.<ElkEdge>filter(graph.eAllContents(), ElkEdge.class), _function);
    final Consumer<ElkEdgeSection> _function_1 = new Consumer<ElkEdgeSection>() {
      @Override
      public void accept(final ElkEdgeSection it) {
        EcoreUtil.remove(it);
      }
    };
    toRemove.forEach(_function_1);
  }

  /**
   * Return the algorithm that is responsible for the layout of the given element. Note that this
   * might be ambiguous: e.g. a port of a composite node can be handled both by the algorithm that
   * arranges that node and the algorithm that arranges its container node.
   */
  public static LayoutAlgorithmData getAlgorithm(final ElkGraphElement element) {
    ElkNode _switchResult = null;
    boolean _matched = false;
    if (element instanceof ElkNode) {
      _matched=true;
      _switchResult = ((ElkNode)element);
    }
    if (!_matched) {
      if (element instanceof ElkPort) {
        _matched=true;
        ElkNode _elvis = null;
        ElkNode _parent = ((ElkPort)element).getParent();
        ElkNode _parent_1 = null;
        if (_parent!=null) {
          _parent_1=_parent.getParent();
        }
        if (_parent_1 != null) {
          _elvis = _parent_1;
        } else {
          ElkNode _parent_2 = ((ElkPort)element).getParent();
          _elvis = _parent_2;
        }
        _switchResult = _elvis;
      }
    }
    if (!_matched) {
      if (element instanceof ElkEdge) {
        _matched=true;
        _switchResult = ((ElkEdge)element).getContainingNode();
      }
    }
    if (!_matched) {
      if (element instanceof ElkLabel) {
        _matched=true;
        ElkNode _xblockexpression = null;
        {
          ElkGraphElement parent = ((ElkLabel)element).getParent();
          while ((parent instanceof ElkLabel)) {
            parent = ((ElkLabel)parent).getParent();
          }
          ElkNode _switchResult_1 = null;
          boolean _matched_1 = false;
          if (parent instanceof ElkNode) {
            _matched_1=true;
            ElkNode _elvis = null;
            ElkNode _parent = ((ElkNode)parent).getParent();
            if (_parent != null) {
              _elvis = _parent;
            } else {
              _elvis = ((ElkNode)parent);
            }
            _switchResult_1 = _elvis;
          }
          if (!_matched_1) {
            if (parent instanceof ElkPort) {
              _matched_1=true;
              ElkNode _elvis = null;
              ElkNode _parent = ((ElkPort)parent).getParent();
              ElkNode _parent_1 = null;
              if (_parent!=null) {
                _parent_1=_parent.getParent();
              }
              if (_parent_1 != null) {
                _elvis = _parent_1;
              } else {
                ElkNode _parent_2 = ((ElkPort)parent).getParent();
                _elvis = _parent_2;
              }
              _switchResult_1 = _elvis;
            }
          }
          if (!_matched_1) {
            if (parent instanceof ElkEdge) {
              _matched_1=true;
              _switchResult_1 = ((ElkEdge)parent).getContainingNode();
            }
          }
          _xblockexpression = _switchResult_1;
        }
        _switchResult = _xblockexpression;
      }
    }
    final ElkNode node = _switchResult;
    if ((node != null)) {
      final String algorithmId = node.<String>getProperty(CoreOptions.ALGORITHM);
      boolean _isNullOrEmpty = StringExtensions.isNullOrEmpty(algorithmId);
      boolean _not = (!_isNullOrEmpty);
      if (_not) {
        return LayoutMetaDataService.getInstance().getAlgorithmDataBySuffix(algorithmId);
      }
    }
    return null;
  }
}
