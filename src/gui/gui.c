#include "../types/gui.h"
#include "../platform.h"
#include "../types/log.h"
#include "../types/monitor.h"
#include "gtk/gtk.h"

static int isBuilt = 0;

static GtkWidget* build_monitor_listboxitem(AppGUI* gui, const char* name,
											int w, int h) {
	const char* ui_file_path = "./jpaper.ui";

	GtkBuilder* builder = gtk_builder_new_from_file(ui_file_path);

	GtkWidget* item =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_MON_ITEM_NAME);
	GtkWidget* name_label =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_MON_ITEM_LABEL_NAME);
	GtkWidget* res_label = (GtkWidget*)gtk_builder_get_object(
		builder, GUI_MON_ITEM_RES_LABEL_NAME);

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

	gtk_list_box_remove_all(GTK_LIST_BOX(gui->mon_lstbox));

	int		 monitor_count;
	Monitor* monitors = platform_scan_monitors(&monitor_count);

	for (int i = 0; i < monitor_count; i++) {
		printlog(LOG_INFO, NULL, NULL, "Found monitor %s [%dx%d]",
				 monitors[i].display_name, monitors[i].bounds.w,
				 monitors[i].bounds.h);

		GtkWidget* item = build_monitor_listboxitem(
			gui, monitors[i].display_name, 1920, 1080);

		gtk_list_box_insert(GTK_LIST_BOX(gui->mon_lstbox), item, 0);
	}

	free(monitors);
}

static void build(AppGUI* gui) {
	const char* ui_file_path = "./jpaper.ui";
	const char* css_file_path = "./style.css";

	GtkCssProvider* provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(provider, css_file_path);

	gtk_style_context_add_provider_for_display(
		gdk_display_get_default(), GTK_STYLE_PROVIDER(provider),
		GTK_STYLE_PROVIDER_PRIORITY_USER);

	GtkBuilder* builder = gtk_builder_new_from_file(ui_file_path);

	if (builder) {
		printlog(LOG_INFO, NULL, NULL, "GTK builder loaded");
	} else {
		printlog(LOG_ERROR, NULL, NULL, "Couldn't load GTK builder file");
	}

	gui->main_wnd =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_MAIN_WND_NAME);
	gui->mon_lstbox =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_MON_LSTBOX_NAME);
	gui->mon_res_label =
		(GtkWidget*)gtk_builder_get_object(builder, GUI_MON_RES_LABEL_NAME);

	g_object_unref(builder);

	gtk_window_set_application(GTK_WINDOW(gui->main_wnd), gui->gtk_app);

	reload_monitor_listbox(gui);

	gui->gui_built = 1;
}

static void activate(GtkApplication* gtk_app, gpointer userdata) {
	AppGUI* gui = g_object_get_data(G_OBJECT(gtk_app), "app");

	if (gui->gui_built) {
	} else {
		build(gui);
	}
	gtk_widget_set_visible(gui->main_wnd, 1);
}

void gui_init(AppGUI* gui, int argc, char** argv) {
#if GLIB_CHECK_VERSION(2, 74, 0)
	gui->gtk_app = gtk_application_new("com.github.jszczerbinsky.jpaper",
									   G_APPLICATION_DEFAULT_FLAGS);
#else
	gui->gtk_app = gtk_application_new("com.github.jszczerbinsky.jpaper",
									   G_APPLICATION_FLAGS_NONE);
#endif

	g_object_set_data(G_OBJECT(gui->gtk_app), "app", gui);
	g_signal_connect(gui->gtk_app, "activate", G_CALLBACK(activate), NULL);
	g_application_run(G_APPLICATION(gui->gtk_app), argc, argv);
	g_object_unref(gui->gtk_app);
}
