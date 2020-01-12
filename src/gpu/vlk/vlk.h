#ifndef VLK_H
#define VLK_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "engine/camera_.h"
#include "gpu/gpu_.h"
#include "gpu/gpu_frame_.h"
#include "gpu/gpu_window_.h"
#include "platform/platform_.h"
#include "platform/platform_window_.h"

/*=========================================================
INCLUDES
=========================================================*/

#pragma comment(lib, "vulkan-1.lib")

#include <vulkan/vulkan.h>

#include "thirdparty/cimgui/imgui_jetz.h"
#include "thirdparty/tinyobj/tinyobj.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/** Function that allows the platform to create a Vulkan surface for a window. */
typedef VkResult (*vlk_create_surface_func)(platform_window_t* window, VkInstance instance, VkSurfaceKHR* surface);

/** Function that creates a temporary surface used to select a physical GPU. */
typedef VkResult (*vlk_create_temp_surface_func)(VkInstance instance, VkSurfaceKHR* surface);

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Initializes a GPU interface for a Vulkan implementation.

@param intf The interface to initialize.
@param create_surface_func A function that creates a surface for a window.
@param create_temp_surface_func A function that creates a temporary surface used to selecting a physical GPU.
*/
void vlk__init_gpu_intf
	(
	gpu_intf_t*						intf,
	vlk_create_surface_func			create_surface_func,
	vlk_create_temp_surface_func	create_temp_surface_func
	);

void vlk_frame__construct(gpu_frame_t* frame, gpu_t* gpu);
void vlk_frame__destruct(gpu_frame_t* frame, gpu_t* gpu);

void vlk_window__begin_frame(gpu_window_t* window, gpu_frame_t* frame, camera_t* camera);
void vlk_window__construct(gpu_window_t* window, gpu_t* gpu, uint32_t width, uint32_t height);
void vlk_window__destruct(gpu_window_t* window, gpu_t* gpu);
void vlk_window__end_frame(gpu_window_t* window, gpu_frame_t* frame);
void vlk_window__render_imgui(gpu_window_t* window, gpu_frame_t* frame, ImDrawData* draw_data);
void vlk_window__resize(gpu_window_t* window, uint32_t width, uint32_t height);

int vlk_window__get_picker_id(gpu_window_t* window, gpu_frame_t* frame, float x, float y);

#endif /* VLK_H */