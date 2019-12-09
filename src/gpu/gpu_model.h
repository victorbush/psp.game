#ifndef GPU_MODEL_H
#define GPU_MODEL_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "thirdparty/md5/md5model.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct 
{
	void*			gpu_data;	/* Pointer to GPU-specific data. */
	md5_model_t		md5;		/* MD5 model data. */

} gpu_model_t;

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* GPU_MODEL_H */