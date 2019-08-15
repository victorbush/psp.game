/*=========================================================
INCLUDES
=========================================================*/

#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_private.h"
#include "platforms/common.h"
#include "utl/utl_array.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

/**
_vlk_device__init
*/
void _vlk_device__init
	(
	_vlk_t*							vlk,				/* context */
	_vlk_dev_t*						dev,				/* the logical device to initialize */
	_vlk_gpu_t*						gpu,				/* physical device used by the logical device */
	utl_array_t(string)*			req_dev_ext,		/* required device extensions */
	utl_array_t(string)*			req_inst_layers		/* required instance layers */
	)
{
	clear_struct(dev);
	dev->gpu = gpu;
	dev->gfx_family_idx = -1;
	dev->present_family_idx = -1;
}

void _vlk_device__term
	(
	_vlk_dev_t*					dev
	)
{
	utl_array_destroy(&dev->used_queue_families);
}