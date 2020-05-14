/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "ecs/ecs.h"
#include "engine/kk_camera.h"
#include "platform/platform.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

void player_system__run(ecs_t* ecs, kk_camera_t* cam, float frame_delta_time)
{
	if (g_platform->keydown__camera_forward)
	{
		kk_camera__move(cam, frame_delta_time * 0.1f);
	}
	else if (g_platform->keydown__camera_backward)
	{
		kk_camera__move(cam, frame_delta_time * -0.1f);
	}
}
