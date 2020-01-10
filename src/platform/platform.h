#ifndef PLATFORM_H
#define PLATFORM_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "gpu/gpu_.h"
#include "platform/platform_.h"
#include "platform/platform_window_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include <stdio.h>

#include "common.h"

/*=========================================================
CONSTANTS
=========================================================*/

#define PLATFORM__KEY_COUNT 512

/*=========================================================
TYPES
=========================================================*/

typedef uint8_t platform_mouse_button_t;
enum
{
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE,

	MOUSE_BUTTON__COUNT
};

enum platform_input_key_action
{
	KEY_PRESS,
	KEY_RELEASE,
	KEY_REPEAT
};

/*-------------------------------------
Platform callback functions
-------------------------------------*/
typedef float (*platform_get_time_func)(platform_t* platform);

typedef boolean (*platform_load_file_func)(const char* filename, boolean binary, long* out__size, void** out__buffer);
typedef FILE* (*platform_open_file_func)(const char* filename, long* out__size);
typedef void (*platform_close_file_func)(FILE* file);

typedef void (*platform_window_construct_func)(platform_window_t* window, platform_t* platform, gpu_t* gpu, uint32_t width, uint32_t height);
typedef void (*platform_window_destruct_func)(platform_window_t* window, platform_t* platform, gpu_t* gpu);

/*-------------------------------------
Platform interface
-------------------------------------*/
struct platform_s
{
	void* context;	/* Context pointer for platform-specific data */


	platform_get_time_func		get_time;	/* gets the current timestamp (in ms) */

	/*
	Allocates a temporary buffer and loads the specified file into the buffer.
	A pointer to the buffer is returned. The caller is responsible for freeing the buffer.
	The entire file is read into the buffer. File contents treated as binary.
	*/
	platform_load_file_func		load_file;

	/**
	Opens a file and returns a file pointer. If the file failed to open, NULL is returned.
	The file must be closed with the close_file function.
	*/
	platform_open_file_func		open_file;

	/**
	Closes a file.
	*/
	platform_close_file_func	close_file;


	platform_window_construct_func	window__construct;
	platform_window_destruct_func	window__destruct;


	boolean			keydown__camera_forward;
	boolean			keydown__camera_backward;
	boolean			keydown__camera_strafe_left;
	boolean			keydown__camera_strafe_right;


	//boolean	key_down[PLATFORM__KEY_COUNT];
	//float		mouse_down[MOUSE_BUTTON__COUNT];
	//float		mouse_x;
	//float		mouse_y;
	//float		mouse_x_prev;
	//float		mouse_y_prev;
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* PLATFORM_H */