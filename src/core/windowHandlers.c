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

G_MODULE_EXPORT void MainWindow_ExitBtnClick() { gtk_widget_set_visible(exitDialog, 1); }

G_MODULE_EXPORT void MainWindowClose() { gtk_widget_set_visible(mainWnd, 0); }

// Wallpapaer Manager Window handlers

G_MODULE_EXPORT void WallpaperManagerWindowClose() { gtk_widget_set_visible(wallpaperMgrWnd, 0); }

G_MODULE_EXPORT void ScreenWindowClose() { gtk_widget_set_visible(screenWnd, 0); }
