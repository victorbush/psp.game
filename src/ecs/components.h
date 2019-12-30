#ifndef COMPONENTS_H
#define COMPONENTS_H

/*=========================================================
DECLARATIONS
=========================================================*/

typedef struct comp_s comp_t;
typedef struct static_model_comp_s static_model_comp_t;
typedef struct transform_comp_s transform_comp_t;

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/gpu_material.h"
#include "gpu/gpu_plane.h"
#include "gpu/gpu_static_model.h"
#include "utl/utl_math.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/*-------------------------------------
Component instance data
-------------------------------------*/

/**
Base struct for all components. Must be the chunk of data in each component.
*/
struct comp_s
{
	boolean 		is_used;
};

/**
A static model without any animation.
*/
struct static_model_comp_s
{
	comp_t						base;
	char						file_name[MAX_FILENAME_CHARS];
	gpu_static_model_t*			model;
	gpu_material_t*				material;
};

/**
A coordinate transformation that can be applied to an entity.
*/
struct transform_comp_s
{
	comp_t			base;
	vec3_t			pos;
};

#endif /* COMPONENTS_H */