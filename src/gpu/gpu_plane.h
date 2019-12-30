#ifndef GPU_PLANE_H
#define GPU_PLANE_H

/*=========================================================
DECLARATIONS
=========================================================*/

typedef struct gpu_plane_s gpu_plane_t;

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/components.h"
#include "gpu/gpu.h"
#include "gpu/gpu_material.h"
#include "utl/utl_math.h"

/*=========================================================
TYPES
=========================================================*/

struct gpu_plane_s
{
	void*			data;
	vec3_t			_verts[4];
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

void gpu_plane__construct(gpu_plane_t* plane, gpu_t* gpu);

void gpu_plane__destruct(gpu_plane_t* plane, gpu_t* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Renders a plane.

@param plane The plane to render.
@param gpu The GPU context.
@param material The material to apply to the plane.
*/
void gpu_plane__render(gpu_plane_t* plane, gpu_t* gpu, gpu_material_t* material);

void gpu_plane__update_verts(gpu_plane_t* plane, gpu_t* gpu, vec3_t verts[4]);

#endif /* GPU_PLANE_H */