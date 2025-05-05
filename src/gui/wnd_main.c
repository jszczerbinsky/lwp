#include "../platform.h"

#include "../types/gui.h"
#include "../types/log.h"
#include "../types/monitor.h"
#include "gmodule.h"
#include "gtk/gtk.h"
#include "gtk/gtkshortcut.h"

static GtkWidget* build_monitor_listboxitem(AppGUI* gui, const char* name,
											int w, int h) {
	const char* ui_file_path = "./jpaper.ui";

	GtkBuilder* builder = gtk_builder_new_from_file(ui_file_path);

	GtkWidget* item =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_MAINWND_MON_ITEM);
	GtkWidget* name_label =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_MAINWND_MON_ITEM_LABEL);
	GtkWidget* res_label = (GtkWidget*)gtk_builder_get_object(
		builder, GUI_MAINWND_MON_ITEM_RES_LABEL);

	char res_str[15];
	sprintf(res_str, "%dx%d", w, h);

	gtk_widget_unparent(item);
	gtk_label_set_text(GTK_LABEL(name_label), name);
	gtk_label_set_text(GTK_LABEL(res_label), res_str);

	g_object_ref_sink(item);
	g_object_unref(builder);
	return item;
}

static void reload_monitor_listbox(AppGUI* gui) {
	printlog(LOG_INFO, NULL, NULL, "Reloading monitors...");

	gtk_list_box_remove_all(GTK_LIST_BOX(gui->main_wnd.mon_lstbox));

	int		 monitor_count;
	Monitor* monitors = platform_scan_monitors(&monitor_count);

	for (int i = 0; i < monitor_count; i++) {
		printlog(LOG_INFO, NULL, NULL, "Found monitor %s [%dx%d]",
				 monitors[i].display_name, monitors[i].bounds.w,
				 monitors[i].bounds.h);

		GtkWidget* item = build_monitor_listboxitem(
			gui, monitors[i].display_name, 1920, 1080);

		gtk_list_box_insert(GTK_LIST_BOX(gui->main_wnd.mon_lstbox), item, 0);
	}

	free(monitors);
}

void main_wnd_build(AppGUI* gui, GtkBuilder* builder) {
	gui->main_wnd.wnd =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_MAINWND);
	gui->main_wnd.mon_lstbox =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_MAINWND_MON_LSTBOX);
	gui->main_wnd.mon_res_label =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_MAINWND_MON_RES_LABEL);

	GtkWidget* version_label =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_MAINWND_VERSION_LABEL);

	char version_str[100] = "JPaper ";
	strcat(version_str, PROGRAM_VERSION);
	gtk_label_set_text(GTK_LABEL(version_label), version_str);

	reload_monitor_listbox(gui);
}

G_MODULE_EXPORT void MainWindow_OnWallpapersBtnClicked(GtkButton* button,
													   gpointer	  user_data) {
	gtk_widget_set_visible(g_gui->wlp_wnd.select_btn, 0);
	gtk_widget_set_visible(g_gui->wlp_wnd.wnd, 1);
}
