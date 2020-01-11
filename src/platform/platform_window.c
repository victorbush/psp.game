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

//## public
void platform_window__construct(platform_window_t* window, platform_t* platform, gpu_t* gpu, uint32_t width, uint32_t height)
{
	clear_struct(window);
	window->platform = platform;

	/* Platform-specific construction */
	platform->window__construct(window, platform, gpu, width, height);

	/* Create GPU window (framebuffers, swpchain, etc) */
	gpu_window__construct(&window->gpu_window, gpu, window, width, height);
}

//## public
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

//## public
/**
Triggers the request close callback.

@param window The window context.
*/
void platform_window__request_close(platform_window_t* window)
{
	if (!window->request_close_callback)
	{
		return;
	}

	window->request_close_callback(window, window->reuqest_close_user_data);
}

//## public
/**
Sets the callback for when this window wants to close.

@param window The window context.
@param callback The callback function. Use NULL to clear callback.
@param user_data User data pointer that is passed to the callback.
*/
void platform_window__set_request_close_callback
	(
	platform_window_t*			window, 
	platform_window_request_close_callback_func 
								callback,
	void*						user_data
	)
{
	window->request_close_callback = callback;
	window->reuqest_close_user_data = user_data;
}
