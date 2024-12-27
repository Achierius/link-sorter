#include "web_view.hpp"

#include <cstdio>
#include <cstring>

#include <iostream>

#include <cairo/cairo.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>

namespace quicksorter {

WebView::WebView(const char *site, size_t width, size_t height)
    : site_(site), view_width_(width), view_height_(height) {
  app_ = gtk_application_new("com.quicksorter.WebView",
                             G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app_, "activate", G_CALLBACK(onActivate), this);

  int status = g_application_run(G_APPLICATION(app_), 0, nullptr);
  (void)status;

  g_object_unref(app_);
}

WebView::~WebView() {}

void WebView::onSnapshotComplete(GObject *object, GAsyncResult *res,
                                 gpointer user_data) {
  return (*(WebView *)user_data).onSnapshotCompleteImpl(object, res);
}

void WebView::onSnapshotCompleteImpl(GObject *object, GAsyncResult *res) {
  // Better hope the caller gave us a WebKitWebView!
  WebKitWebView *webview = (WebKitWebView *)object;
  GError *error = nullptr;

  // Get the GdkTexture from the snapshot
  GdkTexture *texture =
      webkit_web_view_get_snapshot_finish(webview, res, &error);
  if (error) {
    g_printerr("Error capturing snapshot: %s\n", error->message);
    g_error_free(error);
    return;
  }

  // Get the texture's width and height
  image_width_ = gdk_texture_get_width(texture);
  image_height_ = gdk_texture_get_height(texture);

  // Download pixel data from the texture
  guchar *data = g_new(guchar, image_width_ * image_height_ * 4); // RGBA
  gdk_texture_download(texture, data, image_width_ * 4);
#if 0
  // Create a Cairo surface
  cairo_surface_t *surface = cairo_image_surface_create_for_data(
      data, CAIRO_FORMAT_ARGB32, image_width_, image_height_, image_width_ * 4);

  // Save the surface to a PNG file
  const char *filename = "output.png";
  cairo_surface_write_to_png(surface, filename);
  g_print("Snapshot saved to %s\n", filename);

  // Clean up
  cairo_surface_destroy(surface);
  g_object_unref(texture);
  g_free(data);

  // Quit the application
  GtkApplication *app = GTK_APPLICATION(app_);
  g_application_quit(G_APPLICATION(app));
#else

  // Create a Cairo surface
  cairo_surface_t *surface = cairo_image_surface_create_for_data(
      (unsigned char *)data, CAIRO_FORMAT_ARGB32, image_width_, image_height_,
      image_width_ * 4);

  // Save the surface to a PNG file
  // const char *filename = "output.png";
  // cairo_surface_write_to_png(surface, filename);
  cairo_surface_write_to_png_stream(surface, acceptWriteAsPngStream, this);

  g_object_unref(texture);
  g_free(data);

  // Quit the application
  GtkApplication *app = GTK_APPLICATION(app_);
  g_application_quit(G_APPLICATION(app));

  // Clean up
  cairo_surface_destroy(surface);
#endif
}

void WebView::onLoadChanged(WebKitWebView *webview, WebKitLoadEvent load_event,
                            gpointer user_data) {
  return (*(WebView *)user_data).onLoadChangedImpl(webview, load_event);
}

void WebView::onLoadChangedImpl(WebKitWebView *webview,
                                WebKitLoadEvent load_event) {
  if (load_event == WEBKIT_LOAD_FINISHED) {

    // gtk_widget_set_visible(window_, false);  // makes it not show up
    g_print("Page loaded. Capturing snapshot...\n");
    webkit_web_view_get_snapshot(webview, WEBKIT_SNAPSHOT_REGION_FULL_DOCUMENT,
                                 WEBKIT_SNAPSHOT_OPTIONS_NONE, nullptr,
                                 onSnapshotComplete, this);
  }
}

void WebView::onActivate(GtkApplication *app, gpointer user_data) {
  return (*(WebView *)user_data).onActivateImpl(app);
}

void WebView::onActivateImpl(GtkApplication *app) {
  window_ = gtk_application_window_new(app);

  gtk_window_set_default_size(GTK_WINDOW(window_), view_width_, view_height_);

  WebKitWebView *webview = WEBKIT_WEB_VIEW(webkit_web_view_new());

  gtk_window_set_child(GTK_WINDOW(window_), GTK_WIDGET(webview));
  g_signal_connect(webview, "load-changed", G_CALLBACK(onLoadChanged), this);

  webkit_web_view_load_uri(webview, site_);
  gtk_widget_set_visible(window_, true);
  g_idle_add(
      [](gpointer data) -> gboolean {
        gtk_widget_set_visible(GTK_WIDGET(data), FALSE);
        return G_SOURCE_REMOVE; // Ensure this runs only once
      },
      window_);
}

cairo_status_t WebView::acceptWriteAsPngStream(void *closure,
                                               const unsigned char *data,
                                               unsigned int length) {
  return (*(WebView *)closure).acceptWriteAsPngStreamImpl(data, length);
}

cairo_status_t WebView::acceptWriteAsPngStreamImpl(const unsigned char *data,
                                                   unsigned int length) {
  // TODO make this thread safe
  auto old_size = data_.size();
  for (int i = 0; i < length; i++) {
    data_.push_back(0);
  }
  std::memcpy(data_.data() + old_size, data, length);
  return cairo_status_t::CAIRO_STATUS_SUCCESS;
}

} // namespace quicksorter
