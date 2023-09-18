#ifndef PARSER_H
#define PARSER_H

#include "main.h"

int  parseConfig(App *app, Config *cfg);
int  parseWallpaperConfig(Wallpaper *wallpaper, const char *path);
void freeConfig(Config *cfg);

#endif
