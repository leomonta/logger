#pragma once

#if defined(__cplusplus)
extern "C" { // Prevents name mangling of functions
#endif

#ifdef __GNUC__
#	define LLOG_FORMAT __attribute__((format(printf, 5, 6)))
#	define LLOG_LINE   __LINE__
#	define LLOG_FILE   __FILE_NAME__
#	define LLOG_FUNC   __PRETTY_FUNCTION__
#else
#	define LLOG_FORMAT
#	define LLOG_LINE 0
#	define LLOG_FILE ""
#	define LLOG_FUNC __func__
#endif

// quick macro to use the automatic __FILE__ and __LINE__ gcc defines
#define llog(level, format, ...) logger(level, LLOG_FILE, LLOG_LINE, LLOG_FUNC, format __VA_OPT__(, ) __VA_ARGS__)

#define LLOG_BUFFER_SIZE 8192

typedef enum : char {
	LOG_FATAL,
	LOG_ERROR,
	LOG_WARNING,
	LOG_INFO,
	LOG_DEBUG
} logLevel;

/**
 * A pluggable function to use inside the logging process
 *
 * The user should be mindful that the buffer is `LLOG_BUFFER_SIZE` bytes long and that should not be overwritten.
 * Regardless the printed amount will be `LLOG_BUFFER_SIZE`
 *
 * @param[in] buffer the start of char buffer that will be printed at the end of the process
 * @param[in] count the last position where data was written to the buffer.
 * This can (and should) be modified in case data is added to the buffer, to specify where to continue writing data
 *
 * @return if the operation was successful, disregarded for now
 */
typedef bool (*plug_print_func)(char *buffer, unsigned long *count);

/**
 * specifies whether to print with colors or not,
 * by default the use of color is dependent on the result of `isatty`
 *
 * @param do_color whether to use colors while printing or not
 */
void set_color(bool do_color);

/**
 * Set the local log_level variable to prevent any log message 'lower' than the one selected from being printed
 *
 * @param[in] ll the new log level to use
 */
void set_log_level(logLevel ll);

/**
 * Sets the function to execute before the final printf to stdout and before the flush
 * If nothing is set nothing get executed
 * nullptr or NULL can be set to remove the previously entered function
 *
 * @param[in] fun the function to use from this point on
 */
void set_post_print_func(plug_print_func fun);

/**
 * Sets the function to execute before anything gets written to the buffer
 * If nothing is set nothing get executed
 * nullptr or NULL can be set to remove the previously entered function
 *
 * @param[in] fun the function to use from this point on
 */
void set_pre_print_func(plug_print_func fun);

/**
 * logs a message to the standard output, with info about log level, file, line number, and function name
 * the variadic arguments are passed, alongside format, to vprintf
 * it is suggested to use the `llog` for automatic file name, line number, and function name detection
 *
 * internally it keeps a local buffer where the printf functions output to, then a single call to normal printf is executed and then an fflush(stdout)
 * this makes it reasonably thread safe
 * The user should make sure to add a `\n` at the end of the buffer
 *
 * @param[in] ll the log level of the message
 * @param[in] file_name the file to show at the log message
 * @param[in] line_num the line number to show in the log message
 * @param[in] function_name the function name to show in the log message
 * @param[in] format the printf format to use when printing the variadic arguments
 * @param[in] ... the variadic arguments to pass to printf
 *
 */
void logger(const logLevel ll, const char *file_name, const unsigned line_num, const char *function_name, const char *format, ...) LLOG_FORMAT;

#if defined(__cplusplus)
}
#endif
