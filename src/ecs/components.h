#ifndef COMPONENTS_H
#define COMPONENTS_H

/*=========================================================
INCLUDES
=========================================================*/

#include "gpu/gpu_model.h"
#include "platforms/platform.h"
#include "platforms/common.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct
{
	boolean 		is_used;
	char			file_name[ MAX_FILENAME_CHARS ];

	// TODO : make pointer and malloc?
	gpu_model_t		model;

} static_model_comp;

typedef struct
{
	boolean 		is_used;
	vec3_t			pos;

} transform_comp;

#endif /* COMPONENTS_H */