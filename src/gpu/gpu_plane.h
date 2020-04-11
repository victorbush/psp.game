#ifndef GPU_PLANE_H
#define GPU_PLANE_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "gpu/gpu_.h"
#include "gpu/gpu_frame_.h"
#include "gpu/gpu_material_.h"
#include "gpu/gpu_plane_.h"
#include "gpu/gpu_window_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_math.h"

/*=========================================================
TYPES
=========================================================*/

struct gpu_plane_s
{
	void*			data;
	kk_vec3_t		_verts[4];
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

void gpu_plane__construct(gpu_plane_t* plane, gpu_t* gpu);

void gpu_plane__destruct(gpu_plane_t* plane, gpu_t* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

void gpu_plane__render(gpu_plane_t* plane, gpu_t* gpu, gpu_window_t* window, gpu_frame_t* frame, gpu_material_t* material);

void gpu_plane__update_verts(gpu_plane_t* plane, gpu_t* gpu, kk_vec3_t verts[4]);

#endif /* GPU_PLANE_H */