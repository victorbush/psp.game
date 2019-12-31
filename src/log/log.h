#ifndef LOG_H
#define LOG_H

/*=========================================================
DECLARATIONS
=========================================================*/

typedef struct log_s log_t;

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "utl/utl_array.h"

/*=========================================================
CONSTANTS / MACROS
=========================================================*/

/** Gets the name of the current source file. Extension of the __FILE__ macro. */
#define __FILENAME__ (strrchr("/" __FILE__, '/') + 1)

/*=========================================================
TYPES
=========================================================*/

enum
{
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL,

	LOG_LEVEL__COUNT
};

typedef void (*log_target_func)(log_t* log, const char* msg);

utl_array_declare_type(log_target_func);

struct log_s
{
	uint8_t							next_target_idx;

	/** Each log level has an array of targets */
	utl_array_t(log_target_func)	targets[LOG_LEVEL__COUNT];
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Initializes a logging context.
@param log The context to init.
*/
void log__construct(log_t* log);

/**
Destructs a logging context.
@param log The context to destruct.
*/
void log__destruct(log_t* log);

/*=========================================================
FUNCTIONS
=========================================================*/

extern log_t* g_log;

#define log__dbg(msg) \
	log__msg(g_log, LOG_LEVEL_DEBUG, msg)

#define log__info(msg) \
	log__msg(g_log, LOG_LEVEL_INFO, msg)

#define log__warn(msg) \
	log__msg(g_log, LOG_LEVEL_WARN, msg)

#define log__error(msg) \
	log__msg_with_source(g_log, LOG_LEVEL_ERROR, __FILENAME__, __LINE__, msg)

#define log__fatal(msg) \
	log__msg_with_source(g_log, LOG_LEVEL_FATAL, __FILENAME__, __LINE__, msg)

/**
Logs a message.

@param log The logging context.
@param level The log level.
@param msg The message to log.
*/
void log__msg(log_t* log, uint8_t level, const char* msg);

/**
Logs a message and includes the source filename and line number where the message originated from.

@param log The logging context.
@param level The log level.
@param filename The source filename.
@param line The line number in the source file.
@param msg_fmt The message to log.
*/
void log__msg_with_source(log_t* log, uint8_t level, const char* filename, int line, const char* msg_fmt, ...);

/**
Registers a logging target for all log levels.

@param log The logging context.
@param target The target callback function to call.
*/
void log__register_target(log_t* log, log_target_func target);

#endif /* LOG_H */