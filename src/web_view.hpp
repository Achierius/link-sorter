#pragma once

#include <iostream>
#include <vector>

#include <cairo/cairo.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>

namespace quicksorter {

class WebView {
public:
  WebView(const char *url, size_t width, size_t height);
  ~WebView();

  char *data() { return data_.data(); }

  size_t data_size() const { return data_.size(); }

private:
  static void onSnapshotComplete(GObject *object, GAsyncResult *res,
                                 gpointer raw_this);
  void onSnapshotCompleteImpl(GObject *object, GAsyncResult *res);
  static void onLoadChanged(WebKitWebView *webview, WebKitLoadEvent load_event,
                            gpointer raw_this);
  void onLoadChangedImpl(WebKitWebView *webview, WebKitLoadEvent load_event);
  static void onActivate(GtkApplication *app, gpointer raw_this);
  void onActivateImpl(GtkApplication *app);

  static cairo_status_t acceptWriteAsPngStream(void *closure,
                                               const unsigned char *data,
                                               unsigned int length);
  cairo_status_t acceptWriteAsPngStreamImpl(const unsigned char *data,
                                            unsigned int length);

  const char *site_;
  std::vector<char> data_;

  GtkApplication *app_;
  GtkWidget *window_;

  size_t view_width_;
  size_t view_height_;

  int image_width_{0};
  int image_height_{0};
};

} // namespace quicksorter
