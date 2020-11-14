/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "ecs/ecs.h"
#include "ecs/components/ecs_physics.h"
#include "ecs/components/ecs_transform.h"
#include "engine/kk_log.h"
#include "platform/platform.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/



static void update(ecs_physics_t* phys, ecs_transform_t* transform, float delta_time);



void physics_system__run(ecs_t* ecs, float delta_time)
{
	ecs_physics_t*			phys = NULL;
	ecs_transform_t*		transform = NULL;

	uint32_t			i;

	for (i = 0; i < ecs->next_free_id; ++i)
	{
		phys = &ecs->physics_comp[i];
		transform = &ecs->transform_comp[i];

		/* Find entities */
		if (!phys->base.is_used || !transform->base.is_used)
		{
			continue;
		}

		update(phys, transform, delta_time);
	}
}

static void update(ecs_physics_t* phys, ecs_transform_t* transform, float delta_time)
{
	kk_vec3_t temp;

	/*
	Linear
	*/

	// pos = pos + vel * dt
	kk_math_vec3_scale(&phys->velocity, delta_time, &temp);
	kk_math_vec3_add(&transform->pos, &temp, &transform->pos);

	/* Update secondary values */
	{
		// velocity = momentum * inverse_mass
		kk_math_vec3_scale(&phys->momentum, phys->inverse_mass, &phys->velocity);
	}

	/*
	Rotational
	*/

	/* Integrate */
	// orientation = orientation + spin * dt
	transform->rot.x = transform->rot.x + (phys->spin.x * delta_time);
	transform->rot.y = transform->rot.y + (phys->spin.y * delta_time);
	transform->rot.z = transform->rot.z + (phys->spin.z * delta_time);
	transform->rot.w = transform->rot.w + (phys->spin.w * delta_time);

	kk_math_quat_normalize(&transform->rot);

	/* Update secondary values */
	{
		// angular velocity = angular momentum * inverse inertia
		kk_math_vec3_scale(&phys->angular_momentum, phys->inverse_inertia, &phys->angular_velocity);

		// spin = (0.5 * (w * q)), where:
		//	w is angular velocity as a quaternion (0, x, y, z) -- (x,y,z) are the angular velocity vector
		//	q is the current orientation quaternion
		//	The multiplication done between w and q is quaternion multiplication.

		kk_vec4_t w;
		w.x = phys->angular_velocity.x;
		w.y = phys->angular_velocity.y;
		w.z = phys->angular_velocity.z;
		w.w = 0;

		kk_math_quat_mul(&w, &transform->rot, &phys->spin);
		phys->spin.x = phys->spin.x * 0.5f;
		phys->spin.y = phys->spin.y * 0.5f;
		phys->spin.z = phys->spin.z * 0.5f;
		phys->spin.w = phys->spin.w * 0.5f;
	}
}