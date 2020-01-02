#ifndef GPU_WINDOW_H
#define GPU_WINDOW_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "gpu/gpu_.h"
#include "gpu/gpu_window_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"

/*=========================================================
TYPES
=========================================================*/

struct gpu_window_s
{
	void*				data;		/* Pointer to GPU-specific data. */
	uint32_t			width;
	uint32_t			height;
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Constructs a window.

@param window The window to construct.
@param gpu The GPU context.
*/
void gpu_window__construct(gpu_window_t* window, gpu_t* gpu);

/**
Destructs a window.

@param window The window to destruct.
@param gpu The GPU context.
*/
void gpu_window__destruct(gpu_window_t* window, gpu_t* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

void gpu_window__resize(gpu_window_t* window, gpu_t* gpu, uint32_t width, uint32_t height);

#endif /* GPU_WINDOW_H */