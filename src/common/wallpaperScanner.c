#include <glib.h>
#include <stdio.h>

#include "../common.h"

#define SYSTEM_WALLPAPERS_PATH "/usr/local/share/lwp/wallpapers"

WallpaperInfo *scanWallpapers(int *count)
{
  *count = 0;

  char userWlpPath[PATH_MAX];

  sprintf(userWlpPath, "%s/.config/lwp/wallpapers", g_get_home_dir());

  GDir        *dir;
  GError      *error;
  const gchar *filename;

  dir = g_dir_open(SYSTEM_WALLPAPERS_PATH, 0, &error);
  while ((g_dir_read_name(dir))) (*count)++;
  dir = g_dir_open(userWlpPath, 0, &error);
  while ((g_dir_read_name(dir))) (*count)++;

  WallpaperInfo *list = malloc(sizeof(WallpaperInfo) * (*count));
  WallpaperInfo *ptr  = list;

  dir = g_dir_open(SYSTEM_WALLPAPERS_PATH, 0, &error);
  while ((filename = g_dir_read_name(dir)))
  {
    sprintf(ptr->name, "%s", filename);
    sprintf(ptr->dirPath, "%s/%s", SYSTEM_WALLPAPERS_PATH, filename);
    ptr->isDefault     = 1;
    ptr->config.loaded = 0;
    ptr++;
  }

  dir = g_dir_open(userWlpPath, 0, &error);
  while ((filename = g_dir_read_name(dir)))
  {
    sprintf(ptr->name, "%s", filename);
    sprintf(ptr->dirPath, "%s/%s/%s", g_get_home_dir(), ".config/lwp/wallpapers", filename);
    ptr->isDefault     = 0;
    ptr->config.loaded = 0;
    ptr++;
  }

  return list;
}
