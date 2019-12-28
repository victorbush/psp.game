/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "ecs/ecs.h"
#include "ecs/systems/player_system.h"
#include "ecs/systems/render_system.h"
#include "engine/engine.h"
#include "engine/world.h"
#include "gpu/gpu_plane.h"
#include "utl/utl.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void engine__construct(engine_t* eng, gpu_intf_t* gpu_intf)
{
	clear_struct(eng);

	/* Setup ECS */
	ecs__construct(&eng->ecs);

	/* Setup GPU */
	gpu__construct(&eng->gpu, gpu_intf);

	/* Setup Camera */
	camera__construct(&eng->camera);

	// TODO
	world__construct(&eng->world, &eng->ecs, "worlds/world.lua");

	//// TODO : temp
	//entity_id_t id;
	//id = ecs__alloc_entity(&eng->ecs);
	//eng->ecs.transform_comp[id].base.is_used = TRUE;
	//eng->ecs.transform_comp[id].pos.x = -10.0f;
	//eng->ecs.transform_comp[id].pos.y = 0.0f;
	//eng->ecs.transform_comp[id].pos.z = -10.0f;
	//eng->ecs.static_model_comp[id].base.is_used = TRUE;
	//eng->ecs.static_model_comp[id].model = gpu__load_static_model(&eng->gpu, "models\\cube\\cube.obj");
	//eng->ecs.static_model_comp[id].material = gpu__load_material(&eng->gpu, "models\\cube\\cube.mat.lua");

	//id = ecs__alloc_entity(&eng->ecs);
	//eng->ecs.transform_comp[id].base.is_used = TRUE;
	//eng->ecs.transform_comp[id].pos.x = -5.0f;
	//eng->ecs.transform_comp[id].pos.y = -5.0f;
	//eng->ecs.transform_comp[id].pos.z = -20.0f;
	//eng->ecs.static_model_comp[id].base.is_used = TRUE;
	//eng->ecs.static_model_comp[id].model = gpu__load_static_model(&eng->gpu, "models\\cube\\cube.obj");
	//eng->ecs.static_model_comp[id].material = gpu__load_material(&eng->gpu, "models\\cube\\cube.mat.lua");

	//id = ecs__alloc_entity(&eng->ecs);
	//eng->ecs.transform_comp[id].base.is_used = TRUE;
	//eng->ecs.transform_comp[id].pos.x = -50.0f;
	//eng->ecs.transform_comp[id].pos.y = -25.0f;
	//eng->ecs.transform_comp[id].pos.z = -10.0f;
	//eng->ecs.static_plane_comp[id].base.is_used = TRUE;
	//eng->ecs.static_plane_comp[id].material = gpu__load_material(&eng->gpu, "materials\\wood_floor.mat.lua");
	//gpu_plane__construct(&eng->ecs.static_plane_comp[id].plane, &eng->gpu, 100.0f, 100.0f);
	//// TODO - destruct plane
}

void engine__destruct(engine_t* eng)
{
	world__destruct(&eng->world);
	camera__destruct(&eng->camera);
	gpu__destruct(&eng->gpu);
	ecs__destruct(&eng->ecs);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void engine__run_frame(engine_t* eng)
{
	/* Get frame time delta */
	eng->frame_time = g_platform->get_time(g_platform);
	
	/* Begin frame */
	gpu__begin_frame(&eng->gpu, &eng->camera);

	player_system__run(eng, &eng->ecs);
	render_system__run(eng, &eng->ecs);

	/* End frame */
	gpu__end_frame(&eng->gpu);
}