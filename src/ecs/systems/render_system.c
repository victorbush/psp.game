/*=========================================================
INCLUDES
=========================================================*/

#include "ecs/components.h"
#include "ecs/ecs.h"
#include "engine/engine.h"
#include "platforms/platform.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

/**
render_system_run
*/
void render_system_run(engine_type* eng, ecs_type* ecs)
{

	vec3_t pos;
	pos.x = -5.0f;
	pos.y = 0.0f;
	pos.z = -10.0f;

		eng->gpu->render_model(eng->gpu, &pos);

return;



	static_model_comp* 		sm;
	transform_comp* 		transform;
	int						i;

	for (i = 0; i < ecs->next_free_id; ++i)
	{
		sm = &ecs->static_model_comp[i];
		transform = &ecs->transform_comp[i];

		if (!sm->is_used || !transform->is_used)
		{
			continue;
		}

		//eng->gpu.test(&eng->gpu);
		//eng->gpu.render_model(&eng->gpu, &transform->pos);

		//psp_gpu_test();
	}
}
