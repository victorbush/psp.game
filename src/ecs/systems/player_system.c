/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/components.h"
#include "ecs/ecs.h"
#include "engine/engine.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

void player_system__run(engine_t* eng, ecs_t* ecs)
{
	if (eng->platform->keydown__camera_forward)
	{
		camera__move(&eng->camera, eng->frame_time / 1000.0f * 1.0f);
	}
	else if (eng->platform->keydown__camera_backward)
	{
		camera__move(&eng->camera, eng->frame_time / 1000.0f * -1.0f);
	}
}
