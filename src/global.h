/*=========================================================
Global variables and functions.
=========================================================*/

#ifndef GLOBAL_H
#define GLOBAL_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "app/app_.h"
#include "engine/kk_log_.h"
#include "gpu/gpu_.h"
#include "platform/platform_.h"

/*=========================================================
INCLUDES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

extern app_t*			g_app;			/* Current app instance. */
extern gpu_t*			g_gpu;			/* Current GPU instance. */
extern kk_log_t*		g_log;			/* Current logging instance. */
extern platform_t*		g_platform;		/* Current platform instance. */

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* GLOBAL_H */