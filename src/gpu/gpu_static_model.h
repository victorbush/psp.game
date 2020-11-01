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
#include "utl/utl_array.h"
#include "thirdparty/tinyobj/tinyobj.h"

/*=========================================================
TYPES
=========================================================*/

utl_array_declare_type(gpu_material_t);

struct gpu_static_model_s
{
	void*							data;		/* Pointer to GPU-specific data. */
	utl_array_t(gpu_material_t)		materials;
};

/*=========================================================
FUNCTIONS
=========================================================*/

#include "autogen/gpu_static_model.public.h"

#endif /* GPU_STATIC_MODEL_H */