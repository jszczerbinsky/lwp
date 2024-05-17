#include <glib.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "../common.h"

void clearlog()
{
  char path[PATH_MAX];
  getLogPath(path);

  FILE *file = fopen(path, "w");
  fclose(file);
}

void printlog(int type, const char *str, ...)
{
  char path[PATH_MAX];
  getLogPath(path);

  FILE *file = fopen(path, "a");

  time_t t;
  time(&t);
  struct tm *time = localtime(&t);

  char *typePrefix = "";

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

  fprintf(
      file,
      "%d-%02d-%02d %02d:%02d:%02d %s: ",
      time->tm_year + 1900,
      time->tm_mon + 1,
      time->tm_mday,
      time->tm_hour,
      time->tm_min,
      time->tm_sec,
      typePrefix
  );

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
  buff[len] = '\0';

  fclose(file);

  return buff;
}
