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
#include "geo/geo.h"
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
}

void engine__destruct(engine_t* eng)
{
	gpu__wait_idle(&eng->gpu);

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


	geo__render(&eng->world.geo);
	render_system__run(eng, &eng->ecs);

	/* End frame */
	gpu__end_frame(&eng->gpu);
}