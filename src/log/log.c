/*=========================================================
INCLUDES
=========================================================*/

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "log/log.h"
#include "utl/utl_array.h"

/*=========================================================
CONSTANTS
=========================================================*/

#define MAX_MSG_SIZE	512

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Gets a string representation of a log level. */
static const char* get_log_level_str(uint8_t level);

/*=========================================================
CONSTRUCTORS
=========================================================*/

void log__construct(log_t* log)
{
	clear_struct(log);

	for (int i = 0; i < cnt_of_array(log->targets); ++i)
	{
		utl_array_init(&log->targets[i]);
	}
}

void log__destruct(log_t* log)
{
	for (int i = 0; i < cnt_of_array(log->targets); ++i)
	{
		utl_array_destroy(&log->targets[i]);
	}
}

/*=========================================================
FUNCTIONS
=========================================================*/

void log__msg(log_t* log, uint8_t level, const char* in_msg)
{
	/* Validate log level */
	if (level >= LOG_LEVEL__COUNT)
	{
		assert(FALSE);
	}

	/* Build message */
	char msg[MAX_MSG_SIZE];
	sprintf_s(msg, sizeof(msg) - 1, "[%s] %s\n", get_log_level_str(level), in_msg);

	/* Get the array of target callbacks for the log level */
	utl_array_t(log_target_func)* targets = &log->targets[level];
	
	/* Log the message to each target callback */
	for (int i = 0; i < targets->count; ++i)
	{
		targets->data[i](log, msg);
	}

	/* Assert on fatal log */
	if (level == LOG_LEVEL_FATAL)
	{
		assert(FALSE);
	}
}

void log__msg_with_source
	(
	log_t*				log, 
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
	log__msg(log, level, full_msg);
}

void log__register_target(log_t* log, log_target_func target)
{
	for (int i = 0; i < cnt_of_array(log->targets); ++i)
	{
		utl_array_push(&log->targets[i], target);
	}
}

static const char* get_log_level_str(uint8_t level)
{
	switch (level)
	{
	case LOG_LEVEL_DEBUG:
		return "DEBUG";
	case LOG_LEVEL_INFO:
		return "INFO";
	case LOG_LEVEL_WARN:
		return "WARN";
	case LOG_LEVEL_ERROR:
		return "ERROR";
	case LOG_LEVEL_FATAL:
		return "FATAL";
	}

	/* Unknown log level */
	return "";
}