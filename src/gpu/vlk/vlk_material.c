/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_log.h"
#include "gpu/gpu_material.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_material.h"
#include "gpu/vlk/vlk_prv.h"
#include "thirdparty/vma/vma.h"
#include "utl/utl_array.h"

#include "autogen/vlk_material.static.h"

/*=========================================================
MACROS
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

//## public
/**
Constructs a Vulkan material.

@param base The base object.
@param device The Vulkan device.
*/
void vlk_material__construct
	(
	gpu_material_t*					base,
	gpu_t*							gpu
	)
{
	_vlk_t* vlk = _vlk__from_base(gpu);

	/* Allocate memory used for the Vulkan implementation */
	base->data = malloc(sizeof(_vlk_material_t));
	if (!base->data)
	{
		kk_log__fatal("Failed to allocate memory for material.");
	}

	/* Initialize vulkan implementation */
	_vlk_material_t* material = _vlk_material__from_base(base);
	clear_struct(material);
	material->diffuse_texture = _vlk_texture__from_base(base->diffuse_texture);

	create_buffer(material, base, &vlk->dev);
}

//## public
/**
Destructs a material.
*/
void vlk_material__destruct(gpu_material_t* base, gpu_t* gpu)
{
	_vlk_material_t* material = _vlk_material__from_base(base);

	/* Cleanup */
	destroy_buffer(material);

	/* Free Vulkan implementation memory */
	free(base->data);
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## public
_vlk_material_t* _vlk_material__from_base(gpu_material_t* base)
{
	return (_vlk_material_t*)base->data;
}

//## static
static void create_buffer
	(
	_vlk_material_t*				material,
	gpu_material_t*					base,
	_vlk_dev_t*						device
	)
{
	VkDeviceSize buffer_size = sizeof(_vlk_material_ubo_t);
	
	_vlk_material_ubo_t ubo;
	clear_struct(&ubo);

	ubo.ambient.x = base->ambient_color.x;
	ubo.ambient.y = base->ambient_color.y;
	ubo.ambient.z = base->ambient_color.z;

	ubo.specular.x = base->specular_color.x;
	ubo.specular.y = base->specular_color.y;
	ubo.specular.z = base->specular_color.z;

	ubo.diffuse.x = base->diffuse_color.x;
	ubo.diffuse.y = base->diffuse_color.y;
	ubo.diffuse.z = base->diffuse_color.z;

	ubo.has_diffuse_texture = base->has_diffuse_texture;

	_vlk_buffer__construct(&material->buffer, device, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	_vlk_buffer__update(&material->buffer, (void*)&ubo, 0, buffer_size);
}

//## static
static void destroy_buffer(_vlk_material_t* material)
{
	_vlk_buffer__destruct(&material->buffer);
}