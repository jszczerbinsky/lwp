#include <stdarg.h>
#include "debug.h"

void lwpLog(int type, const char *str, ...)
{
	char *typePrefixCodes;
	char *typePrefix;

	switch(type)
	{
		case LOG_ERROR:
			typePrefixCodes = "\x1b[31m\x1b[1m";
			typePrefix = "ERROR";
			break;
		case LOG_INFO:
			typePrefixCodes = "\x1b[34m\x1b[1m";
			typePrefix = "INFO";
			break;
		case LOG_WARNING:
			typePrefixCodes = "\x1b[33m\x1b[1m";
			typePrefix = "WARNING";
			break;
	}	

	printf("%s%s: \x1b[0m", typePrefixCodes, typePrefix);

	va_list args;
	va_start(args, str);
	vprintf(str, args); 
	va_end(args);

	putchar('\n');
	fflush(stdout);
}

