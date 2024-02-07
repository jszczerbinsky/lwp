#include <stdarg.h>
#include <stdio.h>
#include <glib.h>

#include "main.h"

void getLogFilePath(char *path)
{
  #ifdef __WIN32
  sprintf(path, "%s\\lwp\\lwp.log", g_get_user_data_dir());
  #else
  sprintf(path, "/home/cziken/.config/lwp/log");
  #endif
}

void lwpLog(int type, const char *str, ...)
{
  char path[PATH_MAX];
  getLogFilePath(path);

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

