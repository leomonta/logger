#include "logger.h"

#include <stdarg.h>
#include <stdio.h>

#define RED   "\x1B[031m"
#define GRN   "\x1B[032m"
#define YEL   "\x1B[033m"
#define BLU   "\x1B[034m"
#define MAG   "\x1B[035m"
#define CYN   "\x1B[036m"
#define WHT   "\x1B[037m"
#define GRY   "\x1B[090m"
#define RESET "\x1B[0m"

// default log level if not specified via 'set_log_level'
static _Atomic char            log_level = LOG_DEBUG;
static _Atomic plug_print_func post      = nullptr;
static _Atomic plug_print_func pre       = nullptr;

void set_log_level(logLevel ll) {
	log_level = ll;
}

void set_post_print_func(plug_print_func fun) {
	post = fun;
}

void set_pre_print_func(plug_print_func fun) {
	pre = fun;
}

void logger(const logLevel ll, const char *file_name, const unsigned line_num, const char *function_name, const char *format, ...) {

	// ignore too low log levels
	if (log_level < ll) {
		return;
	}

	// straight up rawdogging it
	// needs this to prevent multiple threads to clutter stdout
	char log_buffer[LLOG_BUFFER_SIZE];

	va_list args;
	va_start(args, format);

	// shouldn't happen but to be safe
	const char *prefix = "[ UNKWN ]";

	switch (ll) {
	case LOG_DEBUG:
		prefix = BLU "[ DEBUG ]" RESET;
		break;
	case LOG_INFO:
		prefix = GRN "[  INFO ]" RESET;
		break;
	case LOG_WARNING:
		prefix = YEL "[WARNING]" RESET;
		break;
	case LOG_ERROR:
		prefix = MAG "[ ERROR ]" RESET;
		break;
	case LOG_FATAL:
		prefix = RED "[ FATAL ]" RESET;
		break;
	}

	unsigned long count = 0;
	auto l_pre = pre;
	if (l_pre != nullptr && l_pre != NULL) {
		l_pre(log_buffer, &count);
	}

	// [ DEBUG ] server.cpp:167  in void start(runtimeIn    [
	// this should return always the same value
	count = count > LLOG_BUFFER_SIZE ? LLOG_BUFFER_SIZE : count;
	count += (unsigned long)(snprintf(log_buffer + count, LLOG_BUFFER_SIZE - count, "%s %-10.10s:%-4u " GRY "in" RESET " %-20.20s    " RESET, prefix, file_name, line_num, function_name));
	count = count > LLOG_BUFFER_SIZE ? LLOG_BUFFER_SIZE : count;
	count += (unsigned long)(vsnprintf(log_buffer + count, LLOG_BUFFER_SIZE - count, format, args));
	count = count > LLOG_BUFFER_SIZE ? LLOG_BUFFER_SIZE : count;

	auto l_post = post;
	if (l_post != nullptr && l_post != NULL) {
		l_post(log_buffer, &count);
	}
	count = count > LLOG_BUFFER_SIZE ? LLOG_BUFFER_SIZE : count;
	printf("%.*s", (int)count, log_buffer);
	fflush(stdout); // ensure printing even with no \n

	va_end(args);
}
