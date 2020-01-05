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

	/* Create window */
	platform_window__construct(&eng->window, g_platform, &eng->gpu, SCREEN_WIDTH, SCREEN_HEIGHT);

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
	platform_window__destruct(&eng->window, g_platform, &eng->gpu);
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
	gpu_frame_t* frame = gpu_window__begin_frame(&eng->window.gpu_window, &eng->camera);

	player_system__run(eng, &eng->ecs);


	geo__render(&eng->world.geo, &eng->window.gpu_window, frame);
	render_system__run(eng, &eng->ecs, &eng->window.gpu_window, frame);

	/* End frame */
	gpu_window__end_frame(&eng->window.gpu_window, frame);
}