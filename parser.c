#include "parser.h"

#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "wallpaper.h"

#define TYPE_FLOAT 0
#define TYPE_INT   1
#define TYPE_STR   2

#ifdef __WIN32
static void getline(char **buff, size_t *buffSize, FILE *f)
{
  long int startPos = ftell(f);
  int      length   = 1;
  char     c;
  do
  {
    c = fgetc(f);
    length++;
  } while (c != '\n' && !feof(f));

  if (*buffSize < length)
  {
    *buff     = realloc(*buff, length * sizeof(char));
    *buffSize = length * sizeof(char);
  }

  fseek(f, startPos, SEEK_SET);

  fgets(*buff, length - 1, f);
  (*buff)[length - 1] = '\0';
  fgetc(f);
}
#endif

static FILE *openConfigFile()
{
  FILE *f;
  char  userConfigPath[PATH_MAX];

#ifdef __WIN32
  strcpy(userConfigPath, getenv("AppData"));
  strcat(userConfigPath, "\\lwp\\lwp.cfg");
#else
  char *xdgConfigHome = getenv("XDG_CONFIG_HOME");
  if (xdgConfigHome)
  {
    strcpy(userConfigPath, xdgConfigHome);
    strcat(userConfigPath, "/lwp/lwp.cfg");
  }
  else
  {
    struct passwd *pw = getpwuid(getuid());
    strcpy(userConfigPath, pw->pw_dir);
    strcat(userConfigPath, "/.config/lwp/lwp.cfg");
  }
#endif

  f = fopen(userConfigPath, "r");
  if (!f)
  {
    lwpLog(LOG_INFO, "User config file not found, opening default config instead");
    char defaultConfigPath[PATH_MAX];
#ifdef __WIN32
    GetModuleFileNameA(NULL, defaultConfigPath, PATH_MAX);
    char *ptr = defaultConfigPath + strlen(defaultConfigPath) - 1;
    while (*ptr != '\\') ptr--;
    *ptr = '\0';
    strcat(defaultConfigPath, "\\defaultWin.cfg");

    f = fopen(defaultConfigPath, "r");
#else
    char *sysConfDir = getenv("sysconfdir");
    if (sysConfDir)
    {
      strcpy(defaultConfigPath, sysConfDir);
      strcat(defaultConfigPath, "/lwp.cfg");
    }
    else
      strcpy(defaultConfigPath, "/etc/lwp.cfg");

    f = fopen(defaultConfigPath, "r");
#endif
    if (!f) lwpLog(LOG_ERROR, "Default config file not found!");
  }

  return f;
}

static int findLine(FILE *f, const char *name, int type, void *output)
{
  char  *buff     = NULL;
  size_t buffSize = 0;

  int found = 0;

  fseek(f, 0, SEEK_SET);

  do
  {
    getline(&buff, &buffSize, f);

    if (buffSize > 0 && buff[0] != '#')
    {
      if (strncmp(name, buff, strlen(name)) == 0)
      {
        char *valuePtr = buff;
        while (*valuePtr != '=' && *valuePtr != '\0')
        {
          valuePtr++;
        }

        if (*valuePtr == '=')
        {
          valuePtr++;

          if (valuePtr[strlen(valuePtr) - 1] == '\n') valuePtr[strlen(valuePtr) - 1] = '\0';

          found = 1;

          switch (type)
          {
            case TYPE_INT:
              *(int *)output = atoi(valuePtr);
              break;
            case TYPE_FLOAT:
              *(float *)output = atof(valuePtr);
              break;
            case TYPE_STR:
              strcpy((char *)output, valuePtr);
              break;
          }
        }
      }
    }
  } while (!found && buffSize > 0 && !feof(f));

  free(buff);
  return found;
}

int parseConfig(App *app, Config *cfg)
{
  FILE *f = openConfigFile();

  if (!findLine(f, "monitors", TYPE_INT, &cfg->monitorsCount))
  {
    lwpLog(LOG_ERROR, "Can't find line 'monitors' in config");
    return 0;
  }

  if (!findLine(f, "smooth", TYPE_FLOAT, &cfg->smooth))
  {
    lwpLog(LOG_INFO, "Can't find line 'smooth' in config, setting to default value");
    cfg->smooth = 8;
  }

#ifndef __WIN32
  if (!findLine(f, "reload_rootwindow", TYPE_INT, &cfg->monitors))
  {
    lwpLog(LOG_ERROR, "Can't find line 'reload_rootwindow' in config");
    return 0;
  }
#endif

  cfg->monitors = malloc(cfg->monitorsCount * sizeof(Monitor));

  for (int m = 0; m < cfg->monitorsCount; m++)
  {
    char wallpaperPath[PATH_MAX];

    const int PARAMS_COUNT = 9;

    const char *paramStr[] = {"wallpaper",   "x",           "y",           "w",          "h",
                              "wallpaper_x", "wallpaper_y", "wallpaper_w", "wallpaper_h"};
    void       *outputs[]  = {
        wallpaperPath,
        &cfg->monitors[m].x,
        &cfg->monitors[m].y,
        &cfg->monitors[m].w,
        &cfg->monitors[m].h,
        &cfg->monitors[m].wallpaperX,
        &cfg->monitors[m].wallpaperY,
        &cfg->monitors[m].wallpaperW,
        &cfg->monitors[m].wallpaperH,
    };

    char str[100];

    for (int p = 0; p < PARAMS_COUNT; p++)
    {
      snprintf(str, 100, "monitor%d_%s", m + 1, paramStr[p]);
      if (!findLine(f, str, (p == 0 ? TYPE_STR : TYPE_INT), outputs[p]))
      {
        lwpLog(LOG_ERROR, "Can't find line '%s' in config", str);
        return 0;
      }
    }

#ifdef __WIN32
    if (strlen(wallpaperPath) < 2)
    {
      GetModuleFileNameA(NULL, wallpaperPath, PATH_MAX);
      char *ptr = wallpaperPath + strlen(wallpaperPath) - 1;
      while (*ptr != '\\') ptr--;
      *ptr = '\0';
      strcat(wallpaperPath, "\\wallpapers\\default-fullhd");
    }
#endif

    strncpy(cfg->monitors[m].wallpaper.dirPath, wallpaperPath, PATH_MAX);
  }

  fclose(f);
  return 1;
}

int parseWallpaperConfig(Wallpaper *wallpaper, const char *path)
{
  FILE *f = fopen(path, "r");
  if (!f)
  {
    lwpLog(LOG_ERROR, "Wallpaper config file (%s) doesn't exist", path);
    return 0;
  }

  float defaultMovementX, defaultMovementY;

  const int   PARAMS_COUNT = 5;
  const char *paramStr[]   = {"count", "movement_x", "movement_y", "repeat_x", "repeat_y"};
  const int   types[]      = {TYPE_INT, TYPE_FLOAT, TYPE_FLOAT, TYPE_INT, TYPE_INT};
  void       *outputs[]    = {
      &wallpaper->layersCount, &defaultMovementX,   &defaultMovementY,
      &wallpaper->repeatX,     &wallpaper->repeatY,
  };

  for (int p = 0; p < PARAMS_COUNT; p++)
  {
    if (!findLine(f, paramStr[p], types[p], outputs[p]))
    {
      lwpLog(LOG_ERROR, "Can't find line '%s' in config", paramStr[p]);
      return 0;
    }
  }
  wallpaper->layers = malloc(wallpaper->layersCount * sizeof(Layer));

  char str[100];
  for (int l = 0; l < wallpaper->layersCount; l++)
  {
    snprintf(str, 100, "movement%d_x", l + 1);
    if (!findLine(f, str, TYPE_FLOAT, &wallpaper->layers[l].sensitivityX))
      wallpaper->layers[l].sensitivityX = defaultMovementX * l;
    snprintf(str, 100, "movement%d_y", l + 1);
    if (!findLine(f, str, TYPE_FLOAT, &wallpaper->layers[l].sensitivityY))
      wallpaper->layers[l].sensitivityY = defaultMovementY * l;
  }

  fclose(f);

  return 1;
}

void freeConfig(Config *cfg)
{
  for (int i = 0; i < cfg->monitorsCount; i++) freeMonitor(&cfg->monitors[i]);
  free(cfg->monitors);
}
