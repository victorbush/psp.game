/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "gpu/gpu.h"
#include "gpu/gpu_frame.h"
#include "gpu/gpu_window.h"
#include "log/log.h"
#include "platform/platform_window.h"
#include "thirdparty/cimgui/imgui_jetz.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void gpu_window__construct(gpu_window_t* window, gpu_t* gpu, platform_window_t* platform_window, uint32_t width, uint32_t height)
{
	clear_struct(window);
	window->gpu = gpu;
	window->platform_window = platform_window;
	window->width = width;
	window->height = height;

	/* Init frames */
	for (int i = 0; i < cnt_of_array(window->frames); ++i)
	{
		gpu_frame__construct(&window->frames[i], window->gpu);
	}

	/* Interface */
	gpu->intf->window__construct(window, gpu, width, height);
}

void gpu_window__destruct(gpu_window_t* window)
{
	/* Interface */
	window->gpu->intf->window__destruct(window, window->gpu);

	/* Destruct frames */
	for (int i = 0; i < cnt_of_array(window->frames); ++i)
	{
		gpu_frame__destruct(&window->frames[i], window->gpu);
	}
}

/*=========================================================
FUNCTIONS
=========================================================*/

gpu_frame_t* gpu_window__begin_frame(gpu_window_t* window, camera_t* camera, float delta_time)
{
	/* Get next frame */
	gpu_frame_t* frame = &window->frames[window->frame_idx];
	frame->frame_idx = window->frame_idx;

	/* Interface */
	window->gpu->intf->window__begin_frame(window, frame, camera);

	/* Update frame index */
	window->frame_idx = (window->frame_idx + 1) % NUM_FRAMES;

	return frame;
}

void gpu_window__end_frame(gpu_window_t* window, gpu_frame_t* frame)
{
	/* Interface */
	window->gpu->intf->window__end_frame(window, frame);
}

void gpu_window__render_imgui(gpu_window_t* window, gpu_frame_t* frame, ImDrawData* draw_data)
{
	window->gpu->intf->window__render_imgui(window, frame, draw_data);
}

void gpu_window__resize(gpu_window_t* window, uint32_t width, uint32_t height)
{
	window->width = width;
	window->height = height;
	window->gpu->intf->window__resize(window, width, height);
}