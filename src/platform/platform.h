#ifndef PLATFORM_H
#define PLATFORM_H

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

typedef struct platform_s platform_t;

/*-------------------------------------
Platform callback functions
-------------------------------------*/
typedef uint32_t (*platform_get_time_func)(platform_t* platform);

typedef boolean (*platform_load_file_func)(const char* filename, boolean binary, long* out__size, void** out__buffer);
typedef FILE* (*platform_open_file_func)(const char* filename, long* out__size);
typedef void (*platform_close_file_func)(FILE* file);

/*-------------------------------------
Platform interface
-------------------------------------*/
struct platform_s
{
	void*			context;	/* Context pointer for platform-specific data */

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

	boolean			keydown__camera_forward;
	boolean			keydown__camera_backward;
	boolean			keydown__camera_strafe_left;
	boolean			keydown__camera_strafe_right;

};

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* PLATFORM_H */