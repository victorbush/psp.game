#ifndef PLATFORM_H
#define PLATFORM_H

/*=========================================================
DECLARATIONS
=========================================================*/

typedef struct platform_s platform_t;
typedef struct platform_intf_s platform_intf_t;

/*=========================================================
INCLUDES
=========================================================*/

#include <stdio.h>

#include "common.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/*-------------------------------------
Platform callback functions
-------------------------------------*/
typedef uint32_t (*platform_get_time_func)(platform_t* platform);

typedef void (*platform_construct_func)(platform_t* platform, platform_intf_t* intf);
typedef void (*platform_destruct_func)(platform_t* platform);
typedef void (*platform_run_func)(platform_t* platform);
typedef boolean (*platform_load_file_func)(const char* filename, boolean binary, long* out__size, void** out__buffer);
typedef FILE* (*platform_open_file_func)(const char* filename, long* out__size);
typedef void (*platform_close_file_func)(FILE* file);

/*-------------------------------------
Platform interface
-------------------------------------*/
struct platform_intf_s
{
	void* context;	/* Context pointer for platform-specific data */


	platform_construct_func		construct;
	platform_destruct_func		destruct;
	platform_run_func			run;

	platform_get_time_func		get_time;	/* gets the time (in ms) between the previous frame and the current frame */

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
};

struct platform_s
{
	platform_intf_t*	intf;

	boolean			keydown__camera_forward;
	boolean			keydown__camera_backward;
	boolean			keydown__camera_strafe_left;
	boolean			keydown__camera_strafe_right;
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

void platform__construct(platform_t* platform, platform_intf_t* intf);

void platform__destruct(platform_t* platform);

/*=========================================================
FUNCTIONS
=========================================================*/

void platform__run(platform_t* platform);

#endif /* PLATFORM_H */