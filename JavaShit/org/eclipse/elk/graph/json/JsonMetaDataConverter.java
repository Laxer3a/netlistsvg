/**
 * Copyright (c) 2017, 2019 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.json;

import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import java.util.Arrays;
import java.util.function.Consumer;
import org.eclipse.elk.core.data.ILayoutMetaData;
import org.eclipse.elk.core.data.LayoutAlgorithmData;
import org.eclipse.elk.core.data.LayoutCategoryData;
import org.eclipse.elk.core.data.LayoutOptionData;
import org.eclipse.elk.graph.properties.GraphFeature;
import org.eclipse.xtext.xbase.lib.Extension;
import org.eclipse.xtext.xbase.lib.IterableExtensions;

/**
 * Utility class, allows to convert {@link ILayoutMetaData}s retrieved from the {@link ILayoutMetaDataService}
 * to a json representation.
 */
@SuppressWarnings("all")
public final class JsonMetaDataConverter {
  @Extension
  private static JsonAdapter _jsonAdapter = new JsonAdapter();

  private static JsonObject createCommon(final ILayoutMetaData data) {
    final JsonObject jsonObj = JsonMetaDataConverter._jsonAdapter.newJsonObject();
    String _id = data.getId();
    boolean _tripleNotEquals = (_id != null);
    if (_tripleNotEquals) {
      JsonMetaDataConverter._jsonAdapter.addJsonObj(jsonObj, "id", data.getId());
    }
    String _name = data.getName();
    boolean _tripleNotEquals_1 = (_name != null);
    if (_tripleNotEquals_1) {
      JsonMetaDataConverter._jsonAdapter.addJsonObj(jsonObj, "name", data.getName());
    }
    String _description = data.getDescription();
    boolean _tripleNotEquals_2 = (_description != null);
    if (_tripleNotEquals_2) {
      JsonMetaDataConverter._jsonAdapter.addJsonObj(jsonObj, "description", data.getDescription());
    }
    return jsonObj;
  }

  protected static JsonObject _toJson(final LayoutAlgorithmData lad) {
    final JsonObject jsonObj = JsonMetaDataConverter.createCommon(lad);
    String _categoryId = lad.getCategoryId();
    boolean _tripleNotEquals = (_categoryId != null);
    if (_tripleNotEquals) {
      JsonMetaDataConverter._jsonAdapter.addJsonObj(jsonObj, "category", lad.getCategoryId());
    }
    boolean _isNullOrEmpty = IterableExtensions.isNullOrEmpty(lad.getKnownOptionIds());
    boolean _not = (!_isNullOrEmpty);
    if (_not) {
      final JsonArray jsonArr = JsonMetaDataConverter._jsonAdapter.newJsonArray();
      JsonMetaDataConverter._jsonAdapter.addJsonObj(jsonObj, "knownOptions", jsonArr);
      final Consumer<String> _function = new Consumer<String>() {
        @Override
        public void accept(final String o) {
          JsonMetaDataConverter._jsonAdapter.addJsonArr(jsonArr, JsonMetaDataConverter._jsonAdapter.toJson(o));
        }
      };
      lad.getKnownOptionIds().forEach(_function);
    }
    boolean _isNullOrEmpty_1 = IterableExtensions.isNullOrEmpty(lad.getSupportedFeatures());
    boolean _not_1 = (!_isNullOrEmpty_1);
    if (_not_1) {
      final JsonArray jsonArr_1 = JsonMetaDataConverter._jsonAdapter.newJsonArray();
      JsonMetaDataConverter._jsonAdapter.addJsonObj(jsonObj, "supportedFeatures", jsonArr_1);
      final Consumer<GraphFeature> _function_1 = new Consumer<GraphFeature>() {
        @Override
        public void accept(final GraphFeature f) {
          JsonMetaDataConverter._jsonAdapter.addJsonArr(jsonArr_1, JsonMetaDataConverter._jsonAdapter.toJson(f.toString()));
        }
      };
      lad.getSupportedFeatures().forEach(_function_1);
    }
    return jsonObj;
  }

  protected static JsonObject _toJson(final LayoutCategoryData lcd) {
    final JsonObject jsonObj = JsonMetaDataConverter.createCommon(lcd);
    boolean _isNullOrEmpty = IterableExtensions.isNullOrEmpty(lcd.getLayouters());
    boolean _not = (!_isNullOrEmpty);
    if (_not) {
      final JsonArray jsonArr = JsonMetaDataConverter._jsonAdapter.newJsonArray();
      JsonMetaDataConverter._jsonAdapter.addJsonObj(jsonObj, "knownLayouters", jsonArr);
      final Consumer<LayoutAlgorithmData> _function = new Consumer<LayoutAlgorithmData>() {
        @Override
        public void accept(final LayoutAlgorithmData l) {
          String _id = l.getId();
          boolean _tripleNotEquals = (_id != null);
          if (_tripleNotEquals) {
            JsonMetaDataConverter._jsonAdapter.addJsonArr(jsonArr, JsonMetaDataConverter._jsonAdapter.toJson(l.getId()));
          }
        }
      };
      lcd.getLayouters().forEach(_function);
    }
    return jsonObj;
  }

  protected static JsonObject _toJson(final LayoutOptionData lod) {
    final JsonObject jsonObj = JsonMetaDataConverter.createCommon(lod);
    String _group = lod.getGroup();
    boolean _tripleNotEquals = (_group != null);
    if (_tripleNotEquals) {
      JsonMetaDataConverter._jsonAdapter.addJsonObj(jsonObj, "group", lod.getGroup());
    }
    LayoutOptionData.Type _type = lod.getType();
    boolean _tripleNotEquals_1 = (_type != null);
    if (_tripleNotEquals_1) {
      JsonMetaDataConverter._jsonAdapter.addJsonObj(jsonObj, "type", lod.getType().toString());
    }
    boolean _isNullOrEmpty = IterableExtensions.isNullOrEmpty(lod.getTargets());
    boolean _not = (!_isNullOrEmpty);
    if (_not) {
      final JsonArray jsonArr = JsonMetaDataConverter._jsonAdapter.newJsonArray();
      JsonMetaDataConverter._jsonAdapter.addJsonObj(jsonObj, "targets", jsonArr);
      final Consumer<LayoutOptionData.Target> _function = new Consumer<LayoutOptionData.Target>() {
        @Override
        public void accept(final LayoutOptionData.Target t) {
          JsonMetaDataConverter._jsonAdapter.addJsonArr(jsonArr, JsonMetaDataConverter._jsonAdapter.toJson(t.toString()));
        }
      };
      lod.getTargets().forEach(_function);
    }
    return jsonObj;
  }

  public static JsonObject toJson(final ILayoutMetaData lad) {
    if (lad instanceof LayoutAlgorithmData) {
      return _toJson((LayoutAlgorithmData)lad);
    } else if (lad instanceof LayoutCategoryData) {
      return _toJson((LayoutCategoryData)lad);
    } else if (lad instanceof LayoutOptionData) {
      return _toJson((LayoutOptionData)lad);
    } else {
      throw new IllegalArgumentException("Unhandled parameter types: " +
        Arrays.<Object>asList(lad).toString());
    }
  }
}
