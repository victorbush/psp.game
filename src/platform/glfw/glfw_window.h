#ifndef GLFW_WINDOW_H
#define GLFW_WINDOW_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "gpu/gpu_window_.h"
#include "platform/platform_.h"
#include "platform/platform_window_.h"
#include "platform/glfw/glfw_window_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "platform/glfw/glfw.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

struct _glfw_window_s
{
	GLFWwindow*		glfw_window;
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

void glfw_window__construct(platform_window_t* window, platform_t* platform, gpu_t* gpu, uint32_t width, uint32_t height);

void glfw_window__destruct(platform_window_t* window, platform_t* platform, gpu_t* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* GLFW_WINDOW_H */