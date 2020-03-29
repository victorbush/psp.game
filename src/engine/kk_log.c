/*=========================================================
INCLUDES
=========================================================*/

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "engine/kk_log.h"
#include "utl/utl_array.h"

#include "autogen/kk_log.static.h"

/*=========================================================
CONSTANTS
=========================================================*/

#define MAX_MSG_SIZE	512

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

//## public
/**
Initializes a logging context.
@param log The context to init.
*/
void kk_log__construct(kk_log_t* log)
{
	clear_struct(log);

	for (int i = 0; i < cnt_of_array(log->targets); ++i)
	{
		utl_array_init(&log->targets[i]);
	}
}

//## public
/**
Destructs a logging context.
@param log The context to destruct.
*/
void kk_log__destruct(kk_log_t* log)
{
	for (int i = 0; i < cnt_of_array(log->targets); ++i)
	{
		utl_array_destroy(&log->targets[i]);
	}
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## public
/**
Logs a message.

@param log The logging context.
@param level The log level.
@param msg The message to log.
*/
void kk_log__msg(kk_log_t* log, uint8_t level, const char* in_msg)
{
	/* Validate log level */
	if (level >= KK_LOG_LEVEL__COUNT)
	{
		assert(FALSE);
	}

	/* Build message */
	char msg[MAX_MSG_SIZE];
	sprintf_s(msg, sizeof(msg) - 1, "[%s] %s\n", get_log_level_str(level), in_msg);

	/* Get the array of target callbacks for the log level */
	utl_array_t(kk_log_target_func)* targets = &log->targets[level];
	
	/* Log the message to each target callback */
	for (int i = 0; i < targets->count; ++i)
	{
		targets->data[i](log, msg);
	}

	/* Assert on fatal log */
	if (level == KK_LOG_LEVEL_FATAL)
	{
		assert(FALSE);
	}
}

//## public
/**
Logs a message and includes the source filename and line number where the message originated from.

@param log The logging context.
@param level The log level.
@param filename The source filename.
@param line The line number in the source file.
@param msg_fmt The message to log.
*/
void kk_log__msg_with_source
	(
	kk_log_t*			log, 
	uint8_t				level, 
	const char*			filename, 
	int					line, 
	const char*			msg_fmt, 
	...
	)
{
	/* Build message */
	char msg[MAX_MSG_SIZE];

	/* Format message with the variable arguments */
	va_list var_args;
	va_start(var_args, msg_fmt);
	vsprintf_s(msg, sizeof(msg) - 1, msg_fmt, var_args);
	va_end(var_args);

	/* [Filename:Line] message */
	static const char* FORMAT = "[%s:%i] %s";

	/* Build full message */
	char full_msg[MAX_MSG_SIZE];
	sprintf_s(full_msg, sizeof(full_msg) - 1, FORMAT, filename, line, msg);

	/* Log */
	kk_log__msg(log, level, full_msg);
}

//## public
/**
Registers a logging target for all log levels.

@param log The logging context.
@param target The target callback function to call.
*/
void kk_log__register_target(kk_log_t* log, kk_log_target_func target)
{
	for (int i = 0; i < cnt_of_array(log->targets); ++i)
	{
		utl_array_push(&log->targets[i], target);
	}
}

//## static
/** Gets a string representation of a log level. */
static const char* get_log_level_str(uint8_t level)
{
	switch (level)
	{
	case KK_LOG_LEVEL_DEBUG:
		return "DEBUG";
	case KK_LOG_LEVEL_INFO:
		return "INFO";
	case KK_LOG_LEVEL_WARN:
		return "WARN";
	case KK_LOG_LEVEL_ERROR:
		return "ERROR";
	case KK_LOG_LEVEL_FATAL:
		return "FATAL";
	}

	/* Unknown log level */
	return "";
}