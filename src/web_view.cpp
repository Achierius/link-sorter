#include "web_view.hpp"

#include <cairo/cairo.h>
#include <gtk/gtk.h>
#include <iostream>
#include <webkit/webkit.h>

namespace quicksorter {

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
  int width = gdk_texture_get_width(texture);
  int height = gdk_texture_get_height(texture);

  // Download pixel data from the texture
  guchar *data = g_new(guchar, width * height * 4); // RGBA
  gdk_texture_download(texture, data, width * 4);

  // Create a Cairo surface
  cairo_surface_t *surface = cairo_image_surface_create_for_data(
      data, CAIRO_FORMAT_ARGB32, width, height, width * 4);

  // Save the surface to a PNG file
  const char *filename = "output.png";
  cairo_surface_write_to_png(surface, filename);
  g_print("Snapshot saved to %s\n", filename);

  // Clean up
  cairo_surface_destroy(surface);
  g_object_unref(texture);
  g_free(data);

  // Quit the application
  GtkApplication *app = GTK_APPLICATION(this);
  g_application_quit(G_APPLICATION(app));
}

void WebView::onLoadChanged(WebKitWebView *webview, WebKitLoadEvent load_event,
                            gpointer user_data) {
  return (*(WebView *)user_data).onLoadChangedImpl(webview, load_event);
}

void WebView::onLoadChangedImpl(WebKitWebView *webview,
                                WebKitLoadEvent load_event) {
  if (load_event == WEBKIT_LOAD_FINISHED) {
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
  GtkWidget *window = gtk_application_window_new(app);
  WebKitWebView *webview = WEBKIT_WEB_VIEW(webkit_web_view_new());

  gtk_window_set_child(GTK_WINDOW(window), GTK_WIDGET(webview));
  g_signal_connect(webview, "load-changed", G_CALLBACK(onLoadChanged), app);

  webkit_web_view_load_uri(webview, site_);
  gtk_widget_set_visible(window, true);
}

WebView::WebView(char *site) : site_(site) {
  app_ = gtk_application_new("com.quicksorter.WebView",
                             G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app_, "activate", G_CALLBACK(onActivate), this);

  int status = g_application_run(G_APPLICATION(app_), 0, nullptr);
  (void)status;

  g_object_unref(app_);
}

} // namespace quicksorter
