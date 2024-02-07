#include <glib.h>
#include <stdio.h>

#include "../common.h"

#define WALLPAPERS_SYSTEM 0
#define WALLPAPERS_USER 1

static void getWallpapersDirPath(char *path, int type) {
#ifdef __WIN32
  if (type == WALLPAPERS_USER)
    sprintf(path, "%s\\lwp\\wallpapers", g_get_user_data_dir());
  else {
    char currentDirPath[PATH_MAX];
    GetModuleFileNameA(NULL, currentDirPath, PATH_MAX);
    char *ptr = currentDirPath+strlen(currentDirPath)-1;
    while(*ptr != '\\')
      ptr--;
    *ptr = '\0';
    sprintf(path, "%s\\wallpapers", currentDirPath);
  }
#else
  if (type == WALLPAPERS_USER)
    sprintf(path, "%s/.config/lwp/wallpapers", g_get_home_dir());
  else
    sprintf(path, "/usr/local/share/lwp/wallpapers");
#endif
}

WallpaperInfo *scanWallpapers(int *count) {
  *count = 0;

  char userWlpPath[PATH_MAX];
  getWallpapersDirPath(userWlpPath, WALLPAPERS_USER);
  char systemWlpPath[PATH_MAX];
  getWallpapersDirPath(systemWlpPath, WALLPAPERS_SYSTEM);

  GDir *dir;
  GError *error;
  const gchar *filename;

  dir = g_dir_open(systemWlpPath, 0, &error);
  while ((g_dir_read_name(dir)))
    (*count)++;
  dir = g_dir_open(userWlpPath, 0, &error);
  while ((g_dir_read_name(dir)))
    (*count)++;

  WallpaperInfo *list = malloc(sizeof(WallpaperInfo) * (*count));
  WallpaperInfo *ptr = list;

  dir = g_dir_open(systemWlpPath, 0, &error);
  while ((filename = g_dir_read_name(dir))) {
    sprintf(ptr->name, "%s", filename);
#ifdef __WIN32
    sprintf(ptr->dirPath, "%s\\%s", systemWlpPath, filename);

#else
    sprintf(ptr->dirPath, "%s/%s", systemWlpPath, filename);
#endif
    ptr->isDefault = 1;
    ptr->config.loaded = 0;
    ptr++;
  }

  dir = g_dir_open(userWlpPath, 0, &error);
  while ((filename = g_dir_read_name(dir))) {
    sprintf(ptr->name, "%s", filename);
#ifdef __WIN32
    sprintf(ptr->dirPath, "%s\\%s", userWlpPath, filename);
#else
    sprintf(ptr->dirPath, "%s/%s", userWlpPath, filename);
#endif
    ptr->isDefault = 0;
    ptr->config.loaded = 0;
    ptr++;
  }

  return list;
}
