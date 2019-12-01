/*=========================================================
INCLUDES
=========================================================*/

#include "ecs/components.h"
#include "ecs/ecs.h"
#include "engine/engine.h"
#include "platforms/platform.h"
#include "thirdparty/cglm/include/cglm/affine.h"

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


	gpu_plane_t plane;
	clear_struct(&plane);
	plane.color.y = 1.0f;
	plane.width = 10.0f;
	plane.height = 10.0f;

	mat4_t model_matrix;
	glm_mat4_identity(&model_matrix);

	glm_translate_y(&model_matrix, -5.0f);

	eng->gpu->render_plane(eng->gpu, &plane, &model_matrix);

		//eng->gpu->render_model(eng->gpu, &pos);

return;
//
//
//
//	static_model_comp* 		sm;
//	transform_comp* 		transform;
//	int						i;
//
//	for (i = 0; i < ecs->next_free_id; ++i)
//	{
//		sm = &ecs->static_model_comp[i];
//		transform = &ecs->transform_comp[i];
//
//		if (!sm->is_used || !transform->is_used)
//		{
//			continue;
//		}
//
//		//eng->gpu.test(&eng->gpu);
//		//eng->gpu.render_model(&eng->gpu, &transform->pos);
//
//		//psp_gpu_test();
//	}
}
