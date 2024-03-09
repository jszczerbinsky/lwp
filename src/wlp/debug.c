#include <glib.h>
#include <stdarg.h>
#include <stdio.h>

#include "main.h"

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

