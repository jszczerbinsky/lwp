#ifndef LOG_H
#define LOG_H

typedef struct {
	char name[30];
	int	 lvl;
} LogContext;

#define LOG_ERROR	0
#define LOG_WARNING 1
#define LOG_INFO	2
#define LOG_DEBUG	3

void printlog(int type, const LogContext* ctx, const char* internalerr,
			  const char* str, ...);

#endif
