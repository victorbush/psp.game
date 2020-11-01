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
#include "thirdparty/tinyobj/tinyobj.h"

/*=========================================================
TYPES
=========================================================*/

struct gpu_material_s
{
	void*				data;		/* Pointer to GPU-specific data. */

	/** Filename of the diffuse texture. */
	char				diffuse_texture_name[MAX_FILENAME_CHARS];
	gpu_texture_t*		diffuse_texture;
	boolean				has_diffuse_texture;

	kk_vec3_t			ambient_color;
	kk_vec3_t			diffuse_color;
	kk_vec3_t			specular_color;
};

struct gpu_material_create_info_s
{
	kk_vec3_t			ambient_color;
	kk_vec3_t			diffuse_color;
	gpu_texture_t*		diffuse_texture;	/* Set to NULL if no texture */
	kk_vec3_t			specular_color;
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/gpu_material.public.h"

#endif /* GPU_MATERIAL_H */