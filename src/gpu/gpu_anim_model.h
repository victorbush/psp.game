#ifndef GPU_ANIM_MODEL_H
#define GPU_ANIM_MODEL_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "gpu/gpu_.h"
#include "gpu/gpu_anim_model_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "thirdparty/md5/md5model.h"

/*=========================================================
TYPES
=========================================================*/

struct gpu_anim_model_s 
{
	void*				data;		/* Pointer to GPU-specific data. */
	md5_model_t			md5;		/* MD5 model data. */
};

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Constructs an animated model.
*/
void gpu_anim_model__construct(gpu_anim_model_t* model, gpu_t* gpu, const char* filename);

/**
Destructs an animated model.
*/
void gpu_anim_model__destruct(gpu_anim_model_t* model, gpu_t* gpu);

#endif /* GPU_ANIM_MODEL_H */