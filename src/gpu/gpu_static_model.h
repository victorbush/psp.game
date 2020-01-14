#ifndef GPU_STATIC_MODEL_H
#define GPU_STATIC_MODEL_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "ecs/components/ecs_transform_.h"
#include "gpu/gpu_.h"
#include "gpu/gpu_frame_.h"
#include "gpu/gpu_material_.h"
#include "gpu/gpu_static_model_.h"
#include "gpu/gpu_window_.h"

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
FUNCTIONS
=========================================================*/

#include "autogen/gpu_static_model.public.h"

#endif /* GPU_STATIC_MODEL_H */