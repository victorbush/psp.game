/*=========================================================
INCLUDES
=========================================================*/

#include <stdio.h>

#include "common.h"
#include "global.h"
#include "log/log.h"
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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

static void resize_callback(GLFWwindow* window, int width, int height);

/*=========================================================
CONSTRUCTORS
=========================================================*/

void glfw_window__construct(platform_window_t* window, platform_t* platform, gpu_t* gpu, uint32_t width, uint32_t height)
{
	/* Allocate memory for implementation context data */
	_glfw_window_t* ctx = malloc(sizeof(_glfw_window_t));
	window->context = ctx;
	if (!window->context)
	{
		log__fatal("Failed to allocate memory.");
	}

	/* Create window */
	ctx->glfw_window = glfwCreateWindow(width, height, "Jetz", NULL, NULL);
	if (!ctx->glfw_window)
	{
		log__fatal("Failed to create GLFW window.");
	}

	/* setup user data pointer for the window */
	glfwSetWindowUserPointer(ctx->glfw_window, window);

	/* Setup window callbacks */
	glfwSetFramebufferSizeCallback(ctx->glfw_window, resize_callback);
	glfwSetKeyCallback(ctx->glfw_window, key_callback);
	//glfwSetCursorPosCallback(ctx->glfw_window, cursorPosCallback);
	//glfwSetMouseButtonCallback(ctx->glfw_window, mouseButtonCallback);
	//glfwSetCharCallback(ctx->glfw_window, charCallback);
	//glfwSetScrollCallback(ctx->glfw_window, scrollCallback);

	/* Show window */
	glfwShowWindow(ctx->glfw_window);
}

void glfw_window__destruct(platform_window_t* window, platform_t* platform, gpu_t* gpu)
{
	_glfw_window_t* ctx = (_glfw_window_t*)window->context;

	/* Destroy window */
	glfwDestroyWindow(ctx->glfw_window);

	/* Free implementation data */
	free(window->context);
}

/*=========================================================
FUNCTIONS
=========================================================*/

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	platform_window_t* p_window = (platform_window_t*)glfwGetWindowUserPointer(window);

	// TODO
	switch (key)
	{
	case GLFW_KEY_W:
		g_platform->keydown__camera_forward = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;
	case GLFW_KEY_S:
		g_platform->keydown__camera_backward = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;
	case GLFW_KEY_A:
		g_platform->keydown__camera_strafe_left = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;
	case GLFW_KEY_D:
		g_platform->keydown__camera_strafe_right = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;
	}
}

static void resize_callback(GLFWwindow* window, int width, int height)
{
	platform_window_t* platform_window = (platform_window_t*)glfwGetWindowUserPointer(window);

	// TODO : Don't wait here if the window size is 0.

	/* width/height will be 0 if minimzed */
	//int width = 0, height = 0;
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	/* Prevent negative window size */
	width = max(0, width);
	height = max(0, height);

	/* Resize swapchain/framebuffers */
	gpu_window__resize(&platform_window->gpu_window, width, height);
}
