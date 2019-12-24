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

/*-------------------------------------
pspgu.c
-------------------------------------*/

/**
Gets the PSP GPU context implementation memory from the GPU context.

@param The GPU context.
@returns The PSP GPU context.
*/
_pspgu_t* _pspgu__get_context(gpu_t* gpu);

/*-------------------------------------
pspgu_static_mesh.c
-------------------------------------*/

void _pspgu_static_mesh__construct
	(
	_pspgu_static_mesh_t*		mesh,
	_pspgu_t*					ctx,
	const tinyobj_t*			obj,
	const tinyobj_shape_t*		obj_shape
	);

void _pspgu_static_mesh__destruct(_pspgu_static_mesh_t* mesh);

void _pspgu_static_mesh__render
	(
	_pspgu_static_mesh_t*		mesh,
	_pspgu_t*					ctx
	);

/*-------------------------------------
pspgu_static_model.c
-------------------------------------*/

void _pspgu_static_model__construct
	(
	_pspgu_static_model_t*		model,
	_pspgu_t*					ctx,
	const tinyobj_t*			obj
	);

void _pspgu_static_model__destruct(_pspgu_static_model_t* model);

void _pspgu_static_model__render
	(
	_pspgu_static_model_t*		model,
	_pspgu_t*					ctx
	);

#endif /* PSPGU_PRV_H */