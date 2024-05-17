#include "main.h"

GtkApplication *app     = NULL;
GtkBuilder     *builder = NULL;

Control controls[] = {
    {.name = "MainWindow"},
    {.name = "Mon_List"},
    {.name = "Mon_Switch"},
    {.name = "Mon_Wallpaper"},
    {.name = "Mon_OffsetX"},
    {.name = "Mon_OffsetY"},
    {.name = "Mon_Width"},
    {.name = "Mon_Height"},
    {.name = "App_TargetFps"},
    {.name = "App_TexFiltering"},
    {.name = "App_UnfocusedBehaviour"},
    {.name = "App_TargetPoint"},
    {.name = "VersionLabel"},
    {.name = "MainStack"},
    {.name = "Sidebar"},
    {.name = "Mon_AspectRatio"},
    {.name = "LogWindow"},
    {.name = "Log_Text"},
};

static void reloadMonitorListBox()
{
  GList *rows =
      gtk_container_get_children(GTK_CONTAINER(controls[CONTROL_MON_LIST].widget
      ));

  GList *ptr = rows;
  while (ptr)
  {
    gtk_container_remove(
        GTK_CONTAINER(controls[CONTROL_MON_LIST].widget), ptr->data
    );
    ptr = ptr->next;
  }

  g_list_free(rows);

  int          monitorsCount;
  MonitorInfo *monitors;

  monitors = scanMonitors(&monitorsCount);

  for (int i = 0; i < monitorsCount; i++)
  {
    char resStr[12];
    sprintf(
        resStr, "%dx%d", monitors[i].clientBounds.w, monitors[i].clientBounds.h
    );

    GtkWidget *nameLabel = gtk_label_new(monitors[i].displayName);
    GtkWidget *resLabel  = gtk_label_new(resStr);
    GtkWidget *icon =
        gtk_image_new_from_icon_name("video-display", GTK_ICON_SIZE_DIALOG);
    gtk_widget_set_margin_start(icon, 10);

    GtkWidget *labelBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(labelBox), nameLabel);
    gtk_container_add(GTK_CONTAINER(labelBox), resLabel);
    gtk_box_set_child_packing(
        GTK_BOX(labelBox), nameLabel, 1, 1, 0, GTK_PACK_START
    );

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(box), icon);
    gtk_container_add(GTK_CONTAINER(box), labelBox);
    gtk_box_set_child_packing(GTK_BOX(box), labelBox, 1, 1, 0, GTK_PACK_START);

    GtkWidget *row = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row), box);

    gtk_list_box_insert(
        GTK_LIST_BOX(controls[CONTROL_MON_LIST].widget), row, 0
    );

    char *nameBuff = malloc(strlen(monitors[i].name) + 1);
    strcpy(nameBuff, monitors[i].name);

    char *displayNameBuff = malloc(strlen(monitors[i].displayName) + 1);
    strcpy(displayNameBuff, monitors[i].displayName);

    g_object_set_data(
        G_OBJECT(row), "monitor_display_name", (gpointer)displayNameBuff
    );

    int *width  = (int *)malloc(sizeof(int));
    int *height = (int *)malloc(sizeof(int));

    *width  = monitors[i].pixelBounds.w;
    *height = monitors[i].pixelBounds.h;

    g_object_set_data(G_OBJECT(row), "monitor_name", (gpointer)nameBuff);
    g_object_set_data(G_OBJECT(row), "monitor_width", (gpointer)width);
    g_object_set_data(G_OBJECT(row), "monitor_height", (gpointer)height);

    gtk_widget_show_all(row);
  }
}

static void activate(GtkApplication *app, gpointer userdata)
{
  static int alreadyRunning = 0;

  if (!alreadyRunning)
  {
    char gladefilePath[PATH_MAX];
    getAppDir(gladefilePath, APP_DIR_SHARE);
#ifdef __WIN32
    const char *format = "%s\\window_templates\\main.glade";
#else
    const char *format = "%s/window_templates/main.glade";
#endif
    sprintf(gladefilePath, format, gladefilePath);

    builder = gtk_builder_new_from_file(gladefilePath);
    gtk_builder_connect_signals(builder, NULL);

    for (int i = 0; i < CONTROLS_MAX; i++)
      controls[i].widget =
          (GtkWidget *)gtk_builder_get_object(builder, controls[i].name);

    gtk_window_set_application(
        GTK_WINDOW(controls[CONTROL_MAIN_WND].widget), GTK_APPLICATION(app)
    );

    gtk_label_set_text(
        GTK_LABEL(controls[CONTROL_VER_LABEL].widget), PROGRAM_VERSION
    );

    runWlp();
  }

  reloadMonitorListBox();

  if (alreadyRunning)
    gtk_widget_set_visible(controls[CONTROL_MAIN_WND].widget, 1);

  alreadyRunning = 1;
}

int main(int argc, char *argv[])
{
  int status;

  createUserDirs();

#ifdef __WIN32
  SetEnvironmentVariable("GTK_THEME", "Adwaita:dark");
  initTrayIcon();
#endif

#if GLIB_CHECK_VERSION(2, 74, 0)
  app = gtk_application_new(
      "com.github.jszczerbinsky.lwp", G_APPLICATION_DEFAULT_FLAGS
  );
#else
  app = gtk_application_new(
      "com.github.jszczerbinsky.lwp", G_APPLICATION_FLAGS_NONE
  );
#endif

  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

#ifdef __WIN32
  removeTrayIcon();
#endif

  return status;
}
