#ifndef ENGINE_H
#define ENGINE_H

/*=========================================================
DECLARATIONS
=========================================================*/

typedef struct engine_s engine_t;

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/ecs.h"
#include "engine/camera.h"
#include "engine/world.h"
#include "gpu/gpu.h"
#include "platform/platform.h"

/*=========================================================
TYPES
=========================================================*/

struct engine_s
{
	camera_t		camera;
	ecs_t			ecs;
	gpu_t 			gpu;
	world_t			world;

	uint32_t		frame_time;			/* Time (in ms) between previous frame and current frame */
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Initializes an engine.
@param eng The context to init.
@param gpu The GPU interface to use.
*/
void engine__construct(engine_t* eng, gpu_intf_t* gpu_intf);

/**
Terminates an engine.
@param eng The context to terminate.
*/
void engine__destruct(engine_t* eng);

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Runs and renders a single frame.
@param eng The engine.
*/
void engine__run_frame(engine_t* eng);

#endif /* ENGINE_H */