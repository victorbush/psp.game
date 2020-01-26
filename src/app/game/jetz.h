#ifndef JETZ_H
#define JETZ_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "app/app_.h"
#include "app/game/jetz_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "engine/camera.h"
#include "engine/world.h"
#include "platform/platform_window.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/**
Jetz game.
*/
struct _jetz_s
{
	/*
	Create/destroy
	*/
	camera_t			camera;
	platform_window_t	window;
	world_t				world;

	/*
	Other
	*/
	float				frame_time;			/* Frame timestamp (in seconds) */
	float				frame_delta_time;	/* Time between current frame and last frame (in seconds) */
	boolean				should_exit;		/* Should the app exit? */
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/jetz.public.h"

#endif /* JETZ_H */