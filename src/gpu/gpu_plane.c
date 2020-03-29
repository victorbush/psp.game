/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "engine/kk_log.h"
#include "gpu/gpu.h"
#include "gpu/gpu_material.h"
#include "gpu/gpu_plane.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void gpu_plane__construct(gpu_plane_t* plane, gpu_t* gpu)
{
	clear_struct(plane);

	gpu->intf->plane__construct(plane, gpu);
}

void gpu_plane__destruct(gpu_plane_t* plane, gpu_t* gpu)
{
	gpu->intf->plane__destruct(plane, gpu);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void gpu_plane__render(gpu_plane_t* plane, gpu_t* gpu, gpu_window_t* window, gpu_frame_t* frame, gpu_material_t* material)
{
	gpu->intf->plane__render(plane, gpu, window, frame, material);
}

void gpu_plane__update_verts(gpu_plane_t* plane, gpu_t* gpu, vec3_t verts[4])
{
	/*
	Plane vertices

		0 ----3
		|    /|
		|  /  |
		|/    |
		1-----2
	*/

	memcpy(plane->_verts, verts, sizeof(verts));
	gpu->intf->plane__update_verts(plane, gpu, verts);
}