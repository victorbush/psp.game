/*=========================================================
This file is automatically generated. Do not edit manually.
=========================================================*/

void pspgu_window__construct(
	gpu_window_t* window,
	gpu_t* gpu,
	uint32_t width,
	uint32_t height)
;

void pspgu_window__destruct(gpu_window_t* window, gpu_t* gpu)
;

void pspgu_window__begin_frame(gpu_window_t* window, gpu_frame_t* frame, kk_camera_t* camera)
;

void pspgu_window__end_frame(gpu_window_t* window, gpu_frame_t* frame)
;

void pspgu_window__render_imgui(gpu_window_t* window, gpu_frame_t* frame, ImDrawData* draw_data)
;

void pspgu_window__resize(gpu_window_t* window, uint32_t width, uint32_t height)
;
