#ifndef GLFW_SHARED_VULKAN_H
#define GLFW_SHARED_VULKAN_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "platform/platform_window_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include <vulkan/vulkan.h>

#include "common.h"
#include "platform/glfw/glfw.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

VkResult glfw__create_surface(platform_window_t* window, VkInstance instance, VkSurfaceKHR* surface);

VkResult glfw__create_temp_surface(VkInstance instance, VkSurfaceKHR* surface);

#endif /* GLFW_SHARED_VULKAN_H */