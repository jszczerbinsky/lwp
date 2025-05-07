#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "types/log.h"

void printlog(int type, const LogContext* ctx, const char* internalerr,
			  const char* str, ...) {
	LogContext default_logctx = {.name = "core", .lvl = LOG_DEBUG};
	if (!ctx) {
		ctx = &default_logctx;
	}

	if (type > ctx->lvl) {
		return;
	}

	// TODO: open logfile
	//FILE* file = tmpfile();
	FILE* pipe = type == LOG_ERROR ? stderr : stdout;

	time_t t;
	time(&t);
	struct tm* time = localtime(&t);

	char* type_prefix = "";
	char* type_ec = "";

	switch (type) {
	case LOG_ERROR:
		type_prefix = " ERROR ";
		type_ec = "\033[31m";
		break;
	case LOG_INFO:
		type_prefix = "  INFO ";
		type_ec = "\033[36m";
		break;
	case LOG_WARNING:
		type_prefix = "WARNING";
		type_ec = "\033[33m";
		break;
	case LOG_DEBUG:
		type_prefix = " DEBUG ";
		type_ec = "\033[35m";
		break;
	}

	const char* tsformat = "%d-%02d-%02d %02d:%02d:%02d";

	//fprintf(file, tsformat, time->tm_year + 1900, time->tm_mon + 1,
	//		time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);
	fprintf(pipe, tsformat, time->tm_year + 1900, time->tm_mon + 1,
			time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);

	//const char* fileformat = " %s [%s] ";
	const char* pipeformat = " %s%s\033[0m [\033[34m%s\033[0m] ";

	//fprintf(file, fileformat, type_prefix, ctx->name);
	fprintf(pipe, pipeformat, type_ec, type_prefix, ctx->name);

	va_list args;

	//va_start(args, str);
	//vfprintf(file, str, args);
	//va_end(args);

	va_start(args, str);
	vfprintf(pipe, str, args);
	va_end(args);

	if (internalerr) {
		//fprintf(file, " - internal error: %s", internalerr);
		fprintf(pipe, " - internal error: %s", internalerr);
	}

	//fprintf(file, "\n");
	fprintf(pipe, "\n");

	fflush(pipe);

	//fclose(file);
}
