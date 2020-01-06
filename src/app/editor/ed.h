#ifndef ED_H
#define ED_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "app/app_.h"
#include "app/editor/ed_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "ecs/ecs.h"
#include "engine/camera.h"
#include "engine/world.h"
#include "platform/platform_window.h"

/*=========================================================
TYPES
=========================================================*/

struct _ed_s
{
	camera_t			camera;
	ecs_t				ecs;
	platform_window_t	window;
	world_t				world;

	float				frame_time;			/* Frame timestamp (in seconds) */
	float				frame_delta_time;	/* Time between current frame and last frame (in seconds) */
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

void ed__construct(app_t* app);

void ed__destruct(app_t* app);

/*=========================================================
FUNCTIONS
=========================================================*/

void ed__init_app_intf(app_intf_t* intf);

void ed__run_frame(app_t* app);

_ed_t* _ed__from_base(app_t* app);

#endif /* ED_H */