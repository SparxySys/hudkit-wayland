/*
   * Original transparent window code by Mike - http://plan99.net/~mike/blog
     Dead link, but on Internet Archive most recently at:
     https://web.archive.org/web/20121127025149/http://mikehearn.wordpress.com/2006/03/26/gtk-windows-with-alpha-channels/
   * Modified by karlphillip for StackExchange -
     http://stackoverflow.com/questions/3908565/how-to-make-gtk-window-background-transparent
   * Re-worked for Gtk 3 by Louis Melahn, L.C., January 30, 2014.
   * Extended with WebKit and input shape kill by Antti Korpi <an@cyan.io>, on
     June 18, 2014.
   * Updated to WebKit 2 by Antti Korpi <an@cyan.io> on December 12, 2017.
 */

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <webkit2/webkit2.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

//@TODO: Make this a c++ app and do things in a better manner.
//Until then, here's some jank to make managing this a bit easier.
#include "config.c"
#include "gtk.c"
#include "webkit.c"
#include "hotkey.c"
#include "gtk-layer-shell.h"

GtkApplication * app;

void int_handler(int s) {
    g_application_quit(app);
}

static void activate_app(GtkApplication* app, void *_data)
{
    struct sigaction intHandler;
    intHandler.sa_handler = int_handler;
    sigemptyset(&intHandler.sa_mask);
    intHandler.sa_flags=0;
    sigaction(SIGINT, &intHandler, NULL);

    // Create the window, set basic properties
    window = gtk_application_window_new(app);
    GtkWindow *gtk_window = GTK_WINDOW(window);

    // We want to be in the wayland overlay layer with no interactivity
    gtk_layer_init_for_window(gtk_window);
    gtk_layer_set_layer(gtk_window, GTK_LAYER_SHELL_LAYER_OVERLAY);
    gtk_layer_set_keyboard_mode(gtk_window, GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);
    GdkDisplay *display = gdk_display_get_default();

    // For multi-monitor setups since you can't drag an overlay
    int monitor_count = gdk_display_get_n_monitors(display);
    if (monitor_count < monitor) {
        fprintf(stderr, "Config monitor is invalid.\n");
    } else {
        GdkMonitor *moni = gdk_display_get_monitor(display, monitor);
        gtk_layer_set_monitor(gtk_window, moni);
    }

    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_LEFT, x);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_TOP, y);

    static const gboolean anchors[] = {TRUE, FALSE, TRUE, FALSE};
    for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
        gtk_layer_set_anchor(gtk_window, i, anchors[i]);
    }

    gtk_window_set_default_size(gtk_window, width, height);
    // since we're an overlay and only anchoring on two sides, we need to force set the size
    gtk_widget_set_size_request(GTK_WIDGET (gtk_window), width, height);
    gtk_window_resize (gtk_window, 1, 1);

    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_widget_add_events(GTK_WIDGET(window), GDK_CONFIGURE);
    g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(g_application_quit), NULL);
    gtk_widget_set_app_paintable(window, TRUE);

    // Set up and add the WebKit web view widget
    // Disable caching
    WebKitWebContext *wk_context = webkit_web_context_get_default();
    webkit_web_context_set_cache_model(wk_context, WEBKIT_CACHE_MODEL_DOCUMENT_VIEWER);
    WebKitSecurityManager *security_manager = webkit_web_context_get_security_manager(wk_context);
    webkit_security_manager_register_uri_scheme_as_secure(security_manager, "ws");
    // allow audio autoplay so you don't need to click the helper button
    WebKitWebsitePolicies *policies = webkit_website_policies_new_with_policies("autoplay", WEBKIT_AUTOPLAY_ALLOW, NULL);
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(g_object_new(WEBKIT_TYPE_WEB_VIEW, "website-policies", policies, webkit_web_view_new_with_context(wk_context)));
    webkit_web_view_set_zoom_level(web_view, zoom);
    // Enable browser console logging to stdout
    WebKitSettings *wk_settings = webkit_settings_new();
    webkit_settings_set_enable_write_console_messages_to_stdout(wk_settings, true);
    webkit_settings_set_media_playback_requires_user_gesture(wk_settings, false);
    webkit_web_view_set_settings(web_view, wk_settings);
    // Make transparent
    GdkRGBA rgba = { .alpha = 0.0 };
    webkit_web_view_set_background_color(web_view, &rgba);

    // We need to put the webview in a scrolledwindow so we don't get a 1px sized webview widget
    GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy(scrolled_window, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER (scrolled_window), GTK_WIDGET(web_view));

    // Load the specified URI
    webkit_web_view_load_uri(web_view, url);

    // Initialise the window and make it active.  We need this so it can
    // fullscreen to the correct size.
    screen_changed(window, NULL, NULL);

    gtk_container_add (GTK_CONTAINER (gtk_window), scrolled_window);
    gtk_container_set_border_width (GTK_CONTAINER (gtk_window), 0);
    gtk_widget_show_all(GTK_WIDGET (gtk_window));

    // "Can't touch this!" - to the user
    //
    // Set the input shape (area where clicks are recognised) to a zero-width,
    // zero-height region a.k.a. nothing.  This makes clicks pass through the
    // window onto whatever's below.
    GdkWindow *gdk_window = gtk_widget_get_window(GTK_WIDGET(window));
    gdk_window_input_shape_combine_region(GDK_WINDOW(gdk_window),cairo_region_create(), 0,0);
    register_hotkey();
}

int main(int argc, char **argv) {
    app = gtk_application_new("net.vampyrebytes.hudkit", G_APPLICATION_NON_UNIQUE);

    if (argc < 2) {
        fprintf(stderr, "Expected 1 argument, got 0:\n");
        fprintf(stderr, "You should pass a json config file path.\n\n");
        fprintf(stderr, "For example,\n");
        fprintf(stderr, "    %s \"config.json\"\n\n", argv[0]);
        exit(1);
    }

    read_config(argv[1]);
	g_signal_connect(app, "activate", G_CALLBACK (activate_app), NULL);
	char *empty_args[0]; // don't let gtk parse the application arguments
	int status = g_application_run(G_APPLICATION (app), 0, empty_args);
    g_object_unref(app);
    return status;
}
