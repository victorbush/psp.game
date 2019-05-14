/*=========================================================
INCLUDES
=========================================================*/

#include <memory.h>

#include "engine/engine.h"
#include "gpu/vlk/vlk.h"
#include "platforms/common.h"
#include "platforms/glfw/glfw.h"

/*=========================================================
VARIABLES
=========================================================*/

static engine_type		s_engine;
static gpu_type			s_gpu;
static ecs_type			s_ecs;

static GLFWwindow*		s_glfw_window;

/*=========================================================
FUNCTIONS
=========================================================*/

/**
main
*/
int main(int argc, char* argv[])
{
	vlk_init_intf(&s_gpu);
	s_engine.gpu = &s_gpu;

	memset(&s_ecs, 0, sizeof(s_ecs));
	s_engine.ecs = &s_ecs;

	engine_init(&s_engine);
	
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

	while (!glfwWindowShouldClose(s_glfw_window))
	{
		glfwPollEvents();

		engine_run_frame(&s_engine);
	}

	glfwDestroyWindow(s_glfw_window);
	glfwTerminate();

	engine_term(&s_engine);
}
