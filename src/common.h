#ifndef COMMON_H
#define COMMON_H

#ifdef __LINUX
#include <linux/limits.h>
#else
#include <windows.h>
#endif

#define MONITOR_NAME_MAX   128
#define WALLPAPER_NAME_MAX 100

#define DEFAULT_LINUX_PREFIX "/usr/local"

#define LOG_ERROR   0
#define LOG_INFO    1
#define LOG_WARNING 2

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
  int          loaded;
  int          repeatX;
  int          repeatY;
  int          layersCount;
  LayerConfig* layerConfigs;
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
  int    loaded;
  int    active;
  char   wlpName[WALLPAPER_NAME_MAX];
  Bounds wlpBounds;
} MonitorConfig;

typedef struct
{
  char          displayName[MONITOR_NAME_MAX];
  char          name[MONITOR_NAME_MAX];
  Bounds        pixelBounds;
  Bounds        clientBounds;
  Bounds        virtualBounds;
  MonitorConfig config;
} MonitorInfo;

typedef struct
{
  int  targetFps;
  char renderQuality[8];
  int  unfocusedComeback;
  int  wndTargetPoint;
} AppConfig;

//
// paths.c
//

#define APP_DIR_BIN           0
#define APP_DIR_SHARE         1
#define APP_DIR_USER_SETTINGS 2

void getAppDir(char* buff, int type);

void getMonitorCfgPath(char* buff, const char* name);
void getWlpCfgPath(char* buff, const char* dirPath);
void getAppCfgPath(char* buff);
void getLogPath(char* buff);

void createUserDirs();

//
// monitorScanner.c
//

MonitorInfo* scanMonitors(int* count);

//
// wallpaperScanner.c
//

WallpaperInfo* scanWallpapers(int* count);

//
// config.c
//

void saveMonitorConfig(const char* name, MonitorConfig* mc);
int  loadMonitorConfig(const char* name, MonitorConfig* mc);

int  loadAppConfig(AppConfig* ac);
void saveAppConfig(AppConfig* ac);

int loadWallpaperConfig(const char* dirName, WallpaperConfig* wc);

//
// debug.c
//
void  lwpLog(int type, const char* str, ...);
char* readLogFile();

#endif
