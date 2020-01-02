/*=========================================================
INCLUDES
=========================================================*/

#include <memory.h>
#include <stdio.h>

#include "common.h"
#include "engine/engine.h"
#include "gpu/gpu.h"
#include "gpu/vlk/vlk.h"
#include "log/log.h"
#include "platform/platform.h"
#include "platform/glfw/glfw.h"
#include "utl/utl.h"

/*=========================================================
VARIABLES
=========================================================*/

engine_t*				g_engine;
log_t*					g_log;
platform_t*				g_platform;

static engine_t			s_engine;
static GLFWwindow*		s_glfw_window;
static gpu_intf_t		s_gpu_intf;
static log_t			s_log;
static platform_t		s_platform;

/*=========================================================
DECLARATIONS
=========================================================*/

/** Callback for framebuffer resize. */
static void glfw_framebuffer_resize(GLFWwindow* window, int width, int height);

/** Callback for key events. */
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

/** Shuts down the engine and platform objects. */
static void shutdown();

/** Initializes the engine and platform objects. */
static void startup();

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
	glfwSetKeyCallback(s_glfw_window, glfw_key_callback);
	//glfwSetCursorPosCallback(s_glfw_window, cursorPosCallback);
	//glfwSetMouseButtonCallback(s_glfw_window, mouseButtonCallback);
	//glfwSetCharCallback(s_glfw_window, charCallback);
	//glfwSetScrollCallback(s_glfw_window, scrollCallback);

	glfwShowWindow(s_glfw_window);

	/*
	Init the engine
	*/
	startup();

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

	shutdown();
}

void glfw_framebuffer_resize(GLFWwindow* window, int width, int height)
{

}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
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

static void shutdown()
{
	/* Shutdown engine */
	engine__destruct(&s_engine);

	/* Shutdown logging */
	log__destruct(g_log);
}

static void startup()
{
	/* Setup logging */
	g_log = &s_log;
	log__construct(g_log);
	log__register_target(g_log, glfw__log_to_stdout);
	log__dbg("Logging initialized.");

	/* Setup the platform */
	g_platform = &s_platform;
	clear_struct(g_platform);
	g_platform->get_time = &glfw__get_time;
	g_platform->load_file = &glfw__load_file;

	/* Init GPU interface */
	vlk__init_gpu_intf(&s_gpu_intf, s_glfw_window);

	/* Construct the engine */
	g_engine = &s_engine;
	engine__construct(&s_engine, &s_gpu_intf);
}
