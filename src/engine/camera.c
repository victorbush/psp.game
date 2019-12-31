/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/camera.h"
#include "log/log.h"
#include "utl/utl_math.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
camera__construct
*/
void camera__construct(camera_t* cam)
{
	clear_struct(cam);
	
	cam->pos.x = 0.0f;
	cam->pos.y = 0.0f;
	cam->pos.z = 5.0f;

	cam->right.x = 1.0f;
	cam->right.y = 0.0f;
	cam->right.z = 0.0f;

	cam->up.x = 0.0f;
	cam->up.y = 1.0f;
	cam->up.z = 0.0f;

	cam->dir.x = 0.0f;
	cam->dir.y = 0.0f;
	cam->dir.z = -1.0f;

	cam->rot_x = 0.0f;
	cam->rot_y = 180.0f;
}

/**
camera__destruct
*/
void camera__destruct(camera_t* cam)
{
}

/*=========================================================
FUNCTIONS
=========================================================*/

void camera__get_pos(camera_t* cam, vec3_t* output)
{
	//DBG_ASSERT(cam, "NULL camera.");

	//glm_vec3_copy(&cam->pos, output);
}

void camera__get_view_matrix(camera_t* cam, mat4_t* output)
{
	//DBG_ASSERT(cam, "NULL camera.");

	//vec3 look_at;
	//glm_vec3_add(&cam->pos, &cam->dir, look_at);
	//glm_lookat(&cam->pos, look_at, &cam->up, output);
}

void camera__move(camera_t* cam, float move_delta)
{
	//DBG_ASSERT(cam, "NULL camera.");

	/* Movement only occurs on the X / Z axes. Kill movement on Y axis. */
	vec3_t delta_vector;
	delta_vector.x = cam->dir.x;
	delta_vector.y = 0.0f;
	delta_vector.z = cam->dir.z;

	delta_vector.x *= move_delta;
	delta_vector.y *= move_delta;
	delta_vector.z *= move_delta;
	//glm_vec3_scale(delta_vector, move_delta, delta_vector);

	cam->pos.x += delta_vector.x;
	cam->pos.y += delta_vector.y;
	cam->pos.z += delta_vector.z;
	//glm_vec3_add(&cam->pos, delta_vector, &cam->pos);
}

void camera__pan(camera_t* cam, float vert_delta, float horz_delta)
{
	//DBG_ASSERT(cam, "NULL camera.");

	//vec3 temp_horz, temp_vert;

	//glm_vec3_scale(&cam->right, horz_delta, temp_horz);
	//glm_vec3_add(&cam->pos, temp_horz, &cam->pos);

	//glm_vec3_scale(&cam->up, vert_delta, temp_vert);
	//glm_vec3_add(&cam->pos, temp_vert, &cam->pos);
}