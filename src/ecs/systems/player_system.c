/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "ecs/ecs.h"
#include "ecs/components/ecs_physics.h"
#include "ecs/components/ecs_player.h"
#include "ecs/components/ecs_static_model.h"
#include "ecs/components/ecs_transform.h"
#include "engine/kk_camera.h"
#include "engine/kk_log.h"
#include "platform/platform.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

void player_system__run(ecs_t* ecs, kk_camera_t* cam, float frame_delta_time)
{
	ecs_physics_t*			phys = NULL;
	ecs_static_model_t*		sm = NULL;
	ecs_player_t*			player = NULL;
	ecs_transform_t*		transform = NULL;

	uint32_t			i;

	for (i = 0; i < ecs->next_free_id; ++i)
	{
		phys = &ecs->physics_comp[i];
		sm = &ecs->static_model_comp[i];
		player = &ecs->player_comp[i];
		transform = &ecs->transform_comp[i];
		
		/* Find entities with static model and transform */
		if (!sm->base.is_used || !phys->base.is_used || !player->base.is_used || !transform->base.is_used)
		{
			continue;
		}

		/* Make sure model is loaded */
		if (!sm->model)
		{
			kk_log__fatal("Static model does not have a model assigned.");
		}

		/* Only one player component, so break here */
		break;
	}

	if (!phys || !phys->base.is_used)
	{
		return;
	}

	phys->momentum.x = 0.0f;
	phys->momentum.y = 0.0f;
	phys->momentum.z = 0.0f;

	phys->angular_momentum.x = 0.0f;
	phys->angular_momentum.y = 0.0f;
	phys->angular_momentum.z = 0.0f;

	if (g_platform->keydown__l)
	{
		/*
		Movement modifier ON
		*/

		if (g_platform->keydown__up)
		{
			/* Move up */
			phys->momentum.y = 1.0f;
		}
		
		if (g_platform->keydown__down)
		{
			/* Move down */
			phys->momentum.y = -1.0f;
		}

		if (g_platform->keydown__right)
		{
			/* Strafe right */
			phys->momentum.x = -1.0f;
		}

		if (g_platform->keydown__left)
		{
			/* Strafe left */
			phys->momentum.x = 1.0f;
		}
	}
	else
	{
		/*
		Movement modifier OFF
		*/

		if (g_platform->keydown__up)
		{
			/* Accelerate forward */
			phys->momentum.z = 5.0f;
		}

		if (g_platform->keydown__down)
		{
			/* Accelerate back */
			phys->momentum.z = -5.0f;
		}

		if (g_platform->keydown__right)
		{
			/* Turn right */
			phys->angular_momentum.x = 0.0f;
			phys->angular_momentum.y = -(0.25f * KK_PIf);
			phys->angular_momentum.z = 0.0f;
		}

		if (g_platform->keydown__left)
		{
			/* Turn left */
			phys->angular_momentum.x = 0.0f;
			phys->angular_momentum.y = 0.25f * KK_PIf;
			phys->angular_momentum.z = 0.0f;
		}
	}

	/* Apply linear momemtum in the direction of orientation */
	glm_quat_rotatev(&transform->rot, &phys->momentum, &phys->momentum);
}
