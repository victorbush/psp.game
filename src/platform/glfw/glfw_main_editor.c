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
#include "platform/platform_window.h"
#include "platform/glfw/glfw.h"
#include "platform/glfw/glfw_shared_vulkan.h"
#include "platform/glfw/glfw_window.h"
#include "utl/utl.h"

/*=========================================================
VARIABLES
=========================================================*/

engine_t*					g_engine;
log_t*						g_log;
platform_t*					g_platform;

static engine_t				s_engine;
//GLFWwindow*			g_glfw_window;
static gpu_intf_t			s_gpu_intf;
static log_t				s_log;
static platform_t			s_platform;

/*=========================================================
DECLARATIONS
=========================================================*/

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

	/* Setup */
	startup();

	/*
	Main loop
	*/
	_glfw_window_t* main_window = (_glfw_window_t*)s_engine.window.context;
	while (!glfwWindowShouldClose(main_window->glfw_window))
	{
		glfwPollEvents();
		engine__run_frame(&s_engine);
	}

	/* Shutdown */
	shutdown();

	glfwTerminate();
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
	g_platform->window__construct = glfw_window__construct;
	g_platform->window__destruct = glfw_window__destruct;

	/* Init GPU interface */
	vlk__init_gpu_intf(&s_gpu_intf, glfw__create_surface, glfw__create_temp_surface);

	/* Construct the engine */
	g_engine = &s_engine;
	engine__construct(&s_engine, &s_gpu_intf);
}
