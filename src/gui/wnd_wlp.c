#include "../types/gui.h"
#include "../types/log.h"
#include "../types/wlp.h"

#include "../platform.h"
#include "glib-object.h"
#include "glib.h"
#include "gtk/gtk.h"

static GtkWidget* build_wallpaper_item(AppGUI* gui, const WlpInfo* info) {
	const char* ui_file_path = "./jpaper.ui";

	GtkBuilder* builder = gtk_builder_new_from_file(ui_file_path);

	GtkWidget* item =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_WLPWND_WLP_ITEM);
	GtkWidget* name_label =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_WLPWND_WLP_ITEM_LABEL);
	GtkWidget* img =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_WLPWND_WLP_ITEM_IMG);

	char thumb_path[PATH_MAX];
	sprintf(thumb_path, "%s" DIR_SEP "%s", info->dir_path, "thumbnail.jpg");

	gtk_image_set_from_file(GTK_IMAGE(img), thumb_path);

	gtk_widget_unparent(item);
	gtk_label_set_text(GTK_LABEL(name_label), info->name);

	g_object_ref_sink(item);
	g_object_unref(builder);
	return item;
}

static void reload_wallpaper_flowbox(AppGUI* gui) {
	printlog(LOG_INFO, NULL, NULL, "Reloading wallpapers...");

	gtk_flow_box_remove_all(GTK_FLOW_BOX(gui->wlp_wnd.wlp_flowbox));

	WlpInfo* wlps = g_object_get_data(G_OBJECT(gui->wlp_wnd.wnd), "wallpapers");
	if (wlps) {
		wlp_free_info_data(wlps);
		free(wlps);
	}

	int wlp_count;
	wlps = platform_scan_wlps(&wlp_count);

	printlog(LOG_DEBUG, NULL, NULL, "Scanner found %d wallpapers", wlp_count);

	g_object_set_data(G_OBJECT(gui->wlp_wnd.wnd), "wallpapers", wlps);

	g_object_set_data(G_OBJECT(gui->wlp_wnd.wnd), "wallpapers_count",
					  GINT_TO_POINTER(wlp_count));

	for (int i = 0; i < wlp_count; i++) {
		printlog(LOG_INFO, NULL, NULL, "Found wallpaper %s by %s", wlps[i].name,
				 wlps[i].author);

		GtkWidget* item = build_wallpaper_item(gui, wlps + i);
		gtk_flow_box_insert(GTK_FLOW_BOX(gui->wlp_wnd.wlp_flowbox), item, i);
	}
}

static void reload_side_panel(AppGUI* gui) {
	GList* selected_list = gtk_flow_box_get_selected_children(
		GTK_FLOW_BOX(gui->wlp_wnd.wlp_flowbox));

	if (selected_list == NULL) {
		gtk_widget_set_visible(gui->wlp_wnd.sidebar, 0);
	} else {
		gtk_widget_set_visible(gui->wlp_wnd.sidebar, 1);
		GtkFlowBoxChild* item = selected_list->data;

		int item_index = gtk_flow_box_child_get_index(item);

		WlpInfo* wlps =
			g_object_get_data(G_OBJECT(gui->wlp_wnd.wnd), "wallpapers");
		WlpInfo* selected_wlp = wlps + item_index;

		gtk_label_set_text(GTK_LABEL(gui->wlp_wnd.selwlp_name_label),
						   selected_wlp->name);
		gtk_label_set_text(GTK_LABEL(gui->wlp_wnd.selwlp_author_label),
						   selected_wlp->author);

		char thumb_path[PATH_MAX];
		sprintf(thumb_path, "%s" DIR_SEP "%s", selected_wlp->dir_path,
				"thumbnail.jpg");
		gtk_image_set_from_file(GTK_IMAGE(gui->wlp_wnd.selwlp_img), thumb_path);

		g_list_free(selected_list);
	}
}

void wlp_wnd_build(AppGUI* gui, GtkBuilder* builder) {
	gui->wlp_wnd.wnd = (GtkWidget*)gtk_builder_get_object(builder, GUI_WLPWND);
	gui->wlp_wnd.wlp_flowbox =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_WLPWND_WLP_FLOWBOX);
	gui->wlp_wnd.selwlp_name_label = (GtkWidget*)gtk_builder_get_object(
		builder, GUI_WLPWND_SEL_WLP_NAME_LABEL);
	gui->wlp_wnd.selwlp_author_label = (GtkWidget*)gtk_builder_get_object(
		builder, GUI_WLPWND_SEL_WLP_AUTHOR_LABEL);
	gui->wlp_wnd.selwlp_img =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_WLPWND_SEL_WLP_IMG);
	gui->wlp_wnd.sidebar =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_WLPWND_SIDEBAR);
	gui->wlp_wnd.select_btn =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_WLPWND_SELECT_BTN);

	g_object_set_data(G_OBJECT(gui->wlp_wnd.wlp_flowbox), "gui", gui);

	reload_wallpaper_flowbox(gui);
	reload_side_panel(gui);
}

G_MODULE_EXPORT void WallpaperWindow_OnWlpSelected(GtkFlowBox* box,
												   gpointer	   user_data) {
	reload_side_panel(g_gui);
}
