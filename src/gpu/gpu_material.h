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

typedef struct 
{
	void*				data;		/* Pointer to GPU-specific data. */

	/** Filename of the diffuse texture. */
	char				diffuse_texture[MAX_FILENAME_CHARS];

	vec3_t				ambient_color;
	vec3_t				diffuse_color;
	vec3_t				specular_color;

} gpu_material_t;

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* GPU_MATERIAL_H */