#include <gtk/gtk.h>

#ifdef __WIN32
    #define MAIN_WINDOW_TEMPLATE_PATH ""
#elif __DARWIN
    #define MAIN_WINDOW_TEMPLATE_PATH ""
#else
    #define MAIN_WINDOW_TEMPLATE_PATH "/usr/local/share/lwp/window_templates/main.glade"
#endif

GtkBuilder *builder = NULL;
static FILE *wlpProcess = NULL;
int wlpPid = 0;

static void killWlp()
{
    if(wlpPid != 0)
        kill(wlpPid, SIGINT);
    if(wlpProcess != NULL)
        pclose(wlpProcess);

    wlpPid = 0;
    wlpProcess = NULL;
}

static void runWlp()
{
    wlpProcess = popen("/home/cziken/Projects/lwp/build/src/wlp/lwpwlp", "r");
    char buff[10];
    fgets(buff, sizeof(buff) - 1, wlpProcess);
    wlpPid = atoi(buff);
}

static gboolean exitClicked(GtkWidget *btn, GdkEvent *event, gpointer userdata)
{
    killWlp();
    g_application_quit(G_APPLICATION(userdata));
}

static gboolean deleted(GtkWidget *window, GdkEvent *event, gpointer userdata)
{
    gtk_widget_hide(window);
}

static void activate(GtkApplication *app, gpointer userdata)
{
    builder = gtk_builder_new_from_file(MAIN_WINDOW_TEMPLATE_PATH);
    GtkWidget  *window  = (GtkWidget *)gtk_builder_get_object(builder, "MainWindow");
    if (window == NULL) printf("wnd not found\n");

    g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(deleted), NULL);

    GtkWidget *exitBtn = (GtkWidget *)gtk_builder_get_object(builder, "MainWindow_ExitBtn");

    g_signal_connect(G_OBJECT(exitBtn), "clicked", G_CALLBACK(exitClicked), app);

    gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));
    gtk_widget_set_visible(window, 1);

    runWlp();
}

int main(int argc, char *argv[])
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("com.github.jszczerbinsky.lwp", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}