#include <glib.h>
#include <stdio.h>

#include "../common.h"

#define WALLPAPERS_SYSTEM 0
#define WALLPAPERS_USER   1

WallpaperInfo *scanWallpapers(int *count)
{
  *count = 0;

#ifdef __WIN32
  const char *format = "%s\\%s";
#else
  const char *format = "%s/%s";
#endif

  char userWlpPath[PATH_MAX];
  getAppDir(userWlpPath, APP_DIR_USER_SETTINGS);
  sprintf(userWlpPath, format, userWlpPath, "wallpapers");

  char systemWlpPath[PATH_MAX];
  getAppDir(systemWlpPath, APP_DIR_SHARE);
  sprintf(systemWlpPath, format, systemWlpPath, "wallpapers");

  GDir        *dir;
  GError      *error;
  const gchar *filename;

  dir = g_dir_open(systemWlpPath, 0, &error);
  while ((g_dir_read_name(dir))) (*count)++;
  dir = g_dir_open(userWlpPath, 0, &error);
  while ((g_dir_read_name(dir))) (*count)++;

  WallpaperInfo *list = malloc(sizeof(WallpaperInfo) * (*count));
  WallpaperInfo *ptr  = list;

  dir = g_dir_open(systemWlpPath, 0, &error);
  while ((filename = g_dir_read_name(dir)))
  {
    sprintf(ptr->name, "%s", filename);
#ifdef __WIN32
    sprintf(ptr->dirPath, "%s\\%s", systemWlpPath, filename);
#else
    sprintf(ptr->dirPath, "%s/%s", systemWlpPath, filename);
#endif
    ptr->isDefault     = 1;
    ptr->config.loaded = 0;
    ptr++;
  }

  dir = g_dir_open(userWlpPath, 0, &error);
  while ((filename = g_dir_read_name(dir)))
  {
    sprintf(ptr->name, "%s", filename);
#ifdef __WIN32
    sprintf(ptr->dirPath, "%s\\%s", userWlpPath, filename);
#else
    sprintf(ptr->dirPath, "%s/%s", userWlpPath, filename);
#endif
    ptr->isDefault     = 0;
    ptr->config.loaded = 0;
    ptr++;
  }

  return list;
}
