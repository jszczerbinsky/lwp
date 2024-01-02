#include "main.h"

#ifdef __WIN32
#define MAIN_WINDOW_TEMPLATE_PATH ""
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
GtkWidget *screenWnd       = NULL;
GtkWidget *screenListBox   = NULL;

static FILE *wlpProcess = NULL;
static int   wlpPid     = 0;

void killWlp()
{
  if (wlpPid != 0) kill(wlpPid, SIGINT);
  if (wlpProcess != NULL) pclose(wlpProcess);

  wlpPid     = 0;
  wlpProcess = NULL;
}

static void runWlp()
{
  wlpProcess = popen("/home/cziken/Projects/lwp/build/src/wlp/lwpwlp", "r");
  char buff[10];
  fgets(buff, sizeof(buff) - 1, wlpProcess);
  wlpPid = atoi(buff);
}

static void reloadScreenListBox()
{
  GList *rows = gtk_container_get_children(GTK_CONTAINER(screenListBox));

  GList *ptr = rows;
  while (ptr)
  {
    gtk_container_remove(GTK_CONTAINER(screenListBox), ptr->data);
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

    gtk_list_box_insert(GTK_LIST_BOX(screenListBox), row, 0);
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
    screenWnd       = (GtkWidget *)gtk_builder_get_object(builder, "ScreenWindow");
    screenListBox   = (GtkWidget *)gtk_builder_get_object(builder, "MainWindow_ScreenListBox");

    gtk_window_set_application(GTK_WINDOW(mainWnd), GTK_APPLICATION(app));
    gtk_window_set_application(GTK_WINDOW(exitDialog), GTK_APPLICATION(app));
    gtk_window_set_application(GTK_WINDOW(wallpaperMgrWnd), GTK_APPLICATION(app));
    gtk_window_set_application(GTK_WINDOW(screenWnd), GTK_APPLICATION(app));

    runWlp();
  }

  reloadScreenListBox();

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
