/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "ecs/components.h"
#include "gpu/gpu.h"
#include "gpu/gpu_plane.h"
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

/*=========================================================
FUNCTIONS
=========================================================*/

void gpu_plane__render(gpu_plane_t* plane, gpu_t* gpu, transform_comp_t* transform)
{
	gpu->intf->plane__render(plane, gpu, transform);
}