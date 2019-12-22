#ifndef COMPONENTS_H
#define COMPONENTS_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "utl/utl_math.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct gpu_material_s gpu_material_t;
typedef struct gpu_static_model_s gpu_static_model_t;

/**
Base struct for all components. Must be the chunk of data in each component.
*/
typedef struct comp_s comp_t;
struct comp_s
{
	boolean 		is_used;
};

/**
A static model without any animation.
*/
typedef struct static_model_comp_s static_model_comp_t;
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
typedef struct transform_comp_s transform_comp_t;
struct transform_comp_s
{
	comp_t			base;
	vec3_t			pos;
};

#endif /* COMPONENTS_H */