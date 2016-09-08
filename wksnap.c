/*
 * Copyright 2016 Igalia S.L.
 * Authors: Carlos Alberto Lopez Perez <clopez@igalia.com>
 *
 * This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
*/

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <webkit2/webkit2.h>

/* compile with:
 * gcc -o wksnap wksnap.c $(pkg-config --cflags --libs webkit2gtk-4.0)
 * Requires WebKit2GTK+ and an xserver (Xvfb can be used to run headless)
 */

static void
snapshot_finish_cb(WebKitWebView *webview,
                  GAsyncResult *res,
                  char *destfile)
{
  GError *err = NULL;
  cairo_surface_t *surface = webkit_web_view_get_snapshot_finish(WEBKIT_WEB_VIEW(webview),res,&err);
  if (err) {
    printf("An error happened generating the snapshot: %s\n",err->message);
    exit(1);
  }
  cairo_surface_write_to_png (surface, destfile);
  exit(0);
}

static void
load_changed_cb (WebKitWebView *webview,
                WebKitLoadEvent status,
                char *destfile) {

  if (status != WEBKIT_LOAD_FINISHED) {
    return;
  }

  webkit_web_view_get_snapshot(webview,
         WEBKIT_SNAPSHOT_REGION_FULL_DOCUMENT,
         WEBKIT_SNAPSHOT_OPTIONS_NONE, // WEBKIT_SNAPSHOT_OPTIONS_TRANSPARENT_BACKGROUND ?
         NULL,
         (GAsyncReadyCallback)snapshot_finish_cb,
         destfile);
}


int
strStartsWith(const char *a,
              const char *b) {
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

int
main (int    argc,
      char **argv)
{
  GtkWidget *window;
  GtkWidget *webview;
  char *url, *destfile;
  static char *fileuri = "file://";


  if (argc != 3) {
    printf ("Use %s [website-uri|file] path-to-destination.png\n",argv[0]);
    exit (1);
  }

  destfile = argv[2];

  if (strStartsWith(argv[1], "http://") ||
      strStartsWith(argv[1], "https://") ||
      strStartsWith(argv[1], "file://")) {
        url = argv[1];
  } else if( access( argv[1], F_OK ) != -1 ) {
        url = (char *)malloc(sizeof(char) * (strlen(argv[1]) + strlen(fileuri) + 1));
        strcpy(url, fileuri);
        strcat(url, argv[1]);
  } else {
    printf ("%s is an unknown uri. Valid options are files, http:// and https://\n",argv[1]);
    exit(1);
  }

  gtk_init (&argc, &argv);
  printf ("Taking snapshoot of: %s to file: %s\n", url, destfile);
  window = gtk_offscreen_window_new ();
  gtk_window_set_default_size(GTK_WINDOW(window), 1280, 720);
  webview = webkit_web_view_new ();
  gtk_container_add (GTK_CONTAINER (window), webview);
  g_signal_connect (webview, "load-changed",
                    G_CALLBACK (load_changed_cb), destfile);
  // Ignore any SSL errors. Don't makes sense to abort taking a snapshoot because of a bad cert.
  webkit_web_context_set_tls_errors_policy(webkit_web_view_get_context(WEBKIT_WEB_VIEW (webview)),
                                           WEBKIT_TLS_ERRORS_POLICY_IGNORE);
  webkit_web_view_load_uri (WEBKIT_WEB_VIEW (webview), url);
  gtk_widget_show_all (window);

  gtk_main ();
}
