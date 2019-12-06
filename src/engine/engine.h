#ifndef ENGINE_H
#define ENGINE_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/ecs.h"
#include "engine/camera.h"
#include "gpu/gpu.h"
#include "platform/platform.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct 
{
	/*
	Dependencies
	*/
	gpu_t* 			gpu;
	platform_t*		platform;

	/*
	Other
	*/
	camera_t		camera;
	ecs_t			ecs;

	uint32_t		frame_time;			/* Time (in ms) between previous frame and current frame */

} engine_t;

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Initializes an engine.
@param eng The context to init.
@param gpu The GPU to use for this engine.
@param platform The platform to use for this engine.
*/
void engine__construct(engine_t* eng, gpu_t* gpu, platform_t* platform);

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