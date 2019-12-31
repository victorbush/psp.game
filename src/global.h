/*=========================================================
Global variables and functions. Globals should be kept
to a minimum.
=========================================================*/

#ifndef GLOBAL_H
#define GLOBAL_H

/*=========================================================
INCLUDES
=========================================================*/

#include "engine/engine.h"
#include "log/log.h"
#include "platform/platform.h"

/*=========================================================
VARIABLES
=========================================================*/

extern engine_t*		g_engine;
extern log_t*			g_log;
extern platform_t*		g_platform;		/* The platform being used. */

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* GLOBAL_H */