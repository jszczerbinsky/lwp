#ifndef COMMON_H
#define COMMON_H

#ifdef __LINUX
#include <linux/limits.h>
#elif __DARWIN
#include <limits.h>
#else
#include <windows.h>
#endif

#define MONITOR_NAME_MAX   100
#define WALLPAPER_NAME_MAX 100

typedef struct
{
  int x;
  int y;
  int w;
  int h;
} Bounds;

typedef struct
{
  int    set;
  char   wlpName[WALLPAPER_NAME_MAX];
  Bounds wlpBounds;
} MonitorConfig;

typedef struct
{
  char          name[MONITOR_NAME_MAX];
  Bounds        bounds;
  MonitorConfig config;
} MonitorInfo;

MonitorInfo *scanMonitors(int *count);

typedef struct
{
  char name[WALLPAPER_NAME_MAX];
  char dirPath[PATH_MAX];
  int  isDefault;
} WallpaperInfo;

WallpaperInfo *scanWallpapers(int *count);

void saveMonitorConfig(const char *name, MonitorConfig *mc);
int  loadMonitorConfig(const char *name, MonitorConfig *mc);

#endif
