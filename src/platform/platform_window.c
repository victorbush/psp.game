/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/gpu.h"
#include "gpu/gpu_window.h"
#include "platform.h"
#include "platform/platform_window.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void platform_window__construct(platform_window_t* window, platform_t* platform, gpu_t* gpu, uint32_t width, uint32_t height)
{
	clear_struct(window);
	window->platform = platform;

	/* Platform-specific construction */
	platform->window__construct(window, platform, gpu, width, height);

	/* Create GPU window (framebuffers, swpchain, etc) */
	gpu_window__construct(&window->gpu_window, gpu, window, width, height);
}

void platform_window__destruct(platform_window_t* window, platform_t* platform, gpu_t* gpu)
{
	/* Destroy GPU window */
	gpu_window__destruct(&window->gpu_window);

	/* Platform specific destruction */
	platform->window__destruct(window, platform, gpu);
}

/*=========================================================
FUNCTIONS
=========================================================*/
