#include <gtk/gtk.h>

#ifdef __WIN32
    #define MAIN_WINDOW_TEMPLATE_PATH ""
#elif __DARWIN
    #define MAIN_WINDOW_TEMPLATE_PATH ""
#else
    #define MAIN_WINDOW_TEMPLATE_PATH "/usr/local/share/lwp/window_templates/main.glade"
#endif

static void activate(GtkApplication *app, gpointer userdata)
{
  GtkBuilder *builder = gtk_builder_new_from_file(MAIN_WINDOW_TEMPLATE_PATH);
  GtkWidget  *window  = (GtkWidget *)gtk_builder_get_object(builder, "MainWindow");
  if (window == NULL) printf("wnd not found\n");
  gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));
  gtk_widget_set_visible(window, 1);
}

int main(int argc, char *argv[])
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("com.github.jszczerbinsky.lwp", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}