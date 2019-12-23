/*=========================================================
INCLUDES
=========================================================*/

#include <memory.h>
#include <stdio.h>

#include "common.h"
#include "engine/engine.h"
#include "gpu/gpu.h"
#include "gpu/vlk/vlk.h"
#include "platform/platform.h"
#include "platform/glfw/glfw.h"
#include "utl/utl.h"
#include "utl/utl_log.h"

/*=========================================================
VARIABLES
=========================================================*/

platform_t*				g_platform;

static engine_t			s_engine;
static GLFWwindow*		s_glfw_window;
static gpu_intf_t		s_gpu_intf;
static platform_t		s_platform;

/*=========================================================
DECLARATIONS
=========================================================*/

/** Callback for key events. */
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

/** Initializes the engine and platform objects. */
static void init();

/** Platform callback to get frame delta time. */
static uint32_t platform_get_time(platform_t* platform);

/** Loads a file. */
boolean platform_load_file(const char* filename, boolean binary, long* out__size, void** out__buffer);

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
	init();

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

static void init()
{
	/* Setup the platform */
	g_platform = &s_platform;
	clear_struct(g_platform);
	g_platform->get_time = &platform_get_time;
	g_platform->load_file = &platform_load_file;

	/* Construct the GPU */
	vlk__init_gpu_intf(&s_gpu_intf, s_glfw_window);

	/* Construct the engine */
	engine__construct(&s_engine, &s_gpu_intf);
}

uint32_t platform_get_time(platform_t* platform)
{
	return (uint32_t)glfwGetTime();
}

boolean platform_load_file(const char* filename, boolean binary, long *out__size, void** out__buffer)
{
	FILE* f;
	errno_t err;

	/* Open the file */
	if (binary)
	{
		err = fopen_s(&f, filename, "rb");
	}
	else
	{
		err = fopen_s(&f, filename, "r");
	}

	/* Make sure file was opened */
	if (err != 0 || !f)
	{
		LOG_ERROR("Failed to open file.");
		return FALSE;
	}

	/* Get file length */
	fseek(f, 0, SEEK_END);
	*out__size = ftell(f);

	/* Alloc buffer */
	*out__buffer = malloc(*out__size);
	if (!*out__buffer)
	{
		LOG_ERROR("Failed to allocate file buffer.");
		return FALSE;
	}
	
	/* Read file into memory */
	fseek(f, 0, SEEK_SET);
	fread_s(*out__buffer, *out__size, *out__size, 1, f);

	/* Close file */
	fclose(f);

	/* Success */
	return TRUE;
}
