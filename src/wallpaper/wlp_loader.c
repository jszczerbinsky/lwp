#include <glib.h>
#include <stdlib.h>

#include "../platform.h"
#include "../types/log.h"
#include "../types/wlp.h"

int wlp_load_info(WlpInfo* info, const char* dir_path) {

	char config_path[PATH_MAX];

	sprintf(config_path, "%s" DIR_SEP "wallpaper.xml", dir_path);

	if (!wlpcfg_validate(config_path)) {
		printlog(LOG_WARNING, NULL, NULL,
				 "Failed to load wallpaper info from %s", config_path);
		return 0;
	}

	printlog(LOG_DEBUG, NULL, NULL, "Config file %s is valid", config_path);

	xmlDocPtr  doc = xmlReadFile(config_path, NULL, 0);
	xmlNodePtr root = xmlDocGetRootElement(doc);

	xmlNodePtr child = root->children;
	while (child) {
		if (xmlStrcmp(child->name, BAD_CAST "info") == 0) {
			wlpcfg_parse_info(info, child, dir_path);
		}
		child = child->next;
	}
	xmlFreeDoc(doc);
	return 1;
}

void wlp_free_info_data(WlpInfo* info) {
	free(info->author);
	free(info->name);
	free(info->dir_path);
}

int wlp_load_to_instance(WlpInstance* inst, const char* dir_path) {
	char config_path[PATH_MAX] =
		"/home/cziken/.config/lwp/testwlp/wallpaper.xml";

	if (!wlpcfg_validate(config_path)) {
		printlog(LOG_WARNING, NULL, NULL,
				 "Failed to load wallpaper info from %s", config_path);
		return 0;
	}

	printlog(LOG_DEBUG, NULL, NULL, "Config file %s is valid", config_path);

	xmlDocPtr  doc = xmlReadFile(config_path, NULL, 0);
	xmlNodePtr root = xmlDocGetRootElement(doc);

	wlpcfg_parse_wallpaper(inst, dir_path, root);

	xmlFreeDoc(doc);
	return 1;
}

void wlp_subscan(int* count, WlpInfo** infos, const char* dir_path) {
	printlog(LOG_DEBUG, NULL, NULL, "Searching for wallpapers in %s", dir_path);

	GError* error = NULL;
	GDir*	dir = g_dir_open(dir_path, 0, &error);

	if (!dir) {
		return;
	}

	const char* dir_name;
	while ((dir_name = g_dir_read_name(dir)) != NULL) {
		gchar* config_path =
			g_build_filename(dir_path, dir_name, "wallpaper.xml", NULL);
		gchar* wlp_path = g_build_filename(dir_path, dir_name, NULL);

		if (g_file_test(wlp_path, G_FILE_TEST_IS_DIR)) {
			if (wlpcfg_validate(config_path)) {

				(*count)++;
				*infos = realloc(*infos, (*count) * sizeof(WlpInfo));
				wlp_load_info((*infos) + (*count) - 1, wlp_path);

			} else {
				printlog(LOG_WARNING, NULL, NULL,
						 "Wallpaper directory %s was found, but validation of "
						 "wallpaper.xml has failed",
						 wlp_path);
			}
		} else {
			printlog(
				LOG_WARNING, NULL, NULL,
				"Unexpected file %s has been found in wallpapers directory %s",
				wlp_path, dir_path);
		}
		g_free(wlp_path);
		g_free(config_path);
	}

	g_dir_close(dir);
}