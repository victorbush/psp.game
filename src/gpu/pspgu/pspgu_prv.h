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

/*=========================================================
TYPES
=========================================================*/

typedef struct
{
	float u, v;
	unsigned int color;
	float x, y, z;

} _pspgu_vertex_t;

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
psp_gpu.c
-------------------------------------*/

/**
Gets the PSP GPU context implementation memory from the GPU context.

@param The GPU context.
@returns The PSP GPU context.
*/
_pspgu_t* _pspgu__get_context(gpu_t* gpu);

#endif /* PSPGU_PRV_H */