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

//## public
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

//## public
/**
camera__destruct
*/
void camera__destruct(camera_t* cam)
{
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## public
/**
Gets the current position of the camera.
*/
//void camera__get_pos(camera_t* cam, vec3_t* output)
//{
//	//glm_vec3_copy(&cam->pos, output);
//}

//## public
/**
Gets a view matrix for the current camera state.
*/
//void camera__get_view_matrix(camera_t* cam, mat4_t* output)
//{
//	//vec3 look_at;
//	//glm_vec3_add(&cam->pos, &cam->dir, look_at);
//	//glm_lookat(&cam->pos, look_at, &cam->up, output);
//}

//## public
/**
Moves the camera forward or backward on the current view vector.
*/
void camera__move(camera_t* cam, float move_delta)
{
	/* Movement only occurs on the X / Z axes. Kill movement on Y axis. */
	vec3_t delta_vector;
	delta_vector.x = cam->dir.x;
	delta_vector.y = 0.0f;
	delta_vector.z = cam->dir.z;

	//delta_vector.x *= move_delta;
	//delta_vector.y *= move_delta;
	//delta_vector.z *= move_delta;
	glm_vec3_scale(&delta_vector, move_delta, &delta_vector);

	//cam->pos.x += delta_vector.x;
	//cam->pos.y += delta_vector.y;
	//cam->pos.z += delta_vector.z;
	glm_vec3_add(&cam->pos, &delta_vector, &cam->pos);
}

//## public
/**
Pans the camera horizontally or vertically along the right and up
axes. Does not rotate the camera.
*/
void camera__pan(camera_t* cam, float vert_delta, float horz_delta)
{
	vec3 temp_horz, temp_vert;

	glm_vec3_scale(&cam->right, horz_delta, temp_horz);
	glm_vec3_add(&cam->pos, temp_horz, &cam->pos);

	glm_vec3_scale(&cam->up, vert_delta, temp_vert);
	glm_vec3_add(&cam->pos, temp_vert, &cam->pos);
}

//## public
void camera__rot_x(camera_t* cam, float delta_x)
{
	cam->rot_x += delta_x;

	// Update camera
	if (cam->rot_x > 89.0)
		cam->rot_x = 89;
	if (cam->rot_x < -89.0)
		cam->rot_x = -89;

	// Calc new direction (note conversion from degrees to radians)
	cam->dir.x = (float)(cos(cam->rot_x * GLM_PIf / 180.0f) * sin(cam->rot_y * GLM_PIf / 180.0f));
	cam->dir.y = (float)sin(cam->rot_x * GLM_PIf / 180.0f);
	cam->dir.z = (float)(cos(cam->rot_x * GLM_PIf / 180.0f) * cos(cam->rot_y * GLM_PIf / 180.0f));

	// Update cam right vector
	glm_cross(&cam->dir, &cam->up, &cam->right);
}

//## public
void camera__rot_y(camera_t* cam, float delta_y)
{
	cam->rot_y += delta_y;

	// Update camera
	if (cam->rot_y > 360)
		cam->rot_y = 0;
	if (cam->rot_y < 0)
		cam->rot_y = 360;

	// Calc new direction (note conversion from degrees to radians)
	cam->dir.x = (float)(cos(cam->rot_x * GLM_PIf / 180.0f) * sin(cam->rot_y * GLM_PIf / 180.0f));
	cam->dir.y = (float)sin(cam->rot_x * GLM_PIf / 180.0f);
	cam->dir.z = (float)(cos(cam->rot_x * GLM_PIf / 180.0f) * cos(cam->rot_y * GLM_PIf / 180.0f));

	// Update cam right vector
	glm_cross(&cam->dir, &cam->up, &cam->right);
}