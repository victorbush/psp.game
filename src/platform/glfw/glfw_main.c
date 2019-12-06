/*=========================================================
INCLUDES
=========================================================*/

#include <memory.h>

#include "common.h"
#include "engine/engine.h"
#include "gpu/gpu.h"
#include "gpu/vlk/vlk.h"
#include "platform/platform.h"
#include "platform/glfw/glfw.h"
#include "utl/utl.h"

/*=========================================================
VARIABLES
=========================================================*/

static engine_t			s_engine;
static gpu_t			s_gpu;
static platform_t		s_platform;

static GLFWwindow*		s_glfw_window;

/*=========================================================
DECLARATIONS
=========================================================*/

/** Callback for key events. */
static void _glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

/** Initializes the engine and platform objects. */
static void _init_engine();

/** Platfor callback to get frame delta time. */
static uint32_t _platform_get_time(platform_t* platform);

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Main entry point.
*/
int main(int argc, char* argv[])
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // NO_API for vulkan
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	/* create the GLFW window */
	s_glfw_window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jetz", NULL, NULL);
	
	/* setup user data pointer for the window */
	//memset(&_glfwUserData, 0, sizeof(_glfwUserData));
	//glfwSetWindowUserPointer(_windowHandle, &_glfwUserData);

	/* set GLFW window callbacks */
	//glfwSetFramebufferSizeCallback(s_glfw_window, framebufferResizeCallback);
	glfwSetKeyCallback(s_glfw_window, _glfw_key_callback);
	//glfwSetCursorPosCallback(s_glfw_window, cursorPosCallback);
	//glfwSetMouseButtonCallback(s_glfw_window, mouseButtonCallback);
	//glfwSetCharCallback(s_glfw_window, charCallback);
	//glfwSetScrollCallback(s_glfw_window, scrollCallback);

	glfwShowWindow(s_glfw_window);

	/*
	Init the engine
	*/
	_init_engine();

	/*
	Main loop
	*/
	while (!glfwWindowShouldClose(s_glfw_window))
	{
		glfwPollEvents();
		engine__run_frame(&s_engine);
	}

	/*
	Shutdown
	*/
	glfwDestroyWindow(s_glfw_window);
	glfwTerminate();

	engine__destruct(&s_engine);
}

static void _glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_W:
		s_platform.keydown__camera_forward = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;
	case GLFW_KEY_S:
		s_platform.keydown__camera_backward = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;
	case GLFW_KEY_A:
		s_platform.keydown__camera_strafe_left = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;
	case GLFW_KEY_D:
		s_platform.keydown__camera_strafe_right = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;
	}
}

static void _init_engine()
{
	/* Setup the GPU */
	vlk__init(&s_gpu, s_glfw_window);

	/* Setup the platform */
	clear_struct(&s_platform);
	s_platform.get_time = &_platform_get_time;

	/* Construct the engine */
	engine__construct(&s_engine, &s_gpu, &s_platform);
}

uint32_t _platform_get_time(platform_t* platform)
{
	return glfwGetTime();
}
