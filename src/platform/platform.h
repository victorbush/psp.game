#ifndef PLATFORM_H
#define PLATFORM_H

/*=========================================================
INCLUDES
=========================================================*/

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

/*-------------------------------------
Platform interface
-------------------------------------*/
struct platform_s
{
	void*			context;	/* Context pointer for platform-specific data */

	platform_get_time_func		get_time;	/* gets the time (in ms) between the previous frame and the current frame */

	boolean			keydown__camera_forward;
	boolean			keydown__camera_backward;
	boolean			keydown__camera_strafe_left;
	boolean			keydown__camera_strafe_right;

};

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* PLATFORM_H */