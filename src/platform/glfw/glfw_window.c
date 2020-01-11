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
#include "thirdparty/cimgui/imgui_jetz.h"

#include "autogen/glfw_window.static.h"

/*=========================================================
VARIABLES
=========================================================*/

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
	glfwSetCursorPosCallback(ctx->glfw_window, cursor_pos_callback);
	glfwSetMouseButtonCallback(ctx->glfw_window, mouse_button_callback);
	glfwSetCharCallback(ctx->glfw_window, char_callback);
	glfwSetScrollCallback(ctx->glfw_window, mouse_scroll_callback);
	glfwSetWindowCloseCallback(ctx->glfw_window, window_close_callback);

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

//## static
static void char_callback(GLFWwindow* window, unsigned int c)
{
	ImGuiIO* io = igGetIO();

	if (c > 0 && c < 0x10000)
	{
		ImGuiIO_AddInputCharacter(io, c);
	}
}

//## static
static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	platform_window_t* p_window = (platform_window_t*)glfwGetWindowUserPointer(window);

	ImGuiIO* io = igGetIO();
	io->MousePos.x = (float)xpos;
	io->MousePos.y = (float)ypos;

	//p_window->platform->mouse_x_prev = p_window->platform->mouse_x;
	//p_window->platform->mouse_y_prev = p_window->platform->mouse_y;
	//p_window->platform->mouse_x = (float)xpos;
	//p_window->platform->mouse_y = (float)ypos;
}

//## static
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	platform_window_t* p_window = (platform_window_t*)glfwGetWindowUserPointer(window);

	ImGuiIO* io = igGetIO();

	if (action == GLFW_PRESS)
		io->KeysDown[key] = true;
	if (action == GLFW_RELEASE)
		io->KeysDown[key] = false;

	/* According to ImGui example, modifiers are not reliable across systems */
	io->KeyCtrl = io->KeysDown[GLFW_KEY_LEFT_CONTROL] || io->KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io->KeyShift = io->KeysDown[GLFW_KEY_LEFT_SHIFT] || io->KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io->KeyAlt = io->KeysDown[GLFW_KEY_LEFT_ALT] || io->KeysDown[GLFW_KEY_RIGHT_ALT];
	io->KeySuper = io->KeysDown[GLFW_KEY_LEFT_SUPER] || io->KeysDown[GLFW_KEY_RIGHT_SUPER];

	//if (!io.WantCaptureKeyboard && _handler != nullptr)
	//{
	//	_handler->OnKey(*this, key, scancode, action, mods);
	//}

	// TODO
	//switch (key)
	//{
	//case GLFW_KEY_W:
	//	g_platform->keydown__camera_forward = action == GLFW_PRESS || action == GLFW_REPEAT;
	//	break;
	//case GLFW_KEY_S:
	//	g_platform->keydown__camera_backward = action == GLFW_PRESS || action == GLFW_REPEAT;
	//	break;
	//case GLFW_KEY_A:
	//	g_platform->keydown__camera_strafe_left = action == GLFW_PRESS || action == GLFW_REPEAT;
	//	break;
	//case GLFW_KEY_D:
	//	g_platform->keydown__camera_strafe_right = action == GLFW_PRESS || action == GLFW_REPEAT;
	//	break;
	//}
}

//## static
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	ImGuiIO* io = igGetIO();

	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		//_prevMouseRightButton = _mouseRightButton;
		//_mouseRightButton = (action == GLFW_PRESS);
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		//_prevMouseLeftButton = _mouseLeftButton;
		//_mouseLeftButton = (action == GLFW_PRESS);
	}

	//io->MouseDown[0] = _mouseLeftButton;
	//io->MouseDown[1] = _mouseRightButton;
	io->MouseDown[0] = (action == GLFW_PRESS) && button == GLFW_MOUSE_BUTTON_LEFT;
	io->MouseDown[1] = (action == GLFW_PRESS) && button == GLFW_MOUSE_BUTTON_RIGHT;

	// TOOD : Middle mouse

	//if (!io.WantCaptureMouse && _handler != nullptr)
	//{
	//	_handler->OnMouseButton(*this, button, action, mods);
	//}
}

//## static
static void mouse_scroll_callback(platform_t* platform, double xoffset, double yoffset)
{
	ImGuiIO* io = igGetIO();
	io->MouseWheelH += (float)xoffset;
	io->MouseWheel += (float)yoffset;
}

//## static
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

//## static
static void window_close_callback(GLFWwindow* window)
{
	platform_window_t* platform_window = (platform_window_t*)glfwGetWindowUserPointer(window);
	platform_window__request_close(platform_window);
}