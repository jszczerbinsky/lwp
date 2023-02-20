#ifndef PARSER_H
#define PARSER_H

#include "wallpaper.h"

typedef struct
{
  int      reloadRootWnd;
  int      monitorsCount;
  float    smooth;
  Monitor *monitors;
} Config;

int  parseConfig(App *app, Config *cfg);
int  parseWallpaperConfig(Wallpaper *wallpaper, const char *path);
void freeConfig(Config *cfg);

#endif
