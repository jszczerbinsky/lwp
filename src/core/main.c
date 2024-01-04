#include "main.h"

#ifdef __WIN32
#define MAIN_WINDOW_TEMPLATE_PATH "./window_templates/main.glade"
#elif __DARWIN
#define MAIN_WINDOW_TEMPLATE_PATH ""
#else
#define MAIN_WINDOW_TEMPLATE_PATH "/usr/local/share/lwp/window_templates/main.glade"
#endif

GtkApplication *app     = NULL;
GtkBuilder     *builder = NULL;

GtkWidget *mainWnd         = NULL;
GtkWidget *exitDialog      = NULL;
GtkWidget *wallpaperMgrWnd = NULL;
GtkWidget *monitorWnd      = NULL;
GtkWidget *monitorListBox  = NULL;

static void reloadMonitorListBox()
{
  GList *rows = gtk_container_get_children(GTK_CONTAINER(monitorListBox));

  GList *ptr = rows;
  while (ptr)
  {
    gtk_container_remove(GTK_CONTAINER(monitorListBox), ptr->data);
    ptr = ptr->next;
  }

  g_list_free(rows);

  int      monitorsCount;
  Monitor *monitors;

  monitors = scanMonitors(&monitorsCount);

  for (int i = 0; i < 1; i++)
  {
    GtkWidget *label = gtk_label_new(monitors[i].name);
    GtkWidget *row   = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row), label);

    gtk_list_box_insert(GTK_LIST_BOX(monitorListBox), row, 0);
    gtk_widget_show_all(row);
  }
}

static void activate(GtkApplication *app, gpointer userdata)
{
  static int alreadyRunning = 0;

  if (!alreadyRunning)
  {
    alreadyRunning = 1;

    builder = gtk_builder_new_from_file(MAIN_WINDOW_TEMPLATE_PATH);
    gtk_builder_connect_signals(builder, NULL);

    mainWnd         = (GtkWidget *)gtk_builder_get_object(builder, "MainWindow");
    exitDialog      = (GtkWidget *)gtk_builder_get_object(builder, "ExitDialog");
    wallpaperMgrWnd = (GtkWidget *)gtk_builder_get_object(builder, "WallpaperManagerWindow");
    monitorWnd      = (GtkWidget *)gtk_builder_get_object(builder, "MonitorWindow");
    monitorListBox  = (GtkWidget *)gtk_builder_get_object(builder, "MainWindow_MonitorListBox");

    gtk_window_set_application(GTK_WINDOW(mainWnd), GTK_APPLICATION(app));
    gtk_window_set_application(GTK_WINDOW(exitDialog), GTK_APPLICATION(app));
    gtk_window_set_application(GTK_WINDOW(wallpaperMgrWnd), GTK_APPLICATION(app));
    gtk_window_set_application(GTK_WINDOW(monitorWnd), GTK_APPLICATION(app));

    runWlp();
  }

  reloadMonitorListBox();

  gtk_widget_set_visible(mainWnd, 1);
}

int main(int argc, char *argv[])
{
  int status;

  app = gtk_application_new("com.github.jszczerbinsky.lwp", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
