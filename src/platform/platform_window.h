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

typedef void (*platform_window_request_close_callback_func)(platform_window_t* window, void* user_data);

struct platform_window_s
{
	void*	context;	/* Context pointer for platform-specific data */
	
	/* Callback for when the window wants to close (i.e., the "X" button was clicked) */
	platform_window_request_close_callback_func 
						request_close_callback;	
	void*				reuqest_close_user_data;

	platform_t*		platform;
	gpu_window_t	gpu_window;
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/platform_window.public.h"

#endif /* PLATFORM_WINDOW_H */