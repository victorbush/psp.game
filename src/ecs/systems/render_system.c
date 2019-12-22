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

/* 	vec3_t pos;
	pos.x = -5.0f;
	pos.y = 0.0f;
	pos.z = -10.0f;
 */

	gpu_plane_t plane;
	clear_struct(&plane);
	plane.color.y = 1.0f;
	plane.width = 10.0f;
	plane.height = 10.0f;

	//mat4_t model_matrix;
	//glm_mat4_identity(&model_matrix);

//#include "thirdparty/cglm/include/cglm/affine.h"
	//glm_translate_z(&model_matrix, -10.0f);
	//glm_translate_y(&model_matrix, -5.0f);

	transform_comp_t plane_transform;
	clear_struct(&plane_transform);
	plane_transform.pos.z = -10.0f;
	plane_transform.pos.y = -5.0f;


	//eng->gpu->render_plane(eng->gpu, &plane, &plane_transform);
	gpu_plane__render(&plane, &eng->gpu, &plane_transform);

	//transform_comp_t lol_transform;
	//clear_struct(&lol_transform);
	////eng->gpu->render_model(eng->gpu, &ecs->static_model_comp[0].model, &lol_transform);
	//eng->gpu->render_static_model(eng->gpu, ecs->static_model_comp[0].model, &lol_transform);





	static_model_comp_t* 	sm;
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
}
