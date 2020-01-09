/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "app/app.h"
#include "app/editor/ed.h"
#include "ecs/ecs.h"
#include "ecs/systems/player_system.h"
#include "ecs/systems/render_system.h"
#include "gpu/gpu.h"
#include "platform/platform.h"
#include "thirdparty/cimgui/imgui_jetz.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void ed__construct(app_t* app)
{
	_ed_t* ed = _ed__from_base(app);

	/* Setup ECS */
	ecs__construct(&ed->ecs);

	/* Create window */
	platform_window__construct(&ed->window, g_platform, g_gpu, SCREEN_WIDTH, SCREEN_HEIGHT);

	/* Setup Camera */
	camera__construct(&ed->camera);

	// TODO
	world__construct(&ed->world, &ed->ecs, "worlds/world.lua");
}

void ed__destruct(app_t* app)
{
	_ed_t* ed = _ed__from_base(app);

	gpu__wait_idle(&g_gpu);

	world__destruct(&ed->world);
	camera__destruct(&ed->camera);
	platform_window__destruct(&ed->window, g_platform, &g_gpu);
	ecs__destruct(&ed->ecs);

	/* Free context memory */
	free(app->intf->context);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void ed__init_app_intf(app_intf_t* intf)
{
	clear_struct(intf);

	/* Allocate context memory */
	intf->context = malloc(sizeof(_ed_t));
	intf->__construct = ed__construct;
	intf->__destruct = ed__destruct;
	intf->__run_frame = ed__run_frame;
}

void ed__run_frame(app_t* app)
{
	_ed_t* ed = _ed__from_base(app);

	/* Get frame time delta */
	float last_time = ed->frame_time;
	ed->frame_time = g_platform->get_time(g_platform);
	ed->frame_delta_time = ed->frame_time - last_time; // TODO : Rollover?

	/* Begin frame */
	gpu_frame_t* frame = gpu_window__begin_frame(&ed->window.gpu_window, &ed->camera, ed->frame_delta_time);

	player_system__run(ed, &ed->ecs);


	geo__render(&ed->world.geo, &ed->window.gpu_window, frame);
	render_system__run(&ed->ecs, &ed->window.gpu_window, frame);

	
	
	igShowDemoWindow(NULL);
	igBegin("HELLO", NULL, 0);
	igEnd();



	/* End frame */
	gpu_window__end_frame(&ed->window.gpu_window, frame);
}

_ed_t* _ed__from_base(app_t* app)
{
	return (_ed_t*)app->intf->context;
}