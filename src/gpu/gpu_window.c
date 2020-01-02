/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "gpu/gpu.h"
#include "gpu/gpu_window.h"
#include "log/log.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void gpu_window__construct(gpu_window_t* window, gpu_t* gpu)
{
	clear_struct(window);

	gpu->intf->window__construct(window, gpu);
}

void gpu_window__destruct(gpu_window_t* window, gpu_t* gpu)
{
	gpu->intf->window__destruct(window, gpu);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void gpu_window__resize(gpu_window_t* window, gpu_t* gpu, uint32_t width, uint32_t height)
{
	window->width = width;
	window->height = height;
	gpu->intf->window__resize(window, gpu, width, height);
}