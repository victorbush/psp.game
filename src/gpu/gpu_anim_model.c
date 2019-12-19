/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "gpu/gpu.h"
#include "gpu/gpu_anim_model.h"
#include "thirdparty/md5/md5model.h"
#include "utl/utl_log.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void gpu_anim_model__construct(gpu_anim_model_t* model, gpu_t* gpu, const char* filename)
{
	clear_struct(model);

	/* Load the model file */
	// TODO : Animation
	ReadMD5Model(filename, &model->md5);

	/* Construct */
	gpu->intf->anim_model__construct(model, gpu);
}

void gpu_anim_model__destruct(gpu_anim_model_t* model, gpu_t* gpu)
{
	/* Destruct */
	gpu->intf->anim_model__destruct(model, gpu);

	/* Free MD5 data */
	FreeModel(&model->md5);
}

/*=========================================================
FUNCTIONS
=========================================================*/
