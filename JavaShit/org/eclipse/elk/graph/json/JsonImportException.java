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

/**
 * Exception indicating that something is wrong with
 * a passed json.
 */
@SuppressWarnings("all")
public class JsonImportException extends RuntimeException {
  public JsonImportException() {
    super();
  }

  public JsonImportException(final String message) {
    super(message);
  }

  public JsonImportException(final Throwable throwable) {
    super(throwable);
  }

  public JsonImportException(final String message, final Throwable throwable) {
    super(message, throwable);
  }
}
