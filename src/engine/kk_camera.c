/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_camera.h"
#include "engine/kk_log.h"
#include "engine/kk_math.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

//## public
/**
kk_camera__construct
*/
void kk_camera__construct(kk_camera_t* cam)
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
kk_camera__destruct
*/
void kk_camera__destruct(kk_camera_t* cam)
{
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## public
/**
Gets the current position of the camera.
*/
//void kk_camera__get_pos(kk_camera_t* cam, vec3_t* output)
//{
//	//glm_vec3_copy(&cam->pos, output);
//}

//## public
/**
Gets a view matrix for the current camera state.
*/
//void kk_camera__get_view_matrix(kk_camera_t* cam, mat4_t* output)
//{
//	//vec3 look_at;
//	//glm_vec3_add(&cam->pos, &cam->dir, look_at);
//	//glm_lookat(&cam->pos, look_at, &cam->up, output);
//}

//## public
/**
Moves the camera forward or backward on the current view vector.
*/
void kk_camera__move(kk_camera_t* cam, float move_delta)
{
	/* Movement only occurs on the X / Z axes. Kill movement on Y axis. */
	kk_vec3_t delta_vector;
	delta_vector.x = cam->dir.x;
	delta_vector.y = 0.0f;
	delta_vector.z = cam->dir.z;

	kk_math_vec3_scale(&delta_vector, move_delta, &delta_vector);
	kk_math_vec3_add(&cam->pos, &delta_vector, &cam->pos);
}

//## public
/**
Pans the camera horizontally or vertically along the right and up
axes. Does not rotate the camera.
*/
void kk_camera__pan(kk_camera_t* cam, float vert_delta, float horz_delta)
{
	kk_vec3_t temp_horz, temp_vert;

	kk_math_vec3_scale(&cam->right, horz_delta, &temp_horz);
	kk_math_vec3_add(&cam->pos, &temp_horz, &cam->pos);

	kk_math_vec3_scale(&cam->up, vert_delta, &temp_vert);
	kk_math_vec3_add(&cam->pos, &temp_vert, &cam->pos);
}

//## public
void kk_camera__rot_x(kk_camera_t* cam, float delta_x)
{
	cam->rot_x += delta_x;

	// Update camera
	if (cam->rot_x > 89.0)
		cam->rot_x = 89;
	if (cam->rot_x < -89.0)
		cam->rot_x = -89;

	// Calc new direction (note conversion from degrees to radians)
	cam->dir.x = (float)(cos(cam->rot_x * KK_PIf / 180.0f) * sin(cam->rot_y * KK_PIf / 180.0f));
	cam->dir.y = (float)sin(cam->rot_x * KK_PIf / 180.0f);
	cam->dir.z = (float)(cos(cam->rot_x * KK_PIf / 180.0f) * cos(cam->rot_y * KK_PIf / 180.0f));

	// Update cam right vector
	kk_math_cross(&cam->dir, &cam->up, &cam->right);
}

//## public
void kk_camera__rot_y(kk_camera_t* cam, float delta_y)
{
	cam->rot_y += delta_y;

	// Update camera
	if (cam->rot_y > 360)
		cam->rot_y = 0;
	if (cam->rot_y < 0)
		cam->rot_y = 360;

	// Calc new direction (note conversion from degrees to radians)
	cam->dir.x = (float)(cos(cam->rot_x * KK_PIf / 180.0f) * sin(cam->rot_y * KK_PIf / 180.0f));
	cam->dir.y = (float)sin(cam->rot_x * KK_PIf / 180.0f);
	cam->dir.z = (float)(cos(cam->rot_x * KK_PIf / 180.0f) * cos(cam->rot_y * KK_PIf / 180.0f));

	// Update cam right vector
	kk_math_cross(&cam->dir, &cam->up, &cam->right);
}