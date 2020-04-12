/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/gpu_window.h"
#include "gpu/pspgu/pspgu_window.h"

/*=========================================================
MACROS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

//## public
void pspgu_window__construct(
	gpu_window_t* window,
	gpu_t* gpu,
	uint32_t width,
	uint32_t height)
{
}

//## public
void pspgu_window__destruct(gpu_window_t* window, gpu_t* gpu)
{
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## public
void pspgu_window__begin_frame(gpu_window_t* window, gpu_frame_t* frame, kk_camera_t* camera)
{
}

//## public
void pspgu_window__end_frame(gpu_window_t* window, gpu_frame_t* frame)
{
}

//## public
void pspgu_window__render_imgui(gpu_window_t* window, gpu_frame_t* frame, ImDrawData* draw_data)
{
}

//## public
void pspgu_window__resize(gpu_window_t* window, uint32_t width, uint32_t height)
{
}
