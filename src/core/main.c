#include "main.h"

#include "../platform_guard.h"

GtkApplication *app     = NULL;
GtkBuilder     *builder = NULL;

GtkWidget *mainWnd               = NULL;
GtkWidget *exitDialog            = NULL;
GtkWidget *wallpaperMgrWnd       = NULL;
GtkWidget *monitorWnd            = NULL;
GtkWidget *monitorListBox        = NULL;
GtkWidget *wallpaperListBox      = NULL;
GtkWidget *wallpaperComboBox     = NULL;
GtkWidget *xPosSpinBtn           = NULL;
GtkWidget *yPosSpinBtn           = NULL;
GtkWidget *widthSpinBtn          = NULL;
GtkWidget *heightSpinBtn         = NULL;
GtkWidget *monitorNameLabel      = NULL;
GtkWidget *versionLabel          = NULL;
GtkWidget *appSettingsWnd        = NULL;
GtkWidget *targetFpsComboBox     = NULL;
GtkWidget *renderQualityComboBox = NULL;
GtkWidget *drawOnRootWndComboBox = NULL;

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

  char iconPath[PATH_MAX];
  getAppDir(iconPath, APP_DIR_SHARE);
#ifdef __WIN32
  const char *format = "%s\\%s\\%s\\%s";
#else
  const char *format = "%s/%s/%s/%s";
#endif
  sprintf(
      iconPath, format, iconPath, "window_templates", "assets", "screen-monitor-svgrepo-com.svg"
  );

  int          monitorsCount;
  MonitorInfo *monitors;

  monitors = scanMonitors(&monitorsCount);

  for (int i = 0; i < monitorsCount; i++)
  {
    char resStr[12];
    sprintf(resStr, "%dx%d", monitors[i].clientBounds.w, monitors[i].clientBounds.h);

    GtkWidget *nameLabel = gtk_label_new(monitors[i].displayName);
    GtkWidget *resLabel  = gtk_label_new(resStr);
    GtkWidget *icon      = gtk_image_new_from_file(iconPath);

    GtkWidget *labelBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(labelBox), nameLabel);
    gtk_container_add(GTK_CONTAINER(labelBox), resLabel);
    gtk_box_set_child_packing(GTK_BOX(labelBox), nameLabel, 1, 1, 0, GTK_PACK_START);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(box), icon);
    gtk_container_add(GTK_CONTAINER(box), labelBox);
    gtk_box_set_child_packing(GTK_BOX(box), labelBox, 1, 1, 0, GTK_PACK_START);

    GtkWidget *row = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row), box);

    gtk_list_box_insert(GTK_LIST_BOX(monitorListBox), row, 0);

    char *nameBuff = malloc(strlen(monitors[i].name)+1);
    strcpy(nameBuff, monitors[i].name);

    char *displayNameBuff = malloc(strlen(monitors[i].displayName)+1);
    strcpy(displayNameBuff, monitors[i].displayName);

    g_object_set_data(G_OBJECT(row), "monitor_display_name", (gpointer)displayNameBuff);
    g_object_set_data(G_OBJECT(row), "monitor_name", (gpointer)nameBuff);

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

    mainWnd         = (GtkWidget *)gtk_builder_get_object(builder, "MainWindow");
    exitDialog      = (GtkWidget *)gtk_builder_get_object(builder, "ExitDialog");
    wallpaperMgrWnd = (GtkWidget *)gtk_builder_get_object(builder, "WallpaperManagerWindow");
    monitorWnd      = (GtkWidget *)gtk_builder_get_object(builder, "MonitorWindow");
    appSettingsWnd  = (GtkWidget *)gtk_builder_get_object(builder, "SettingsWindow");
    monitorListBox  = (GtkWidget *)gtk_builder_get_object(builder, "MainWindow_MonitorListBox");
    versionLabel    = (GtkWidget *)gtk_builder_get_object(builder, "MainWindow_VersionLabel");
    wallpaperListBox =
        (GtkWidget *)gtk_builder_get_object(builder, "WallpaperManagerWindow_WallpaperListBox");
    wallpaperComboBox =
        (GtkWidget *)gtk_builder_get_object(builder, "MonitorWindow_WallpaperComboBox");
    xPosSpinBtn   = (GtkWidget *)gtk_builder_get_object(builder, "MonitorWindow_XPosSpinBtn");
    yPosSpinBtn   = (GtkWidget *)gtk_builder_get_object(builder, "MonitorWindow_YPosSpinBtn");
    widthSpinBtn  = (GtkWidget *)gtk_builder_get_object(builder, "MonitorWindow_WidthSpinBtn");
    heightSpinBtn = (GtkWidget *)gtk_builder_get_object(builder, "MonitorWindow_HeightSpinBtn");
    monitorNameLabel =
        (GtkWidget *)gtk_builder_get_object(builder, "MonitorWindow_MonitorNameLabel");
    targetFpsComboBox =
        (GtkWidget *)gtk_builder_get_object(builder, "SettingsWindow_TargetFpsComboBox");
    renderQualityComboBox =
        (GtkWidget *)gtk_builder_get_object(builder, "SettingsWindow_TexFilteringComboBox");
    drawOnRootWndComboBox =
        (GtkWidget *)gtk_builder_get_object(builder, "SettingsWindow_DrawOnRootWndComboBox");

    gtk_window_set_application(GTK_WINDOW(mainWnd), GTK_APPLICATION(app));
    gtk_window_set_application(GTK_WINDOW(exitDialog), GTK_APPLICATION(app));
    gtk_window_set_application(GTK_WINDOW(wallpaperMgrWnd), GTK_APPLICATION(app));
    gtk_window_set_application(GTK_WINDOW(monitorWnd), GTK_APPLICATION(app));
    gtk_window_set_application(GTK_WINDOW(appSettingsWnd), GTK_APPLICATION(app));

    gtk_label_set_text(GTK_LABEL(versionLabel), PROGRAM_VERSION);

    runWlp();
  }

  reloadMonitorListBox();

  if (alreadyRunning) gtk_widget_set_visible(mainWnd, 1);

  alreadyRunning = 1;
}

int main(int argc, char *argv[])
{
  int status;

  createUserDirs();

#ifdef __WIN32
  initTrayIcon();
#endif

  app = gtk_application_new("com.github.jszczerbinsky.lwp", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

#ifdef __WIN32
  removeTrayIcon();
#endif

  return status;
}
