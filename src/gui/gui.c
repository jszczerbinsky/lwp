#include "../types/gui.h"
#include "../types/log.h"
#include "gtk/gtk.h"

AppGUI* g_gui = NULL;

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

	main_wnd_build(gui, builder);
	wlp_wnd_build(gui, builder);

	g_object_unref(builder);

	gtk_window_set_application(GTK_WINDOW(gui->main_wnd.wnd), gui->gtk_app);

	gui->gui_built = 1;
}

static void activate(GtkApplication* gtk_app, gpointer userdata) {
	AppGUI* gui = g_object_get_data(G_OBJECT(gtk_app), "app");

	if (gui->gui_built) {
	} else {
		build(gui);
	}
	gtk_widget_set_visible(gui->main_wnd.wnd, 1);
}

void gui_init(AppGUI* gui, int argc, char** argv) {
	g_gui = gui;
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

G_MODULE_EXPORT gboolean Window_HideOnClose(GtkWindow* window,
											gpointer   user_data) {
	gtk_widget_set_visible(GTK_WIDGET(window), 0);
	return 1;
}
