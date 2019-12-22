#ifndef GPU_MATERIAL_H
#define GPU_MATERIAL_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "utl/utl_math.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct gpu_s gpu_t;
typedef struct gpu_material_s gpu_material_t;
typedef struct gpu_texture_s gpu_texture_t;

struct gpu_material_s
{
	void*				data;		/* Pointer to GPU-specific data. */

	/** Filename of the diffuse texture. */
	char				diffuse_texture_name[MAX_FILENAME_CHARS];
	gpu_texture_t*		diffuse_texture;

	vec3_t				ambient_color;
	vec3_t				diffuse_color;
	vec3_t				specular_color;
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