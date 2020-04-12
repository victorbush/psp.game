#ifndef PSPGU_PRV_H
#define PSPGU_PRV_H

/*=========================================================
INCLUDES
=========================================================*/

/* #include <pspdisplay.h>
#include <pspge.h>
#include <pspgu.h>
#include <pspgum.h> */
#include <pspkernel.h>

#include "common.h"
#include "engine/kk_math.h"
#include "gpu/gpu.h"
#include "thirdparty/tinyobj/tinyobj.h"
#include "utl/utl_array.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct _pspgu_static_mesh_s _pspgu_static_mesh_t;

utl_array_declare_type(_pspgu_static_mesh_t);

typedef struct
{
	float u, v;
	unsigned int color;
	float x, y, z;

} _pspgu_vertex_t;

struct _pspgu_static_mesh_s
{
	/*
	Create/destroy
	*/
	_pspgu_vertex_t*	vertex_array;

	/*
	Other
	*/
	int num_verts;
};

typedef struct
{
	/*
	Create/destroy
	*/
	utl_array_t(_pspgu_static_mesh_t)	meshes;		/* List of meshes the comprise the model. */

} _pspgu_static_model_t;

typedef struct
{
	_pspgu_vertex_t 	verts[4];

} _pspgu_plane_t;

typedef struct
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

} _pspgu_texture_t;

typedef struct
{
	/*
	Dependencies
	*/
	kk_vec3_t				diffuse_color;
	_pspgu_texture_t*		diffuse_texture;

} _pspgu_material_t;

/**
PSP GPU context data.
*/
typedef struct
{
	void* 		frame_buffer_0;	
	void*		frame_buffer_1;
	void*		z_buffer;
	uint32_t 	vram_stack_ptr;	/* using a simple stack for vram right now */

	/* display list (16 byte aligned) */
 	uint32_t 	__attribute__((aligned(16))) display_list[262144];

} _pspgu_t; 

/*=========================================================
CONSTRUCTORS
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/pspgu.internal.h"

#endif /* PSPGU_PRV_H */