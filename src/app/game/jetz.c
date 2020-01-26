/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "app/app.h"
#include "app/game/jetz.h"
#include "ecs/systems/player_system.h"
#include "ecs/systems/render_system.h"
#include "gpu/gpu.h"
#include "platform/platform.h"

//#include "autogen/jetz.static.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

//## public
void jetz__construct(app_t* app)
{
	_jetz_t* j = _jetz__from_base(app);

	/* Create window */
	platform_window__construct(&j->window, g_platform, g_gpu, 800, 600);
	platform_window__set_user_data(&j->window, (void*)j);
	//platform_window__set_on_mouse_button_callback(&j->window, window_on_mouse_button);
	//platform_window__set_on_mouse_move_callback(&j->window, window_on_mouse_move);
	//platform_window__set_on_window_close_callback(&j->window, window_on_close);
	
	/* Setup Camera */
	camera__construct(&j->camera);

	world__construct(&j->world, "worlds/world.lua");
}

//## public
void jetz__destruct(app_t* app)
{
	_jetz_t* j = _jetz__from_base(app);

	gpu__wait_idle(g_gpu);

	camera__destruct(&j->camera);
	platform_window__destruct(&j->window, g_platform, g_gpu);

	/* Free context memory */
	free(app->intf->context);
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## public
void jetz__init_app_intf(app_intf_t* intf)
{
	clear_struct(intf);

	/* Allocate context memory */
	intf->context = malloc(sizeof(_jetz_t));
	clear_struct((_jetz_t*)intf->context);
	intf->__construct = jetz__construct;
	intf->__destruct = jetz__destruct;
	intf->__run_frame = jetz__run_frame;
	intf->__should_exit = jetz__should_exit;
}

//## public
void jetz__run_frame(app_t* app)
{
	_jetz_t* j = _jetz__from_base(app);

	/* Get frame time delta */
	float last_time = j->frame_time;
	j->frame_time = g_platform->get_time(g_platform);
	j->frame_delta_time = j->frame_time - last_time; // TODO : Rollover?

	/* Begin frame */
	gpu_frame_t* frame = gpu_window__begin_frame(&j->window.gpu_window, &j->camera, j->frame_delta_time);

	player_system__run(&j->world.ecs);
	render_system__run(&j->world.ecs, &j->window.gpu_window, frame);

	/* End frame */
	gpu_window__end_frame(&j->window.gpu_window, frame);
}

//## public
boolean jetz__should_exit(app_t* app)
{
	_jetz_t* j = _jetz__from_base(app);
	return (j->should_exit);
}

//## public
_jetz_t* _jetz__from_base(app_t* app)
{
	return (_jetz_t*)app->intf->context;
}
