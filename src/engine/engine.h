#ifndef ENGINE_H
#define ENGINE_H

/*=========================================================
INCLUDES
=========================================================*/

#include "ecs/ecs.h"
#include "gpu/gpu.h"
#include "platforms/common.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct 
{
	ecs_t*			ecs;
	gpu_t* 			gpu;

} engine_t;

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Initializes an engine.
@param eng The context to init.
*/
void engine_init(engine_t* eng);

/**
Terminates an engine.
@param eng The context to terminate.
*/
void engine_term(engine_t* eng);

void engine_run_frame(engine_t* eng);

#endif /* ENGINE_H */