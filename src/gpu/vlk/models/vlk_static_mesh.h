#ifndef VLK_STATIC_MESH_H
#define VLK_STATIC_MESH_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "gpu/vlk/models/vlk_static_mesh_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

struct _vlk_static_mesh_s
{
	/*
	Create/destroy
	*/
	_vlk_buffer_t			vertex_buffer;
	_vlk_buffer_t			index_buffer;

	/*
	Other
	*/
	uint32_t				num_indices;
};

/**
Defines vertex attributes as used by the pipeline and shaders.
*/
struct _vlk_static_mesh_vertex_s
{
	kk_vec3_t				pos;
	kk_vec3_t				normal;
	kk_vec2_t				tex;
	int						material_idx;
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/vlk_static_mesh.public.h"

#endif /* VLK_STATIC_MESH_H */