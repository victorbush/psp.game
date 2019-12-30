#ifndef GPU_STATIC_MODEL_H
#define GPU_STATIC_MODEL_H

/*=========================================================
DECLARATIONS
=========================================================*/

typedef struct gpu_static_model_s gpu_static_model_t;

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/components.h"
#include "gpu.h"
#include "gpu/gpu_material.h"
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
void gpu_static_model__render(gpu_static_model_t* model, gpu_t* gpu, gpu_material_t* material, transform_comp_t* transform);

#endif /* GPU_STATIC_MODEL_H */