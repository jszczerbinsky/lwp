#ifndef PARSER_H
#define PARSER_H

#include "main.h"

typedef struct
{
	char path[PATH_MAX];
	float smooth;
	float movementX;
	float movementY;
	int monitors;
	int count;
} Config;

void openConfig();
void closeConfig();
int parseConfig(Config *cfg);
void parseInstancesConfig(Instance *instances, int instancesCount);

#endif
