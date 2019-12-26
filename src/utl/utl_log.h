/*=========================================================
Logging and debugging utilities
=========================================================*/

#ifndef UTL_LOG_H
#define UTL_LOG_H

/*=========================================================
INCLUDES
=========================================================*/

#include <assert.h>

#ifdef JETZ_CONFIG_PLATFORM_GLFW
#include <stdio.h>
#endif

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

#define FATAL(msg) assert(FALSE)
#define FATAL_IF_NULL(var, msg) assert(var)

#ifdef JETZ_CONFIG_PLATFORM_GLFW
#define LOG_ERROR(msg) printf(msg)
#elif JETZ_CONFIG_PLATFORM_PSP
#define LOG_ERROR(msg) // TODO ???
#elif JETZ_CONFIG_PLATFORM_TESTS
#define LOG_ERROR(msg)
#endif

#define DBG_ASSERT(condition, msg) assert(condition)
#define DBG_ASSERT_ALWAYS(msg) assert(FALSE)

#endif /* UTL_LOG_H */