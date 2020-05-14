#ifndef KK_LOG_H
#define KK_LOG_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "engine/kk_log_.h"

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
	KK_LOG_LEVEL_DEBUG,
	KK_LOG_LEVEL_INFO,
	KK_LOG_LEVEL_WARN,
	KK_LOG_LEVEL_ERROR,
	KK_LOG_LEVEL_FATAL,

	KK_LOG_LEVEL__COUNT
};

typedef void (*kk_log_target_func)(kk_log_t* log, const char* msg);

utl_array_declare_type(kk_log_target_func);

struct kk_log_s
{
	uint8_t							next_target_idx;

	/** Each log level has an array of targets */
	utl_array_t(kk_log_target_func)	targets[KK_LOG_LEVEL__COUNT];
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

extern kk_log_t* g_log;

#define kk_log__dbg(msg) \
	kk_log__msg(g_log, KK_LOG_LEVEL_DEBUG, msg)

#define kk_log__info(msg) \
	kk_log__msg(g_log, KK_LOG_LEVEL_INFO, msg)

#define kk_log__warn(msg) \
	kk_log__msg(g_log, KK_LOG_LEVEL_WARN, msg)

#define kk_log__error(msg) \
	kk_log__msg_with_source(g_log, KK_LOG_LEVEL_ERROR, __FILENAME__, __LINE__, msg)

#define kk_log__error_fmt(msg, params) \
	kk_log__msg_with_source(g_log, KK_LOG_LEVEL_ERROR, __FILENAME__, __LINE__, msg, params)

#define kk_log__fatal(msg) \
	kk_log__msg_with_source(g_log, KK_LOG_LEVEL_FATAL, __FILENAME__, __LINE__, msg)

#define kk_log__fatal_fmt(msg, params) \
	kk_log__msg_with_source(g_log, KK_LOG_LEVEL_FATAL, __FILENAME__, __LINE__, msg, params)

#include "autogen/kk_log.public.h"

#endif /* KK_LOG_H */