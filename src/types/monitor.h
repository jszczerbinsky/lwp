#ifndef MONITOR_H
#define MONITOR_H

#include "geometry.h"

#define MONITOR_NAME_MAX 256

typedef struct {
	char name[MONITOR_NAME_MAX];
	char display_name[MONITOR_NAME_MAX];

	Bounds bounds;
} Monitor;

#endif
