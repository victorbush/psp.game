/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_log.h"
#include "gpu/gpu.h"
#include "gpu/gpu_window.h"
#include "platform.h"
#include "platform/platform_window.h"
#include "thirdparty/cimgui/imgui_jetz.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

//## public
void platform_window__construct(platform_window_t* window, platform_t* platform, gpu_t* gpu, uint32_t width, uint32_t height)
{
	clear_struct(window);
	window->platform = platform;

	/* Platform-specific construction */
	platform->window__construct(window, platform, gpu, width, height);

	/* Create GPU window (framebuffers, swpchain, etc) */
	gpu_window__construct(&window->gpu_window, gpu, window, width, height);
}

//## public
void platform_window__destruct(platform_window_t* window, platform_t* platform, gpu_t* gpu)
{
	/* Destroy GPU window */
	gpu_window__destruct(&window->gpu_window);

	/* Platform specific destruction */
	platform->window__destruct(window, platform, gpu);
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## public
/**
Gets the user data pointer for this window. Returns NULL if none has been set.

@param window The window to get the user data for.
*/
void* platform_window__get_user_data(platform_window_t* window)
{
	return window->user_data;
}

//## public
/**
Handles a mouse button event.

@param window The window context.
@param button The button that trigger the action.
@param action The type of action that occurred.
*/
void platform_window__on_mouse_button
	(
	platform_window_t*			window,
	platform_mouse_button_t		button,
	platform_input_key_action_t action
	)
{
#ifndef JETZ_CONFIG_PLATFORM_PSP

	/* Validate button value */
	if (button >= MOUSE_BUTTON__COUNT)
	{
		kk_log__error_fmt("Unknown mouse button '%i'.", button);
		return;
	}

	/* Validate action value */
	if (action >= KEY_ACTION__COUNT)
	{
		kk_log__error_fmt("Unknown input key action '%i'.", action);
		return;
	}

	/* Save mouse button state */
	window->mouse_down[button] = action == KEY_ACTION_PRESS || action == KEY_ACTION_REPEAT;

	/* Update imgui */
	ImGuiIO* imgui = igGetIO();
	imgui->MouseDown[0] = window->mouse_down[MOUSE_BUTTON_LEFT];
	imgui->MouseDown[1] = window->mouse_down[MOUSE_BUTTON_RIGHT];

	/* Callback */
	if (!imgui->WantCaptureMouse && window->on_mouse_button_callback)
	{
		window->on_mouse_button_callback(window, button, action);
	}

#endif
}

//## public
/**
Handles a mouse move event.

@param window The window context.
@param x The new x-coordinate of the mouse.
@param y The new y-coordinate of the mouse.
*/
void platform_window__on_mouse_move(platform_window_t* window, float x, float y)
{
#ifndef JETZ_CONFIG_PLATFORM_PSP

	/* imgui */
	ImGuiIO* imgui = igGetIO();
	imgui->MousePos.x = x;
	imgui->MousePos.y = y;

	/* Save mouse position info */
	window->mouse_x_prev = window->mouse_x;
	window->mouse_y_prev = window->mouse_y;
	window->mouse_x = x;
	window->mouse_y = y;

	/* Callback */
	if (!imgui->WantCaptureMouse && window->on_mouse_move_callback)
	{
		window->on_mouse_move_callback(window, x, y);
	}

#endif
}

//## public
/**
Handles a mouse wheel scroll event.

@param window The window context.
@param xoffset The scroll x offset.
@param yoffset The scroll y offset.
*/
void platform_window__on_mouse_scroll(platform_window_t* window, float xoffset, float yoffset)
{
#ifndef JETZ_CONFIG_PLATFORM_PSP

	/* imgui */
	ImGuiIO* imgui = igGetIO();
	imgui->MouseWheelH += xoffset;
	imgui->MouseWheel += yoffset;

	/* Callback */
	if (!imgui->WantCaptureMouse && window->on_mouse_scroll_callback)
	{
		window->on_mouse_scroll_callback(window, xoffset, yoffset);
	}

#endif
}

//## public
/**
Handles a window close request.

@param window The window context.
*/
void platform_window__on_window_close(platform_window_t* window)
{
	/* Callback */
	if (window->on_window_close_callback)
	{
		window->on_window_close_callback(window);
	}
}

//## public
/**
Sets the callback for a mouse button event.

@param window The window context.
@param callback The callback function. Use NULL to clear callback.
*/
void platform_window__set_on_mouse_button_callback
	(
	platform_window_t*			window,
	platform_window_on_mouse_button_callback
								callback
	)
{
	window->on_mouse_button_callback = callback;
}

//## public
/**
Sets the callback for a mouse move event.

@param window The window context.
@param callback The callback function. Use NULL to clear callback.
*/
void platform_window__set_on_mouse_move_callback
	(
	platform_window_t*			window,
	platform_window_on_mouse_move_callback
								callback
	)
{
	window->on_mouse_move_callback = callback;
}

//## public
/**
Sets the callback for a mouse wheel scroll event.

@param window The window context.
@param callback The callback function. Use NULL to clear callback.
*/
void platform_window__set_on_mouse_scroll_callback
	(
	platform_window_t*			window,
	platform_window_on_mouse_scroll_callback
								callback
	)
{
	window->on_mouse_scroll_callback = callback;
}

//## public
/**
Sets the callback for when this window wants to close.

@param window The window context.
@param callback The callback function. Use NULL to clear callback.
*/
void platform_window__set_on_window_close_callback
	(
	platform_window_t*			window, 
	platform_window_on_window_close_callback
								callback
	)
{
	window->on_window_close_callback = callback;
}

//## public
/**
Sets user data pointer associated with this window.

@param window The window to set the user data for.
@param user_data The user data pointer.
*/
void platform_window__set_user_data(platform_window_t* window, void* user_data)
{
	window->user_data = user_data;
}
