#ifndef GPU_WINDOW_H
#define GPU_WINDOW_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "engine/kk_camera_.h"
#include "gpu/gpu_.h"
#include "gpu/gpu_frame_.h"
#include "gpu/gpu_window_.h"
#include "platform/platform_window_.h"
#include "thirdparty/cimgui/imgui_jetz.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/gpu_frame.h"

/*=========================================================
TYPES
=========================================================*/

struct gpu_window_s
{
	/*
	Dependencies
	*/
	gpu_t*				gpu;				/* GPU context */
	platform_window_t*	platform_window;	/* Platform window context */

	/*
	Create/destroy
	*/
	void*				data;				/* Pointer to GPU-specific data. */

	/*
	Other
	*/
	gpu_frame_t			frames[NUM_FRAMES];	/* */
	uint8_t				frame_idx;			/* Index of the current frame. */
	uint32_t			height;
	uint32_t			width;
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Constructs a window.

@param window The window to construct.
@param gpu The GPU context.
*/
void gpu_window__construct(gpu_window_t* window, gpu_t* gpu, platform_window_t* platform_window, uint32_t width, uint32_t height);

/**
Destructs a window.

@param window The window to destruct.
@param gpu The GPU context.
*/
void gpu_window__destruct(gpu_window_t* window);

/*=========================================================
FUNCTIONS
=========================================================*/

gpu_frame_t* gpu_window__begin_frame(gpu_window_t* window, kk_camera_t* camera, float delta_time);

void gpu_window__end_frame(gpu_window_t* window, gpu_frame_t* frame);

void gpu_window__render_imgui(gpu_window_t* window, gpu_frame_t* frame, ImDrawData* draw_data);

void gpu_window__resize(gpu_window_t* window, uint32_t width, uint32_t height);

#endif /* GPU_WINDOW_H */