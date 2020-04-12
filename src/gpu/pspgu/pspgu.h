#ifndef PSPGU_H
#define PSPGU_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "gpu/pspgu/pspgu_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "gpu/gpu.h"
#include "utl/utl_array.h"

/*=========================================================
TYPES
=========================================================*/

utl_array_declare_type(_pspgu_static_mesh_t);

struct _pspgu_vertex_s
{
	float u, v;
	unsigned int color;
	float x, y, z;
};

struct _pspgu_static_mesh_s
{
	/*
	Create/destroy
	*/
	_pspgu_vertex_t* vertex_array;

	/*
	Other
	*/
	int num_verts;
};

struct _pspgu_static_model_s
{
	/*
	Create/destroy
	*/
	utl_array_t(_pspgu_static_mesh_t)	meshes;		/* List of meshes the comprise the model. */
};

struct _pspgu_plane_s
{
	_pspgu_vertex_t 	verts[4];
};

struct _pspgu_texture_s
{
	/*
	Create/destroy
	*/
	void*		data;	/* Pointer to the texture data. */

	/*
	Other
	*/
	uint32_t	height;
	uint32_t	size_in_bytes;
	uint32_t	width;

};

typedef struct
{
	/*
	Dependencies
	*/
	kk_vec3_t			diffuse_color;
	_pspgu_texture_t*	diffuse_texture;

} _pspgu_material_t;

/**
PSP GPU context data.
*/
typedef struct
{
	void*		frame_buffer_0;
	void*		frame_buffer_1;
	void*		z_buffer;
	uint32_t 	vram_stack_ptr;	/* using a simple stack for vram right now */

	/* display list (16 byte aligned) */
	uint32_t 	__attribute__((aligned(16))) display_list[262144];

} _pspgu_t;

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/pspgu.public.h"
#include "autogen/pspgu.internal.h"
#include "autogen/pspgu_material.internal.h"
#include "autogen/pspgu_plane.internal.h"
#include "autogen/pspgu_static_mesh.internal.h"
#include "autogen/pspgu_static_model.internal.h"
#include "autogen/pspgu_texture.internal.h"

#endif /* PSPGU_H */