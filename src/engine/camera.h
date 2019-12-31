#ifndef CAMERA_H
#define CAMERA_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "engine/camera_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "utl/utl_math.h"

/*=========================================================
TYPES
=========================================================*/

struct camera_s
{
	vec3_t				dir;
	vec3_t				pos;
	vec3_t				up;
	vec3_t				right;
	float				rot_x;	/* degrees rotation on x axis */
	float				rot_y;	/* degrees rotation on y axis */
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Initializes a camera.
*/
void camera__construct(camera_t* cam);

/**
Destructs a camera.
*/
void camera__destruct(camera_t* cam);

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Gets the current position of the camera.
*/
void camera__get_pos(camera_t* cam, vec3_t* output);

/**
Gets a view matrix for the current camera state.
*/
void camera__get_view_matrix(camera_t* cam, mat4_t* output);

/**
Moves the camera forward or backward on the current view vector.
*/
void camera__move(camera_t* cam, float move_delta);

/**
Pans the camera horizontally or vertically along the right and up
axes. Does not rotate the camera.
*/
void camera__pan(camera_t* cam, float vert_delta, float horz_delta);

#endif /* CAMERA_H */