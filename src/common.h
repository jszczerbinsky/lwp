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
  float sensitivityX;
  float sensitivityY;
} LayerConfig;

typedef struct
{
  int          set;
  int          repeatX;
  int          repeatY;
  int          layersCount;
  LayerConfig *layerConfigs;
} WallpaperConfig;

typedef struct
{
  char            name[WALLPAPER_NAME_MAX];
  char            dirPath[PATH_MAX];
  int             isDefault;
  WallpaperConfig config;
} WallpaperInfo;

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

typedef struct
{
  int drawOnRootWindow;
  int targetFps;
} AppConfig;

MonitorInfo   *scanMonitors(int *count);
WallpaperInfo *scanWallpapers(int *count);

void saveMonitorConfig(const char *name, MonitorConfig *mc);
int  loadMonitorConfig(const char *name, MonitorConfig *mc);

int loadAppConfig(AppConfig *ac);

int loadWallpaperConfig(const char *dirName, WallpaperConfig *wc);

#endif
