#include "main.h"

#define VIEW_MON      0
#define VIEW_SETTINGS 1

static int currentView = VIEW_MON;

// Exit Dialog handlers
/*
G_MODULE_EXPORT void ExitDialogClose()
{
  gtk_widget_set_visible(exitDialog, 0);
}

G_MODULE_EXPORT void ExitDialog_No() { gtk_widget_set_visible(exitDialog,
0); }

G_MODULE_EXPORT void ExitDialog_Yes()
{
  killWlp();
  g_application_quit(G_APPLICATION(app));
}*/

// Main Window handlers
/*
G_MODULE_EXPORT void MainWindow_ManageWallpapersBtnClick()
{
  GtkDialogFlags flags  = GTK_DIALOG_DESTROY_WITH_PARENT;
  GtkWidget     *dialog = gtk_message_dialog_new(
      GTK_WINDOW(mainWnd),
      flags,
      GTK_MESSAGE_ERROR,
      GTK_BUTTONS_CLOSE,
      "This feature will be available in the future"
  );
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
  // gtk_widget_set_visible(wallpaperMgrWnd, 1);
}

G_MODULE_EXPORT void MainWindow_MonitorEditBtnClick()
{
  gtk_widget_set_visible(monitorWnd, 1);
}

G_MODULE_EXPORT void MainWindow_ExitBtnClick()
{
  gtk_widget_set_visible(exitDialog, 1);
}

G_MODULE_EXPORT void MainWindow_AppSettingsBtnClick()
{
  gtk_widget_set_visible(appSettingsWnd, 1);
}


// Wallpapaer Manager Window handlers

G_MODULE_EXPORT void WallpaperManagerWindowClose()
{
  gtk_widget_set_visible(wallpaperMgrWnd, 0);
}

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

G_MODULE_EXPORT void MonitorWindowClose()
{
  gtk_widget_set_visible(monitorWnd, 0);
}
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
        GTK_COMBO_BOX_TEXT(wallpaperComboBox),
        0,
        wlpList[i].name,
        wlpList[i].name
    );
  }
  gtk_combo_box_set_active(GTK_COMBO_BOX(wallpaperComboBox), 0);

  free(wlpList);

  // Find selected monitor name
  GtkListBoxRow *listBoxRow =
      gtk_list_box_get_selected_row(GTK_LIST_BOX(monitorListBox));
  const char *monitorDisplayName =
      g_object_get_data(G_OBJECT(listBoxRow), "monitor_display_name");
  const char *monitorName =
      g_object_get_data(G_OBJECT(listBoxRow), "monitor_name");
  gtk_label_set_text(GTK_LABEL(monitorNameLabel), monitorDisplayName);

  char *nameBuff = strdup(monitorName);
  g_object_set_data(G_OBJECT(monitorWnd), "monitor_name",
(gpointer)nameBuff);

  // Read configuration from config file
  MonitorConfig mc;
  if (loadMonitorConfig(monitorName, &mc))
  {
    gtk_spin_button_set_value(
        GTK_SPIN_BUTTON(xPosSpinBtn), (gdouble)mc.wlpBounds.x
    );
    gtk_spin_button_set_value(
        GTK_SPIN_BUTTON(yPosSpinBtn), (gdouble)mc.wlpBounds.y
    );
    gtk_spin_button_set_value(
        GTK_SPIN_BUTTON(widthSpinBtn), (gdouble)mc.wlpBounds.w
    );
    gtk_spin_button_set_value(
        GTK_SPIN_BUTTON(heightSpinBtn), (gdouble)mc.wlpBounds.h
    );

    gtk_combo_box_set_active_id(GTK_COMBO_BOX(wallpaperComboBox),
mc.wlpName);
  }
}

G_MODULE_EXPORT void MonitorWindow_ApplyBtnClick()
{
  MonitorConfig mc;
  strcpy(
      mc.wlpName,
gtk_combo_box_get_active_id(GTK_COMBO_BOX(wallpaperComboBox))
  );
  mc.wlpBounds.x =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(xPosSpinBtn));
  mc.wlpBounds.y =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(yPosSpinBtn));
  mc.wlpBounds.w =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widthSpinBtn));
  mc.wlpBounds.h =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(heightSpinBtn));

  saveMonitorConfig(
      g_object_get_data(G_OBJECT(monitorWnd), "monitor_name"), &mc
  );

  killWlp();
  runWlp();
}

G_MODULE_EXPORT void MonitorWindow_ExitBtnClick()
{
  gtk_widget_set_visible(monitorWnd, 0);
}

// App Settings Window handlers

G_MODULE_EXPORT void SettingsWindowShow()
{
  AppConfig ac;
  loadAppConfig(&ac);

  char targetFpsStr[4];
  sprintf(targetFpsStr, "%d", ac.targetFps);

  gtk_combo_box_set_active_id(
      GTK_COMBO_BOX(renderQualityComboBox), ac.renderQuality
  );
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(targetFpsComboBox),
targetFpsStr);
}

G_MODULE_EXPORT void SettingsWindowClose()
{
  gtk_widget_set_visible(appSettingsWnd, 0);
}

G_MODULE_EXPORT void SettingsWindow_ApplyBtnClick()
{
  AppConfig ac;
  strcpy(
      ac.renderQuality,
      gtk_combo_box_get_active_id(GTK_COMBO_BOX(renderQualityComboBox))
  );
  ac.targetFps =
      atoi(gtk_combo_box_get_active_id(GTK_COMBO_BOX(targetFpsComboBox)));

  saveAppConfig(&ac);

  killWlp();
  runWlp();
}

G_MODULE_EXPORT void SettingsWindow_ExitBtnClick()
{
  gtk_widget_set_visible(appSettingsWnd, 0);
}
*/

static void reloadWlpList()
{
  gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(WID(CONTROL_MON_WLP)));

  int            wlpCount;
  WallpaperInfo *wlpList = scanWallpapers(&wlpCount);

  for (int i = 0; i < wlpCount; i++)
  {
    gtk_combo_box_text_insert(
        GTK_COMBO_BOX_TEXT(WID(CONTROL_MON_WLP)),
        0,
        wlpList[i].name,
        wlpList[i].name
    );
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(WID(CONTROL_MON_WLP)), 0);
}

static const char *getSelectedMonName()
{
  GtkListBoxRow *row =
      gtk_list_box_get_selected_row(GTK_LIST_BOX(WID(CONTROL_MON_LIST)));

  return g_object_get_data(G_OBJECT(row), "monitor_name");
}

static int getSelectedMonWidth()
{
  GtkListBoxRow *row =
      gtk_list_box_get_selected_row(GTK_LIST_BOX(WID(CONTROL_MON_LIST)));

  return *(int *)g_object_get_data(G_OBJECT(row), "monitor_width");
}
static int getSelectedMonHeight()
{
  GtkListBoxRow *row =
      gtk_list_box_get_selected_row(GTK_LIST_BOX(WID(CONTROL_MON_LIST)));

  return *(int *)g_object_get_data(G_OBJECT(row), "monitor_height");
}

static void reloadAppSettings()
{
  AppConfig ac;
  loadAppConfig(&ac);

  char targetFpsStr[4];
  sprintf(targetFpsStr, "%d", ac.targetFps);

  char unfocusedBehaviourStr[10];
  sprintf(
      unfocusedBehaviourStr, "%s", ac.unfocusedBehaviour ? "Come back" : "Clamp"
  );

  char targetPointStr[15];
  sprintf(
      unfocusedBehaviourStr, "%s", ac.targetPoint ? "Focused window" : "Mouse"
  );

  gtk_combo_box_set_active_id(
      GTK_COMBO_BOX(WID(CONTROL_APP_TEX_FILTERING)), ac.renderQuality
  );
  gtk_combo_box_set_active_id(
      GTK_COMBO_BOX(WID(CONTROL_APP_TARGET_FPS)), targetFpsStr
  );
}

static void reloadMonSettings()
{
  const char *monitor_name = getSelectedMonName();

  MonitorConfig mc;
  if (loadMonitorConfig(monitor_name, &mc))
  {
    gtk_spin_button_set_value(
        GTK_SPIN_BUTTON(WID(CONTROL_MON_OFFSET_X)), (gdouble)mc.wlpBounds.x
    );
    gtk_spin_button_set_value(
        GTK_SPIN_BUTTON(WID(CONTROL_MON_OFFSET_Y)), (gdouble)mc.wlpBounds.y
    );
    gtk_spin_button_set_value(
        GTK_SPIN_BUTTON(WID(CONTROL_MON_WIDTH)), (gdouble)mc.wlpBounds.w
    );
    gtk_spin_button_set_value(
        GTK_SPIN_BUTTON(WID(CONTROL_MON_HEIGHT)), (gdouble)mc.wlpBounds.h
    );

    gtk_combo_box_set_active_id(
        GTK_COMBO_BOX(WID(CONTROL_MON_WLP)), mc.wlpName
    );

    gtk_switch_set_active(GTK_SWITCH(WID(CONTROL_MON_SWITCH)), mc.active);
  }
}

static void saveMonitor()
{
  MonitorConfig mc;
  strcpy(
      mc.wlpName,
      gtk_combo_box_get_active_id(GTK_COMBO_BOX(WID(CONTROL_MON_WLP)))
  );
  mc.wlpBounds.x =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(CONTROL_MON_OFFSET_X)
      ));
  mc.wlpBounds.y =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(CONTROL_MON_OFFSET_Y)
      ));
  mc.wlpBounds.w =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(CONTROL_MON_WIDTH)));
  mc.wlpBounds.h =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(CONTROL_MON_HEIGHT))
      );
  mc.active = gtk_switch_get_active(GTK_SWITCH(WID(CONTROL_MON_SWITCH)));

  saveMonitorConfig(getSelectedMonName(), &mc);

  killWlp();
  runWlp();
}

static void saveApp()
{
  AppConfig ac;
  strcpy(
      ac.renderQuality,
      gtk_combo_box_get_active_id(GTK_COMBO_BOX(WID(CONTROL_APP_TEX_FILTERING)))
  );
  ac.targetFps = atoi(
      gtk_combo_box_get_active_id(GTK_COMBO_BOX(WID(CONTROL_APP_TARGET_FPS)))
  );

  saveAppConfig(&ac);

  killWlp();
  runWlp();
}

G_MODULE_EXPORT void TurnOffClick()
{
  killWlp();
  g_application_quit(G_APPLICATION(app));
}

int aspect_ratio_ignore = 0;

G_MODULE_EXPORT void MonChangeWidth()
{
  if (!aspect_ratio_ignore &&
      gtk_switch_get_active(GTK_SWITCH(WID(CONTROL_MON_ASPECT_RATIO))))
  {
    int originalW = getSelectedMonWidth();
    int originalH = getSelectedMonHeight();

    int width =
        gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(CONTROL_MON_WIDTH))
        );

    float ratio = (float)width / originalW;

    aspect_ratio_ignore = 1;
    gtk_spin_button_set_value(
        GTK_SPIN_BUTTON(WID(CONTROL_MON_HEIGHT)), (gdouble)(originalH * ratio)
    );
    aspect_ratio_ignore = 0;
  }
}

G_MODULE_EXPORT void MonChangeHeight()
{
  if (!aspect_ratio_ignore &&
      gtk_switch_get_active(GTK_SWITCH(WID(CONTROL_MON_ASPECT_RATIO))))
  {
    int originalW = getSelectedMonWidth();
    int originalH = getSelectedMonHeight();

    int height =
        gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(CONTROL_MON_HEIGHT)
        ));

    float ratio = (float)height / originalH;

    aspect_ratio_ignore = 1;
    gtk_spin_button_set_value(
        GTK_SPIN_BUTTON(WID(CONTROL_MON_WIDTH)), (gdouble)(originalW * ratio)
    );
    aspect_ratio_ignore = 0;
  }
}

G_MODULE_EXPORT void SaveClick()
{
  if (currentView == VIEW_MON)
    saveMonitor();
  else
    saveApp();
}

G_MODULE_EXPORT
void MonListSelect(GtkListBox *list, GtkListBoxRow *row, gpointer userdata)
{
  reloadWlpList();
  reloadMonSettings();
}

G_MODULE_EXPORT void MainWindowShow()
{
  reloadAppSettings();
  reloadWlpList();

  gtk_list_box_select_row(
      GTK_LIST_BOX(WID(CONTROL_SIDEBAR)),
      gtk_list_box_get_row_at_index(GTK_LIST_BOX(WID(CONTROL_SIDEBAR)), 0)
  );

  gtk_list_box_select_row(
      GTK_LIST_BOX(controls[CONTROL_MON_LIST].widget),
      gtk_list_box_get_row_at_index(
          GTK_LIST_BOX(controls[CONTROL_MON_LIST].widget), 0
      )
  );

  reloadMonSettings();
}

G_MODULE_EXPORT void MainWindowClose()
{
  gtk_widget_set_visible(controls[CONTROL_MAIN_WND].widget, 0);
}

G_MODULE_EXPORT void SidebarSelect(
    GtkListBox *listbox, GtkListBoxRow *row, gpointer userData
)
{
  currentView = gtk_list_box_row_get_index(row);

  char childName[6];
  sprintf(childName, "page%d", currentView);

  gtk_stack_set_visible_child_name(
      GTK_STACK(controls[CONTROL_MAIN_STACK].widget), childName
  );
}
