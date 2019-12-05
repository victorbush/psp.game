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

/** Initializes the engine and platform objects. */
static void _init_engine();

/** Platform callback for the start of a frame. */
static void _platform_begin_frame(platform_t* platform);

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
	//glfwSetKeyCallback(s_glfw_window, keyCallback);
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
		engine__run_frame(&s_engine);
	}

	/*
	Shutdown
	*/
	glfwDestroyWindow(s_glfw_window);
	glfwTerminate();

	engine__destruct(&s_engine);
}

static void _init_engine()
{
	/* Setup the GPU */
	vlk__init(&s_gpu, s_glfw_window);

	/* Setup the platform callbacks */
	clear_struct(&s_platform);
	s_platform.begin_frame = &_platform_begin_frame;

	/* Construct the engine */
	engine__construct(&s_engine, &s_gpu, &s_platform);
}

static void _platform_begin_frame(platform_t* platform)
{
	glfwPollEvents();
}
