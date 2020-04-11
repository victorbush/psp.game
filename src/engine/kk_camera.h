#ifndef KK_CAMERA_H
#define KK_CAMERA_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "engine/kk_camera_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_math.h"

/*=========================================================
TYPES
=========================================================*/

struct kk_camera_s
{
	kk_vec3_t			dir;
	kk_vec3_t			pos;
	kk_vec3_t			up;
	kk_vec3_t			right;
	float				rot_x;	/* degrees rotation on x axis */
	float				rot_y;	/* degrees rotation on y axis */
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/kk_camera.public.h"

#endif /* KK_CAMERA_H */