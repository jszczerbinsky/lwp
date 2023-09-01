#include <gtk/gtk.h>
#include <stdio.h>

static void apply_btn_click(GtkWidget *widget, gpointer data) {}
static void close_btn_click(GtkWidget *widget, gpointer data)
{
  g_application_quit(G_APPLICATION(data));
}

static void activate(GtkApplication *app, gpointer user_data)
{
  GtkWidget *window;
  GtkWidget *box;
  GtkWidget *btn_box;
  GtkWidget *version_label;
  GtkWidget *apply_btn;
  GtkWidget *close_btn;

  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "LayeredWallpaper Control Panel");
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

  char version_label_str[1024];
  sprintf(version_label_str, "Layered WallPaper version: %s", VER);

  version_label = gtk_label_new(version_label_str);
  box           = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  btn_box       = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_window_set_child(GTK_WINDOW(window), box);

  gtk_box_append(GTK_BOX(box), version_label);

  apply_btn = gtk_button_new_with_label("Apply");
  close_btn = gtk_button_new_with_label("Close");
  g_signal_connect(apply_btn, "clicked", G_CALLBACK(apply_btn_click), NULL);
  g_signal_connect(close_btn, "clicked", G_CALLBACK(close_btn_click), app);
  gtk_box_append(GTK_BOX(btn_box), apply_btn);
  gtk_box_append(GTK_BOX(btn_box), close_btn);

  gtk_box_append(GTK_BOX(box), btn_box);

  gtk_window_present(GTK_WINDOW(window));
}

int initConfigWindow()
{
  int   argc = 0;
  char *argv[1];

  GtkApplication *app;
  int             status;

  app = gtk_application_new("com.github.jszczerbinsky.lwp", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
