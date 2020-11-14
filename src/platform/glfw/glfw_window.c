/*=========================================================
INCLUDES
=========================================================*/

#include <stdio.h>

#include "common.h"
#include "global.h"
#include "engine/kk_log.h"
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
		kk_log__fatal("Failed to allocate memory.");
	}

	/* Create window */
	ctx->glfw_window = glfwCreateWindow(width, height, "Jetz", NULL, NULL);
	if (!ctx->glfw_window)
	{
		kk_log__fatal("Failed to create GLFW window.");
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
	platform_window__on_mouse_move(p_window, (float)xpos, (float)ypos);
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

	switch (key)
	{
	case GLFW_KEY_W:
		g_platform->keydown__up = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;
	case GLFW_KEY_S:
		g_platform->keydown__down = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;
	case GLFW_KEY_A:
		g_platform->keydown__left = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;
	case GLFW_KEY_D:
		g_platform->keydown__right = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;

	case GLFW_KEY_LEFT_SHIFT:
		g_platform->keydown__l = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;
	case GLFW_KEY_KP_ENTER:
		g_platform->keydown__r = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;

	case GLFW_KEY_KP_4:
		g_platform->keydown__square = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;
	case GLFW_KEY_KP_5:
		g_platform->keydown__cross = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;
	case GLFW_KEY_KP_6:
		g_platform->keydown__circle = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;
	case GLFW_KEY_KP_8:
		g_platform->keydown__triangle = action == GLFW_PRESS || action == GLFW_REPEAT;
		break;
	}
}

//## static
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	platform_window_t* platform_window = (platform_window_t*)glfwGetWindowUserPointer(window);
	platform_mouse_button_t p_button;
	platform_input_key_action_t p_action;

	/* Translate button */
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		p_button = MOUSE_BUTTON_LEFT;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		p_button = MOUSE_BUTTON_RIGHT;
		break;
	default:
		// TODO ?
		return;
	}

	/* Translate action */
	switch (action)
	{
	case GLFW_PRESS:
		p_action = KEY_ACTION_PRESS;
		break;
	case GLFW_RELEASE:
		p_action = KEY_ACTION_RELEASE;
		break;
	default:
		// TODO ?
		return;
	}

	platform_window__on_mouse_button(platform_window, p_button, p_action);
}

//## static
static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	platform_window_t* platform_window = (platform_window_t*)glfwGetWindowUserPointer(window);
	platform_window__on_mouse_scroll(platform_window, (float)xoffset, (float)yoffset);
}

//## static
static void resize_callback(GLFWwindow* window, int width, int height)
{
	platform_window_t* platform_window = (platform_window_t*)glfwGetWindowUserPointer(window);

	// TODO : Don't wait here if the window size is 0.

	/* width/height will be 0 if minimzed */
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
	platform_window__on_window_close(platform_window);
}