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
static char            log_level = LOG_DEBUG;
static plug_print_func post      = nullptr;
static plug_print_func pre       = nullptr;

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

	// stright up rawdogging it
	// needs this to prevent multiple threads to clutter stdout
	char log_buffer[BUFFER_SIZE];

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

	int count = 0;
	if (pre != nullptr && pre != NULL) {
		pre(log_buffer, &count);
	}

	// [ DEBUG ] server.cpp:167  in void start(runtimeIn    [
	// this should return always the same value
	count += snprintf(log_buffer + count, (size_t)(BUFFER_SIZE - count), "%s %-10.10s:%-4d " GRY "in" RESET " %-20.20s    " RESET, prefix, file_name, line_num, function_name);
	count += vsnprintf(log_buffer + count, (size_t)(BUFFER_SIZE - count), format, args);

	if (post != nullptr && post != NULL) {
		post(log_buffer, &count);
	}
	if (count > BUFFER_SIZE) {
		count = BUFFER_SIZE;
	}
	printf("%*s", count, log_buffer);
	fflush(stdout); // ensure printing even with no \n

	va_end(args);
}
