#pragma once

#include <cairo/cairo.h>
#include <gtk/gtk.h>
#include <iostream>
#include <webkit/webkit.h>

namespace quicksorter {

class WebView {
public:
  WebView(char *url);

private:
  static void onSnapshotComplete(GObject *object, GAsyncResult *res,
                                 gpointer raw_this);
  void onSnapshotCompleteImpl(GObject *object, GAsyncResult *res);
  static void onLoadChanged(WebKitWebView *webview, WebKitLoadEvent load_event,
                            gpointer raw_this);
  void onLoadChangedImpl(WebKitWebView *webview, WebKitLoadEvent load_event);
  static void onActivate(GtkApplication *app, gpointer raw_this);
  void onActivateImpl(GtkApplication *app);

  GtkApplication *app_;
  char *site_;
};

} // namespace quicksorter
