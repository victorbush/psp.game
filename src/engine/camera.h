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
FUNCTIONS
=========================================================*/

#include "autogen/camera.public.h"

#endif /* CAMERA_H */