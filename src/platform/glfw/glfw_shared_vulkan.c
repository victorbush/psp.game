/*=========================================================
INCLUDES
=========================================================*/

#include <vulkan/vulkan.h>

#include "common.h"
#include "global.h"
#include "engine/kk_log.h"
#include "platform/platform.h"
#include "platform/platform_window.h"
#include "platform/glfw/glfw.h"
#include "platform/glfw/glfw_window.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

VkResult glfw__create_surface(platform_window_t* window, VkInstance instance, VkSurfaceKHR* surface)
{
	_glfw_window_t* w = (_glfw_window_t*)window->context;
	return glfwCreateWindowSurface(instance, w->glfw_window, NULL, surface);
}

VkResult glfw__create_temp_surface(VkInstance instance, VkSurfaceKHR* surface)
{
	/* Create temp window */
	GLFWwindow* win = glfwCreateWindow(256, 256, "Temp", NULL, NULL);
	if (!win)
	{
		kk_log__fatal("Failed to create temp window.");
	}

	/* Create a temp surface. */
	VkResult result = glfwCreateWindowSurface(instance, win, NULL, surface);

	/* Destroy temp window. Caller will destroy surface. */
	glfwDestroyWindow(win);

	return result;
}
