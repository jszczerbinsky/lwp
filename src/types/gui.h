#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

#define GUI_MAINWND					   "MainWindow"
#define GUI_MAINWND_MON_LSTBOX		   "MainWindow.MonitorListBox"
#define GUI_MAINWND_MON_RES_LABEL	   "MainWindow.MonitorResolutionLabel"
#define GUI_MAINWND_MON_ITEM		   "MainWindow.MonitorItem"
#define GUI_MAINWND_MON_ITEM_LABEL	   "MainWindow.MonitorItemLabel"
#define GUI_MAINWND_MON_ITEM_RES_LABEL "MainWindow.MonitorItemResLabel"
#define GUI_MAINWND_VERSION_LABEL	   "MainWindow.VersionLabel"

#define GUI_WLPWND				  "WallpaperWindow"
#define GUI_WLPWND_WLP_FLOWBOX	  "WallpaperWindow.WallpaperFlowBox"
#define GUI_WLPWND_WLP_ITEM		  "WallpaperWindow.WallpaperItem"
#define GUI_WLPWND_WLP_ITEM_LABEL "WallpaperWindow.WallpaperItemLabel"
#define GUI_WLPWND_WLP_ITEM_IMG	  "WallpaperWindow.WallpaperItemImg"
#define GUI_WLPWND_SIDEBAR		  "WallpaperWindow.SideBar"
#define GUI_WLPWND_SEL_WLP_NAME_LABEL                                          \
	"WallpaperWindow.SelectedWallpaperNameLabel"
#define GUI_WLPWND_SEL_WLP_AUTHOR_LABEL                                        \
	"WallpaperWindow.SelectedWallpaperAuthorLabel"
#define GUI_WLPWND_SEL_WLP_IMG "WallpaperWindow.SelectedWallpaperImg"
#define GUI_WLPWND_SELECT_BTN  "WallpaperWindow.SelectBtn"

typedef struct {
	GtkWidget* wnd;
	GtkWidget* mon_lstbox;
	GtkWidget* mon_res_label;
} MainWnd;

typedef struct {
	GtkWidget* wnd;
	GtkWidget* wlp_flowbox;
	GtkWidget* sidebar;
	GtkWidget* selwlp_name_label;
	GtkWidget* selwlp_author_label;
	GtkWidget* selwlp_img;
	GtkWidget* select_btn;
} WlpWnd;

typedef struct {
	int gui_built;

	GtkApplication* gtk_app;

	MainWnd main_wnd;
	WlpWnd	wlp_wnd;
} AppGUI;

typedef struct {
	AppGUI gui;
} App;

extern AppGUI* g_gui;

void gui_init(AppGUI* app, int argc, char** argv);

void main_wnd_build(AppGUI* gui, GtkBuilder* builder);
void wlp_wnd_build(AppGUI* gui, GtkBuilder* builder);

#endif
