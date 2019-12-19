#ifndef VLK_H
#define VLK_H

/*=========================================================
INCLUDES
=========================================================*/

#include "gpu/gpu.h"
#include "platform/platform.h"
#include "platform/glfw/glfw.h"

#pragma comment(lib, "vulkan-1.lib")

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Initializes a GPU interface for a Vulkan implementation.

@param intf The interface to initialize.
@param window The GLFW window to use.
*/
void vlk__init_gpu_intf(gpu_intf_t* intf, GLFWwindow* window);

#endif /* VLK_H */