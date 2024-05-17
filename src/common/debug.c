#include <glib.h>
#include <stdarg.h>
#include <stdio.h>

#include "../common.h"

void lwpLog(int type, const char *str, ...)
{
  char path[PATH_MAX];
  getLogPath(path);

  FILE *file = fopen(path, "a");

  char *typePrefix;

  switch (type)
  {
    case LOG_ERROR:
      typePrefix = "ERROR";
      break;
    case LOG_INFO:
      typePrefix = "INFO";
      break;
    case LOG_WARNING:
      typePrefix = "WARNING";
      break;
  }

  fprintf(file, "%s: ", typePrefix);

  va_list args;
  va_start(args, str);
  vfprintf(file, str, args);
  va_end(args);

  fprintf(file, "\n");

  fclose(file);
}

char *readLogFile()
{
  char path[PATH_MAX];
  getLogPath(path);

  FILE *file = fopen(path, "r");

  fseek(file, 0, SEEK_END);
  int len = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buff = malloc((len + 1) * sizeof(char));
  fread(buff, sizeof(char), len, file);

  fclose(file);

  return buff;
}
