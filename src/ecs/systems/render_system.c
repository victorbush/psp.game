/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/ecs.h"
#include "ecs/ecs_component.h"
#include "ecs/components/ecs_static_model.h"
#include "ecs/components/ecs_transform.h"
#include "engine/engine.h"
#include "gpu/gpu_plane.h"
#include "gpu/gpu_static_model.h"
#include "log/log.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

void render_system__run(engine_t* eng, ecs_t* ecs)
{
	ecs_static_model_t* 	sm;
	ecs_transform_t* 		transform;
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
			log__fatal("Static model does not have a model assigned.");
		}

		/* Render the model */
		gpu_static_model__render(sm->model, &eng->gpu, sm->material, transform);
	}
}
