#ifndef GPU_STATIC_MODEL_H
#define GPU_STATIC_MODEL_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "ecs/components/ecs_transform_.h"
#include "gpu/gpu_.h"
#include "gpu/gpu_material_.h"
#include "gpu/gpu_static_model_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "thirdparty/tinyobj/tinyobj.h"

/*=========================================================
TYPES
=========================================================*/

struct gpu_static_model_s
{
	void*				data;		/* Pointer to GPU-specific data. */
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Constructs a static model.

@param model The model to construct.
@param gpu The GPU context.
@param filename The model file to load.
*/
void gpu_static_model__construct(gpu_static_model_t* model, gpu_t* gpu, const char* filename);

/**
Destructs a static model.

@param model The model to destruct.
@param gpu The GPU context.
*/
void gpu_static_model__destruct(gpu_static_model_t* model, gpu_t* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Renders a static model.

@param model The model to render.
@param gpu The GPU context.
@param material The material to use for the model.
@param transform The transformation to apply to the model.
*/
void gpu_static_model__render(gpu_static_model_t* model, gpu_t* gpu, gpu_material_t* material, ecs_transform_t* transform);

#endif /* GPU_STATIC_MODEL_H */