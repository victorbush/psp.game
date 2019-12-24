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

void gpu_plane__construct(gpu_plane_t* plane, gpu_t* gpu, float width, float height)
{
	clear_struct(plane);
	plane->width = width;
	plane->height = height;

	gpu->intf->plane__construct(plane, gpu);
}

void gpu_plane__destruct(gpu_plane_t* plane, gpu_t* gpu)
{
	gpu->intf->plane__destruct(plane, gpu);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void gpu_plane__render(gpu_plane_t* plane, gpu_t* gpu, gpu_material_t* material, transform_comp_t* transform)
{
	gpu->intf->plane__render(plane, gpu, material, transform);
}