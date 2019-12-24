/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/components.h"
#include "ecs/ecs.h"
#include "engine/engine.h"
#include "gpu/gpu_plane.h"
#include "gpu/gpu_static_model.h"
#include "utl/utl_log.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

void render_system__run(engine_t* eng, ecs_t* ecs)
{
	static_model_comp_t* 	sm;
	static_plane_comp_t*	plane;
	transform_comp_t* 		transform;
	uint32_t				i;

	for (i = 0; i < ecs->next_free_id; ++i)
	{
		sm = &ecs->static_model_comp[i];
		transform = &ecs->transform_comp[i];

		/* Find entities with static model and transform */
		if (!sm->base.is_used || !transform->base.is_used)
		{
			continue;
		}

		/* Make sure model is loaded */
		if (!sm->model)
		{
			FATAL("Static model does not have a model assigned.");
		}

		/* Render the model */
		gpu_static_model__render(sm->model, &eng->gpu, sm->material, transform);
	}
	
	for (i = 0; i < ecs->next_free_id; ++i)
	{
		plane = &ecs->static_plane_comp[i];
		transform = &ecs->transform_comp[i];

		/* Find entities with static plane and transform */
		if (!plane->base.is_used || !transform->base.is_used)
		{
			continue;
		}

		/* Render the plane */
		gpu_plane__render(&plane->plane, &eng->gpu, plane->material, transform);
	}
}
