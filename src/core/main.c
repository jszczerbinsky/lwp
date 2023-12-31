#include "main.h"

#ifdef __WIN32
    #define MAIN_WINDOW_TEMPLATE_PATH ""
#elif __DARWIN
    #define MAIN_WINDOW_TEMPLATE_PATH ""
#else
    #define MAIN_WINDOW_TEMPLATE_PATH "/usr/local/share/lwp/window_templates/main.glade"
#endif

GtkApplication *app = NULL;
GtkBuilder *builder = NULL;

GtkWidget  *mainWnd = NULL;
GtkWidget *exitDialog = NULL;
GtkWidget *wallpaperMgrWnd = NULL;
GtkWidget *screenWnd = NULL;

static FILE *wlpProcess = NULL;
static int wlpPid = 0;

void killWlp()
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

static void activate(GtkApplication *app, gpointer userdata)
{
    static int alreadyRunning = 0;

    if(!alreadyRunning)
    {
        alreadyRunning = 1;

        builder = gtk_builder_new_from_file(MAIN_WINDOW_TEMPLATE_PATH);
        gtk_builder_connect_signals(builder, NULL);
        
        mainWnd = (GtkWidget *)gtk_builder_get_object(builder, "MainWindow");
        exitDialog = (GtkWidget *)gtk_builder_get_object(builder, "ExitDialog");
        wallpaperMgrWnd = (GtkWidget *)gtk_builder_get_object(builder, "WallpaperManagerWindow");
        screenWnd = (GtkWidget *)gtk_builder_get_object(builder, "ScreenWindow");

        gtk_window_set_application(GTK_WINDOW(mainWnd), GTK_APPLICATION(app));
        gtk_window_set_application(GTK_WINDOW(exitDialog), GTK_APPLICATION(app));
        gtk_window_set_application(GTK_WINDOW(wallpaperMgrWnd), GTK_APPLICATION(app));
        gtk_window_set_application(GTK_WINDOW(screenWnd), GTK_APPLICATION(app));
        runWlp();
    }

    gtk_widget_set_visible(mainWnd, 1);
}

int main(int argc, char *argv[])
{
    int status;

    Monitor *monitors = scanMonitors();
    free(monitors);

    app = gtk_application_new ("com.github.jszczerbinsky.lwp", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}