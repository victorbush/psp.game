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

/**
Base struct for all components. Must be the chunk of data in each component.
*/
typedef struct
{
	boolean 		is_used;

} comp_t;

/**
A static model without any animation.
*/
typedef struct
{
	comp_t			base;
	char			file_name[ MAX_FILENAME_CHARS ];

	// TODO : make pointer and malloc?
	gpu_model_t		model;

} static_model_comp_t;

/**
A coordinate transformation that can be applied to an entity.
*/
typedef struct
{
	comp_t			base;
	vec3_t			pos;

} transform_comp_t;

#endif /* COMPONENTS_H */