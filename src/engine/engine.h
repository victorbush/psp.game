#ifndef ENGINE_H
#define ENGINE_H

/*=========================================================
INCLUDES
=========================================================*/

#include "ecs/ecs.h"
#include "gpu/gpu_intf.h"
#include "platforms/common.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct 
{
	ecs_type		ecs;
	gpu_type 		gpu;

} engine_type;

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Initializes an engine context.
@param eng The context to init.
*/
void engine_init(engine_type* eng);

/**
Terminates an engine context.
@param eng The context to terminate.
*/
void engine_term(engine_type* eng);

void engine_run_frame(engine_type* eng);

#endif /* ENGINE_H */