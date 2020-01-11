
void platform_window__construct(platform_window_t* window, platform_t* platform, gpu_t* gpu, uint32_t width, uint32_t height)
;

void platform_window__destruct(platform_window_t* window, platform_t* platform, gpu_t* gpu)
;

/**
Gets the user data pointer for this window. Returns NULL if none has been set.

@param window The window to get the user data for.
*/
void* platform_window__get_user_data(platform_window_t* window)
;

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
;

/**
Handles a mouse move event.

@param window The window context.
@param x The new x-coordinate of the mouse.
@param y The new y-coordinate of the mouse.
*/
void platform_window__on_mouse_move(platform_window_t* window, float x, float y)
;

/**
Handles a window close request.

@param window The window context.
*/
void platform_window__on_window_close(platform_window_t* window)
;

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
;

/**
Sets the callback for a mouse move event.

@param window The window context.
@param callback The callback function. Use NULL to clear callback.
*/
void platform_window__set_on_mouse_move_callback
(
	platform_window_t* window,
	platform_window_on_mouse_move_callback
	callback
)
;

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
;

/**
Sets user data pointer associated with this window.

@param window The window to set the user data for.
@param user_data The user data pointer.
*/
void platform_window__set_user_data(platform_window_t* window, void* user_data)
;
