/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/ecs.h"
#include "ecs/systems/render_system.h"
#include "engine/engine.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void engine__construct(engine_t* eng, gpu_t* gpu, platform_t* platform)
{
	clear_struct(eng);
	eng->gpu = gpu;
	eng->platform = platform;
	
	/* Setup ECS */
	ecs__construct(&eng->ecs);

	/* Setup GPU */
	eng->gpu->init(eng->gpu);

	/* Setup Camera */
	camera__construct(&eng->camera);


	// TODO : temp
	entity_id_t id;
	id = ecs__alloc_entity(&eng->ecs);
	eng->ecs.transform_comp[id].base.is_used = TRUE;
	eng->ecs.transform_comp[id].pos.x = 10.0f;
	eng->ecs.transform_comp[id].pos.y = 0.0f;
	eng->ecs.transform_comp[id].pos.z = -20.0f;
	eng->ecs.static_model_comp[id].base.is_used = TRUE;

	//	eng->gpu->create_model(&eng->gpu, &eng->ecs->static_model_comp[id].model);


	id = ecs__alloc_entity(&eng->ecs);
	eng->ecs.transform_comp[id].base.is_used = TRUE;
	eng->ecs.transform_comp[id].pos.x = -10.0f;
	eng->ecs.transform_comp[id].pos.y = 0.0f;
	eng->ecs.transform_comp[id].pos.z = -50.0f;
}

void engine__destruct(engine_t* eng)
{
	//eng->gpu->destroy_model(&eng->gpu, &eng->ecs->static_model_comp[id].model);



	eng->gpu->term(eng->gpu);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void engine__run_frame(engine_t* eng)
{
	eng->platform->begin_frame(eng->platform);
	eng->gpu->begin_frame(eng->gpu, &eng->camera);

	render_system__run(eng, &eng->ecs);

	eng->gpu->end_frame(eng->gpu);
}