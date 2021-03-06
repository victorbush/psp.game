#ifndef VLK_STATIC_MODEL_H
#define VLK_STATIC_MODEL_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "gpu/gpu_static_model.h"
#include "gpu/vlk/models/vlk_static_model_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "gpu/vlk/vlk_material.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

struct _vlk_static_model_s
{
	/*
	Dependencies
	*/
	_vlk_t*								vlk;
	gpu_static_model_t*					base;

	/*
	Create/destroy
	*/
	_vlk_material_set_t					material_set;	/* The material desriptor set for this model. */
	utl_array_t(_vlk_static_mesh_t)		meshes;			/* List of meshes the comprise the model. */
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/vlk_static_model.public.h"

#endif /* VLK_STATIC_MODEL_H */