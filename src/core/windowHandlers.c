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
  gtk_widget_set_visible(wallpaperMgrWnd, 1);
}

G_MODULE_EXPORT void MainWindow_MonitorEditBtnClick() { gtk_widget_set_visible(monitorWnd, 1); }

G_MODULE_EXPORT void MainWindow_ExitBtnClick() { gtk_widget_set_visible(exitDialog, 1); }

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
  GList         *children    = gtk_container_get_children(GTK_CONTAINER(listBoxRow));
  const char    *monitorName = gtk_label_get_label(GTK_LABEL(children->data));
  gtk_label_set_text(GTK_LABEL(monitorNameLabel), monitorName);
  g_list_free(children);

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

  saveMonitorConfig(gtk_label_get_text(GTK_LABEL(monitorNameLabel)), &mc);

  killWlp();
  runWlp();
}
