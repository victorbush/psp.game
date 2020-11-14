/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "app/app.h"
#include "app/game/jetz.h"
#include "ecs/systems/physics_system.h"
#include "ecs/systems/player_system.h"
#include "ecs/systems/render_system.h"
#include "engine/kk_log.h"
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
	kk_camera__construct(&j->camera);
	j->camera.pos.y = 1.0f;

	kk_world__construct(&j->world, "worlds/world.lua");
}

//## public
void jetz__destruct(app_t* app)
{
	_jetz_t* j = _jetz__from_base(app);

	gpu__wait_idle(g_gpu);

	kk_camera__destruct(&j->camera);
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
	j->frame_delta_time = g_platform->get_delta_time(g_platform);

	player_system__run(&j->world.ecs, &j->camera, j->frame_delta_time);
	physics_system__run(&j->world.ecs, j->frame_delta_time);


	////// update camera

	kk_vec3_t temp;

	/* Set distance behind the player */
	kk_vec3_t cam_dist;
	cam_dist.x = 0.0f;
	cam_dist.y = 0.0f;
	cam_dist.z = 5.0f;

	/* Rotate based on player orientation to get directly behind */
	glm_quat_rotatev(&j->world.ecs.transform_comp[0].rot, &cam_dist, &cam_dist);

	/* player pos - cam dist */
	kk_math_vec3_sub(&j->world.ecs.transform_comp[0].pos, &cam_dist, &temp);

	/* Move the camera up slightly */
	cam_dist.x = 0.0f;
	cam_dist.y = 2.0f;
	cam_dist.z = 0.0f;
	kk_math_vec3_add(&temp, &cam_dist, &temp);
	
	j->camera.pos = temp;

	/* Get the camera direction vector - this points from the camera to the player */
	kk_math_vec3_sub(&j->world.ecs.transform_comp[0].pos, &temp, &j->camera.dir);

	j->camera.up.x = 0.0f;
	j->camera.up.y = 1.0f;
	j->camera.up.z = 0.0f;

	/* 
	
	Reference note for quaternions:

	x	n.x * sin( theta / 2 )
	y	n.y * sin( theta / 2 )
	z	n.z * sin( theta / 2 )
	w	cos( theta / 2 )

	*/

	/////


	gpu_frame_t* frame = gpu_window__begin_frame(&j->window.gpu_window, &j->camera, j->frame_delta_time);
	render_system__run(&j->world.ecs, &j->window.gpu_window, frame);
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
