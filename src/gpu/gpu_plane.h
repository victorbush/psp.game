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

typedef struct gpu_plane_s gpu_plane_t;

/*
Default orientation for plane is to be flat on the ground.
Meaning the y coord is always 0 and the z coord is used for height.
*/
struct gpu_plane_s
{
	vec2_t			anchor;		/* (x,y) anchor point in model space */
	float			width;		/* width of plane (x-axis) */
	float			height;		/* height of plane (z-axis) */
	vec3_t			color;		/* color of the plane */
};

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Renders a plane.

@param plane The plane to render.
@param gpu The GPU context.
@param transform The transformation to apply to the plane.
*/
void gpu_plane__render(gpu_plane_t* plane, gpu_t* gpu, transform_comp_t* transform);

#endif /* GPU_PLANE_H */