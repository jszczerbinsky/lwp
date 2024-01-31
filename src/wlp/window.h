#ifndef WINDOW_H
#define WINDOW_H

#include "main.h"
#include "parser.h"

#ifdef __WIN32
void initTrayIcon();
int  updateTrayIcon();
void removeTrayIcon();
#endif

#endif
