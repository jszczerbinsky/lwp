#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "debug.h"

#define TYPE_FLOAT 0
#define TYPE_INT 1
#define TYPE_STR 2

FILE *file;

#ifdef __WIN32
	void getline(char **buff, size_t *buffSize, FILE *f)
	{
		long int startPos = ftell(f);
		int length = 1;
		char c;
		do
		{
			c = fgetc(f);
			length++;
		}
		while(c != '\n' && !feof(f));
		
		if(*buffSize < length)
		{
			*buff = realloc(*buff, length * sizeof(char));
			*buffSize = length * sizeof(char);
		}
		
		fseek(f, startPos, SEEK_SET);
		
		fgets(*buff, length-1, f);
		(*buff)[length-1] = '\0';
		fgetc(f);
	}
#endif

void openConfig()
{
	char userConfigPath[PATH_MAX];

	#ifdef __WIN32
		strcpy(userConfigPath, getenv("AppData"));
		strcat(userConfigPath, "\\lwp\\lwp.cfg");
	#else
		struct passwd *pw = getpwuid(getuid());
		sprintf(userConfigPath, "%s/.config/lwp/lwp.cfg", pw->pw_dir);
	#endif

	file = fopen(userConfigPath, "r");
	if(!file)
	{
		lwpLog(LOG_INFO, "User config file not found, opening default config");
		#ifdef __WIN32
			char defaultConfigPath[PATH_MAX];
			GetModuleFileNameA(NULL, defaultConfigPath, PATH_MAX);
			char *ptr = defaultConfigPath+strlen(defaultConfigPath)-1;
			while(*ptr != '\\')
				ptr--;
			*ptr = '\0';
			strcat(defaultConfigPath, "\\defaultWin.cfg");
			
			file = fopen(defaultConfigPath, "r");
		#else
			file = fopen("/etc/lwp.cfg", "r");
		#endif
		if(!file)
			lwpLog(LOG_ERROR, "Default config file not found!");
	}
}

void closeConfig()
{
	fclose(file);
}

int findLine(const char *name, int type, void *output)
{
	char *buff = NULL;
	size_t buffSize = 0;

	int found = 0;

	fseek(file, 0, SEEK_SET);

	do
	{
		getline(&buff, &buffSize, file);

		if(buffSize > 0 && buff[0] != '#')
		{
			if(strncmp(name, buff, strlen(name)) == 0)	
			{
				char *valuePtr = 	buff;
				while(*valuePtr != '=' && *valuePtr != '\0')
				{
					valuePtr++;
				}

				if(*valuePtr == '=')
				{
					valuePtr++;

					if(valuePtr[strlen(valuePtr)-1] == '\n')
							valuePtr[strlen(valuePtr)-1] = '\0';

					found = 1;	

					switch(type)
					{
						case TYPE_INT:
							*(int*)output = atoi(valuePtr);
							break;
						case TYPE_FLOAT:
							*(float*)output = atof(valuePtr);
							break;
						case TYPE_STR:
							strcpy((char*)output, valuePtr);
							break;
					}
				}
			}
		}
	}
	while(!found && buffSize > 0);

	free(buff);
	return found;
}

int parseConfig(Config *cfg)
{
	const char *names[] = {
		"path",
		"monitors",
		"count",
		"smooth",
		"movementX",
		"movementY"
	};

	const int types[] = {
		TYPE_STR,
		TYPE_INT,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_FLOAT,
		TYPE_FLOAT
	};	

	void *outputs[] = {
		cfg->path,
		&cfg->monitors,
		&cfg->count,
		&cfg->smooth,
		&cfg->movementX,
		&cfg->movementY,
	};	

	for(int i = 0; i < 6; i++)
	{
		if(!findLine(names[i], types[i], outputs[i]))
		{
			lwpLog(LOG_ERROR, "Can't find line '%s' in config", names[i]);
			return 0;
		}
	}
	
	#ifdef __WIN32
		if(strlen(cfg->path) < 2)
		{
			
			GetModuleFileNameA(NULL, cfg->path, PATH_MAX);
			char *ptr = cfg->path+strlen(cfg->path)-1;
			while(*ptr != '\\')
				ptr--;
			*ptr = '\0';
			strcat(cfg->path, "\\wallpapers\\test");
		}
	#endif
	return 1;
}

void parseInstancesConfig(Instance *instances, int instancesCount)
{
	char str[15];

	const char *paramStr[] = {
		"_x",
		"_y",
		"_w",
		"_h"
	};

	for(int i = 0; i < instancesCount; i++)
	{
		int *outputPtr[] = {
			&instances[i].dest.x,
			&instances[i].dest.y,
			&instances[i].dest.w,
			&instances[i].dest.h,
		};

		for(int p = 0; p < 4; p++)
		{
			sprintf(str, "monitor%d%s", i+1, paramStr[p]);
			if(!findLine(str, TYPE_INT, outputPtr[p]))
				lwpLog(LOG_ERROR, "Can't find line '%s' in config", str);
		}
	}
}
