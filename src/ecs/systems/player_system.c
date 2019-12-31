/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
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
	if (g_platform->keydown__camera_forward)
	{
		camera__move(&eng->camera, eng->frame_time / 1000.0f * 5.0f);
	}
	else if (g_platform->keydown__camera_backward)
	{
		camera__move(&eng->camera, eng->frame_time / 1000.0f * -5.0f);
	}
}
