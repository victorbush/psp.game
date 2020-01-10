#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "gpu/gpu_.h"
#include "platform/platform_.h"
#include "platform/platform_window_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/gpu_window.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

struct platform_window_s
{
	void*	context;	/* Context pointer for platform-specific data */

	platform_t*		platform;
	gpu_window_t	gpu_window;
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

void platform_window__construct(platform_window_t* window, platform_t* platform, gpu_t* gpu, uint32_t width, uint32_t height);

void platform_window__destruct(platform_window_t* window, platform_t* platform, gpu_t* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* PLATFORM_WINDOW_H */