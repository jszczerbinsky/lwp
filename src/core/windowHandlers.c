#include "main.h"

// Exit Dialog handlers

G_MODULE_EXPORT void ExitDialogClose() { gtk_widget_set_visible(exitDialog, 0); }

G_MODULE_EXPORT void ExitDialog_No() { gtk_widget_set_visible(exitDialog, 0); }

G_MODULE_EXPORT void ExitDialog_Yes()
{
  killWlp();
  g_application_quit(G_APPLICATION(app));
}

// Main Window handlers

G_MODULE_EXPORT void MainWindow_ManageWallpapersBtnClick()
{
  GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
  GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(mainWnd),
      flags,
      GTK_MESSAGE_ERROR,
      GTK_BUTTONS_CLOSE,
      "This feature will be available in the future");
    gtk_dialog_run (GTK_DIALOG (dialog));
 gtk_widget_destroy (dialog);
  //gtk_widget_set_visible(wallpaperMgrWnd, 1);
}

G_MODULE_EXPORT void MainWindow_MonitorEditBtnClick() { gtk_widget_set_visible(monitorWnd, 1); }

G_MODULE_EXPORT void MainWindow_ExitBtnClick() { gtk_widget_set_visible(exitDialog, 1); }

G_MODULE_EXPORT void MainWindow_AppSettingsBtnClick() { gtk_widget_set_visible(appSettingsWnd, 1); }

G_MODULE_EXPORT void MainWindowClose() { gtk_widget_set_visible(mainWnd, 0); }

// Wallpapaer Manager Window handlers

G_MODULE_EXPORT void WallpaperManagerWindowClose() { gtk_widget_set_visible(wallpaperMgrWnd, 0); }

G_MODULE_EXPORT void WallpaperManagerWindowShow()
{
  int            wlpCount;
  WallpaperInfo *wlpList = scanWallpapers(&wlpCount);

  for (int i = 0; i < wlpCount; i++)
  {
    GtkWidget *label = gtk_label_new(wlpList[i].name);
    GtkWidget *row   = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row), label);

    gtk_list_box_insert(GTK_LIST_BOX(wallpaperListBox), row, 0);
    gtk_widget_show_all(row);
  }

  free(wlpList);
}

// Monitor Window handlers

G_MODULE_EXPORT void MonitorWindowClose() { gtk_widget_set_visible(monitorWnd, 0); }
G_MODULE_EXPORT void MonitorWindowShow()
{
  // Clear wallpaper list
  gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(wallpaperComboBox));

  // Load wallpaper list
  int            wlpCount;
  WallpaperInfo *wlpList = scanWallpapers(&wlpCount);

  for (int i = 0; i < wlpCount; i++)
  {
    gtk_combo_box_text_insert(
        GTK_COMBO_BOX_TEXT(wallpaperComboBox), 0, wlpList[i].name, wlpList[i].name
    );
  }
  gtk_combo_box_set_active(GTK_COMBO_BOX(wallpaperComboBox), 0);

  free(wlpList);

  // Find selected monitor name
  GtkListBoxRow *listBoxRow  = gtk_list_box_get_selected_row(GTK_LIST_BOX(monitorListBox));
  const char    *monitorDisplayName = g_object_get_data(G_OBJECT(listBoxRow), "monitor_display_name");
  const char    *monitorName = g_object_get_data(G_OBJECT(listBoxRow), "monitor_name");
  gtk_label_set_text(GTK_LABEL(monitorNameLabel), monitorDisplayName);

  char *nameBuff = strdup(monitorName);
  g_object_set_data(G_OBJECT(monitorWnd), "monitor_name", (gpointer)nameBuff);

  // Read configuration from config file
  MonitorConfig mc;
  if (loadMonitorConfig(monitorName, &mc))
  {
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(xPosSpinBtn), (gdouble)mc.wlpBounds.x);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(yPosSpinBtn), (gdouble)mc.wlpBounds.y);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widthSpinBtn), (gdouble)mc.wlpBounds.w);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(heightSpinBtn), (gdouble)mc.wlpBounds.h);

    gtk_combo_box_set_active_id(GTK_COMBO_BOX(wallpaperComboBox), mc.wlpName);
  }
}

G_MODULE_EXPORT void MonitorWindow_ApplyBtnClick()
{
  MonitorConfig mc;
  strcpy(mc.wlpName, gtk_combo_box_get_active_id(GTK_COMBO_BOX(wallpaperComboBox)));
  mc.wlpBounds.x = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(xPosSpinBtn));
  mc.wlpBounds.y = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(yPosSpinBtn));
  mc.wlpBounds.w = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widthSpinBtn));
  mc.wlpBounds.h = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(heightSpinBtn));

  saveMonitorConfig(g_object_get_data(G_OBJECT(monitorWnd), "monitor_name"), &mc);

  killWlp();
  runWlp();
}

G_MODULE_EXPORT void MonitorWindow_ExitBtnClick() { gtk_widget_set_visible(monitorWnd, 0); }

// App Settings Window handlers

G_MODULE_EXPORT void SettingsWindowShow()
{
  AppConfig ac;
  loadAppConfig(&ac);

  char targetFpsStr[4];
  sprintf(targetFpsStr, "%d", ac.targetFps);

#ifdef __LINUX
  char drawOnRootWindowStr[2];
  sprintf(drawOnRootWindowStr, "%d", ac.drawOnRootWindow);
#else
  char *drawOnRootWindowStr = "0";
#endif
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(renderQualityComboBox), ac.renderQuality);
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(targetFpsComboBox), targetFpsStr);
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(drawOnRootWndComboBox), drawOnRootWindowStr);
}

G_MODULE_EXPORT void SettingsWindowClose() { gtk_widget_set_visible(appSettingsWnd, 0); }

G_MODULE_EXPORT void SettingsWindow_ApplyBtnClick()
{
  AppConfig ac;
  strcpy(ac.renderQuality, gtk_combo_box_get_active_id(GTK_COMBO_BOX(renderQualityComboBox)));
  ac.targetFps        = atoi(gtk_combo_box_get_active_id(GTK_COMBO_BOX(targetFpsComboBox)));
  ac.drawOnRootWindow = atoi(gtk_combo_box_get_active_id(GTK_COMBO_BOX(drawOnRootWndComboBox)));

  saveAppConfig(&ac);

  killWlp();
  runWlp();
}

G_MODULE_EXPORT void SettingsWindow_ExitBtnClick() { gtk_widget_set_visible(appSettingsWnd, 0); }
