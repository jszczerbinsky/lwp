#include "main.h"
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

void printlog(int type, const char* str, ...) {
	// TODO: open logfile
	FILE* file = tmpfile();
	FILE* pipe = type == LOG_ERROR ? stderr : stdout;

	time_t t;
	time(&t);
	struct tm* time = localtime(&t);

	char* type_prefix = "";

	switch (type) {
	case LOG_ERROR:
		type_prefix = "ERROR";
		break;
	case LOG_INFO:
		type_prefix = "INFO";
		break;
	case LOG_WARNING:
		type_prefix = "WARNING";
		break;
	}

	const char* format = "%d-%02d-%02d %02d:%02d:%02d %s: ";

	fprintf(file, format, time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
			time->tm_hour, time->tm_min, time->tm_sec, type_prefix);
	fprintf(pipe, format, time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
			time->tm_hour, time->tm_min, time->tm_sec, type_prefix);

	va_list args;

	va_start(args, str);
	vfprintf(file, str, args);
	va_end(args);

	va_start(args, str);
	vfprintf(pipe, str, args);
	va_end(args);

	fprintf(file, "\n");
	fprintf(pipe, "\n");

	fflush(pipe);

	fclose(file);
}
