#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "gpu/gpu_.h"
#include "platform/platform_.h"
#include "platform/platform_window_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/gpu_window.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

enum platform_mouse_button_e
{
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE,

	MOUSE_BUTTON__COUNT
};

enum platform_input_key_action_e
{
	KEY_ACTION_PRESS,
	KEY_ACTION_RELEASE,
	KEY_ACTION_REPEAT,

	KEY_ACTION__COUNT
};

typedef void (*platform_window_on_mouse_button_callback)(platform_window_t* window, platform_mouse_button_t button, platform_input_key_action_t action);
typedef void (*platform_window_on_mouse_move_callback)(platform_window_t* window, float x, float y);
typedef void (*platform_window_on_window_close_callback)(platform_window_t* window);

struct platform_window_s
{
	void*						context;	/* Context pointer for platform-specific data. */
	platform_t*					platform;	/* Platform instance that owns this window. */
	
	/*
	Create/destroy
	*/
	gpu_window_t				gpu_window;

	/*
	Callbacks
	*/

	/* Callback for when a mouse button action is received. */
	platform_window_on_mouse_button_callback	on_mouse_button_callback;

	/* Callback for when the mouse moves. */
	platform_window_on_mouse_move_callback		on_mouse_move_callback;

	/* Callback for when the window wants to close (i.e., the "X" button was clicked) */
	platform_window_on_window_close_callback	on_window_close_callback;

	/*
	Other
	*/
	void*						user_data;	/* User context data that can be requested. */



	//boolean	key_down[PLATFORM__KEY_COUNT];
	float		mouse_down[MOUSE_BUTTON__COUNT];
	float		mouse_x;
	float		mouse_y;
	float		mouse_x_prev;
	float		mouse_y_prev;
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/platform_window.public.h"

#endif /* PLATFORM_WINDOW_H */