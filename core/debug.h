#ifndef DEBUG_H
#define DEBUG_H

#define LOG_ERROR   0
#define LOG_INFO    1
#define LOG_WARNING 2

void lwpLog(int type, const char *str, ...);

#endif
