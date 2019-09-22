/*=========================================================
INCLUDES
=========================================================*/

#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_private.h"
#include "gpu/vlk/memory/vlk_memory.h"
#include "platforms/common.h"
#include "utl/utl_array.h"

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

void _vlk_allocation__alloc_buffer()
{
	/* */
	vkBindBufferMemory()

	
}

static void find_allocation()
{

}

static void create_pools(_vlk_gpu_t* gpu)
{
	uint32_t num_types;
	
	num_types = gpu->mem_properties.memoryTypeCount;

}