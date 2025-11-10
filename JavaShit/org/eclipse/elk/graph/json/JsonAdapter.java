/**
 * Copyright (c) 2017 Kiel University and others.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 * 
 * SPDX-License-Identifier: EPL-2.0
 */
package org.eclipse.elk.graph.json;

import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonPrimitive;
import java.util.Map;
import org.eclipse.xtext.xbase.lib.Functions.Function1;
import org.eclipse.xtext.xbase.lib.IterableExtensions;

/**
 * The library dependent part of the {@link JsonImporter}
 * using Google's gson library.
 * 
 * <b>IMPORTANT</b>
 * Whenever methods are added or removed from this class,
 * elkjs's corresponding JsonAdapter must be update accordingly!
 */
@SuppressWarnings("all")
public final class JsonAdapter {
  public Object getId(final JsonObject o) {
    Object _xblockexpression = null;
    {
      boolean _has = o.has("id");
      boolean _not = (!_has);
      if (_not) {
        throw this.formatError("Every element must have an id.");
      }
      _xblockexpression = this.asId(o.get("id"));
    }
    return _xblockexpression;
  }

  public Object asId(final JsonElement id) {
    boolean _matched = false;
    if (id instanceof JsonPrimitive) {
      boolean _isString = ((JsonPrimitive)id).isString();
      if (_isString) {
        _matched=true;
        return ((JsonPrimitive)id).getAsString();
      }
    }
    if (!_matched) {
      if (id instanceof JsonPrimitive) {
        if ((((JsonPrimitive)id).isNumber() && this.isInt(((JsonPrimitive)id).getAsDouble()))) {
          _matched=true;
          return Integer.valueOf(Double.valueOf(((JsonPrimitive)id).getAsDouble()).intValue());
        }
      }
    }
    throw this.formatError((("Id must be a string or an integer: \'" + id) + "\'."));
  }

  public String getIdSave(final JsonObject o) {
    return this.optString(o, "id");
  }

  public Error implementationError() {
    return new Error("Severe implementation error in the Json to ElkGraph importer.");
  }

  public JsonImportException formatError(final String msg) {
    return new JsonImportException(msg);
  }

  public JsonObject toJsonObject(final Object o) {
    return ((JsonObject) o);
  }

  public JsonArray toJsonArray(final Object o) {
    return ((JsonArray) o);
  }

  /**
   * If {@value e} is a primitve type (string, number, boolean)
   * it is guaranteed to be converted to a String
   * (in the gson implementation of 'asString').
   */
  public String stringVal(final JsonElement e) {
    return e.getAsString();
  }

  public String optString(final JsonObject o, final String element) {
    JsonElement _get = o.get(element);
    String _stringVal = null;
    if (_get!=null) {
      _stringVal=this.stringVal(_get);
    }
    return _stringVal;
  }

  public String optString(final JsonArray arr, final int i) {
    JsonElement _get = arr.get(i);
    String _stringVal = null;
    if (_get!=null) {
      _stringVal=this.stringVal(_get);
    }
    return _stringVal;
  }

  public String optString(final Object o, final String element) {
    JsonObject _jsonObject = null;
    if (o!=null) {
      _jsonObject=this.toJsonObject(o);
    }
    return this.optString(_jsonObject, element);
  }

  public Double optDouble(final JsonObject o, final String element) {
    Double _xifexpression = null;
    boolean _has = o.has(element);
    if (_has) {
      _xifexpression = Double.valueOf(o.get(element).getAsDouble());
    } else {
      _xifexpression = null;
    }
    return _xifexpression;
  }

  public JsonArray optJSONArray(final JsonObject o, final String element) {
    JsonElement _get = o.get(element);
    JsonArray _asJsonArray = null;
    if (_get!=null) {
      _asJsonArray=_get.getAsJsonArray();
    }
    return _asJsonArray;
  }

  public JsonObject optJSONObject(final JsonObject o, final String element) {
    JsonElement _get = o.get(element);
    JsonObject _asJsonObject = null;
    if (_get!=null) {
      _asJsonObject=_get.getAsJsonObject();
    }
    return _asJsonObject;
  }

  public JsonObject optJSONObject(final JsonArray arr, final int i) {
    JsonElement _get = arr.get(i);
    JsonObject _asJsonObject = null;
    if (_get!=null) {
      _asJsonObject=_get.getAsJsonObject();
    }
    return _asJsonObject;
  }

  public int sizeJsonArr(final JsonArray o) {
    return o.size();
  }

  public boolean hasJsonObj(final JsonObject o, final String element) {
    return o.has(element);
  }

  public Iterable<String> keysJsonObj(final JsonObject o) {
    final Function1<Map.Entry<String, JsonElement>, String> _function = new Function1<Map.Entry<String, JsonElement>, String>() {
      @Override
      public String apply(final Map.Entry<String, JsonElement> e) {
        return e.getKey();
      }
    };
    return IterableExtensions.<Map.Entry<String, JsonElement>, String>map(o.entrySet(), _function);
  }

  public JsonElement getJsonObj(final JsonObject o, final String element) {
    return o.get(element);
  }

  public JsonElement getJsonArr(final JsonArray arr, final int i) {
    return arr.get(i);
  }

  public JsonObject newJsonObject() {
    return new JsonObject();
  }

  public JsonArray newJsonArray() {
    return new JsonArray();
  }

  public JsonPrimitive toJson(final String s) {
    return new JsonPrimitive(s);
  }

  public JsonPrimitive toJson(final Number n) {
    return new JsonPrimitive(n);
  }

  public void addJsonObj(final JsonObject o, final String element, final Number n) {
    o.addProperty(element, n);
  }

  public void addJsonObj(final JsonObject o, final String element, final boolean b) {
    o.addProperty(element, Boolean.valueOf(b));
  }

  public void addJsonObj(final JsonObject o, final String element, final String s) {
    o.addProperty(element, s);
  }

  public void addJsonObj(final JsonObject o, final String element, final JsonElement je) {
    o.add(element, je);
  }

  public void addJsonObj(final JsonObject o, final String element, final Object obj) {
    boolean _matched = false;
    if (obj instanceof String) {
      _matched=true;
      this.addJsonObj(o, element, ((String)obj));
    }
    if (!_matched) {
      if (obj instanceof Boolean) {
        _matched=true;
        this.addJsonObj(o, element, obj);
      }
    }
    if (!_matched) {
      if (obj instanceof Number) {
        _matched=true;
        this.addJsonObj(o, element, ((Number)obj));
      }
    }
    if (!_matched) {
      throw this.implementationError();
    }
  }

  public void addJsonArr(final JsonArray arr, final JsonElement je) {
    arr.add(je);
  }

  public void addJsonArr(final JsonArray arr, final Object o) {
    boolean _matched = false;
    if (o instanceof String) {
      _matched=true;
      this.addJsonArr(arr, this.toJson(((String)o)));
    }
    if (!_matched) {
      if (o instanceof Number) {
        _matched=true;
        this.addJsonArr(arr, this.toJson(((Number)o)));
      }
    }
    if (!_matched) {
      throw this.implementationError();
    }
  }

  public boolean isInt(final double d) {
    return ((d % 1) == 0);
  }
}
