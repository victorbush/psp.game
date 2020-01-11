
void platform_window__construct(platform_window_t* window, platform_t* platform, gpu_t* gpu, uint32_t width, uint32_t height)
;

void platform_window__destruct(platform_window_t* window, platform_t* platform, gpu_t* gpu)
;

/**
Triggers the request close callback.

@param window The window context.
*/
void platform_window__request_close(platform_window_t* window)
;

/**
Sets the callback for when this window wants to close.

@param window The window context.
@param callback The callback function. Use NULL to clear callback.
@param user_data User data pointer that is passed to the callback.
*/
void platform_window__set_request_close_callback
	(
	platform_window_t*			window, 
	platform_window_request_close_callback_func 
								callback,
	void*						user_data
	)
;
