#ifndef GPU_MATERIAL_H
#define GPU_MATERIAL_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "gpu/gpu_.h"
#include "gpu/gpu_material_.h"
#include "gpu/gpu_texture_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_math.h"

/*=========================================================
TYPES
=========================================================*/

struct gpu_material_s
{
	void*				data;		/* Pointer to GPU-specific data. */

	/** Filename of the diffuse texture. */
	char				diffuse_texture_name[MAX_FILENAME_CHARS];
	gpu_texture_t*		diffuse_texture;

	kk_vec3_t			ambient_color;
	kk_vec3_t			diffuse_color;
	kk_vec3_t			specular_color;
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Constructs a material using a material script file.

@param material The material to construct.
@param gpu The GPU context.
@param filename The file that defines the material.
*/
void gpu_material__construct(gpu_material_t* material, gpu_t* gpu, const char* filename);

/**
Destructs a material.

@param material The material to destruct.
@param gpu The GPU context.
*/
void gpu_material__destruct(gpu_material_t* material, gpu_t* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* GPU_MATERIAL_H */