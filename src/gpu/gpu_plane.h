#ifndef GPU_PLANE_H
#define GPU_PLANE_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "utl/utl_math.h"

/*=========================================================
TYPES
=========================================================*/

/*
Default orientation for plane is to be flat on the ground.
Meaning the y coord is always 0 and the z coord is used for height.
*/
typedef struct 
{
	vec2_t			anchor;		/* (x,y) anchor point in model space */
	float			width;		/* width of plane (x-axis) */
	float			height;		/* height of plane (z-axis) */
	vec3_t			color;		/* color of the plane */

} gpu_plane_t;

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* GPU_PLANE_H */