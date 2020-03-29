/*=========================================================
INCLUDES
=========================================================*/

#include <memory.h>
#include <stdio.h>

#include "common.h"
#include "app/app.h"
#include "app/game/jetz.h"
#include "engine/kk_log.h"
#include "gpu/gpu.h"
#include "gpu/vlk/vlk.h"
#include "platform/platform.h"
#include "platform/platform_window.h"
#include "platform/glfw/glfw.h"
#include "platform/glfw/glfw_shared_vulkan.h"
#include "platform/glfw/glfw_window.h"
#include "thirdparty/cimgui/imgui_jetz.h"
#include "utl/utl.h"

/*=========================================================
VARIABLES
=========================================================*/

app_t*						g_app;
gpu_t*						g_gpu;
kk_log_t*					g_log;
platform_t*					g_platform;

static app_t				s_app;
static app_intf_t			s_app_intf;
static gpu_t				s_gpu;
static gpu_intf_t			s_gpu_intf;
static kk_log_t				s_log;
static platform_t			s_platform;

static ImGuiContext*		s_imgui_ctx;

/*=========================================================
DECLARATIONS
=========================================================*/

#include "autogen/glfw_main.static.h"

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
	while (!app__should_exit(&s_app))
	{
		glfwPollEvents();
		app__run_frame(&s_app);
	}

	/* Shutdown */
	shutdown();

	glfwTerminate();
}

//## static
/**
Shuts down up the app.
*/
static void shutdown()
{
	/* Shutdown app */
	app__destruct(&s_app);

	/* Shutdown GPU */
	gpu__destruct(&s_gpu);

	/* Shutdown logging */
	kk_log__destruct(g_log);
}

//## static
/**
Sets up the app.
*/
static void startup()
{
	/* Setup imgui */
	s_imgui_ctx = igCreateContext(NULL);
	igSetCurrentContext(s_imgui_ctx);

	/* Setup logging */
	g_log = &s_log;
	kk_log__construct(g_log);
	kk_log__register_target(g_log, glfw__log_to_stdout);
	log__dbg("Logging initialized.");

	/* Setup the platform */
	g_platform = &s_platform;
	clear_struct(g_platform);
	g_platform->get_time = &glfw__get_time;
	g_platform->load_file = &glfw__load_file;
	g_platform->window__construct = glfw_window__construct;
	g_platform->window__destruct = glfw_window__destruct;

	/* Init GPU */
	g_gpu = &s_gpu;
	vlk__init_gpu_intf(&s_gpu_intf, glfw__create_surface, glfw__create_temp_surface);
	gpu__construct(&s_gpu, &s_gpu_intf);

	/* Construct the app */
	g_app = &s_app;
	jetz__init_app_intf(&s_app_intf);
	app__construct(&s_app, &s_app_intf);
}
