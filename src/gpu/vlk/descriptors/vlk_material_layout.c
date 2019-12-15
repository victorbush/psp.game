/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "thirdparty/vma/vma.h"
#include "utl/utl_array.h"
#include "utl/utl_log.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Creates a descriptor pool for this layout. Descriptor sets are allocated from the pool. */
static void create_descriptor_pool(_vlk_descriptor_layout_t* layout);

/** Creates the descriptor set layout. */
static void create_layout(_vlk_descriptor_layout_t* layout);

/** Destroys the descriptor pool. */
static void destroy_descriptor_pool(_vlk_descriptor_layout_t* layout);

/** Destroys the descriptor set layout. */
static void destroy_layout(_vlk_descriptor_layout_t* layout);

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
_vlk_material_layout__construct
*/
void _vlk_material_layout__construct
	(
	_vlk_descriptor_layout_t*	layout,
	_vlk_dev_t*					device
	)
{
	clear_struct(layout);
	layout->dev = device;

	create_descriptor_pool(layout);
	create_layout(layout);
}

/**
_vlk_material_layout__destruct
*/
void _vlk_material_layout__destruct(_vlk_descriptor_layout_t* layout)
{
	destroy_layout(layout);
	destroy_descriptor_pool(layout);
}

/*=========================================================
FUNCTIONS
=========================================================*/

/**
create_layout
*/
static void create_layout(_vlk_descriptor_layout_t* layout)
{
	/* Material UBO */
	VkDescriptorSetLayoutBinding ubo_layout_binding;
	clear_struct(&ubo_layout_binding);
	ubo_layout_binding.binding = 0;
	ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubo_layout_binding.descriptorCount = 1;
	ubo_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	ubo_layout_binding.pImmutableSamplers = NULL;

	/* Diffuse texture */
	VkDescriptorSetLayoutBinding diffuse_texture_binding;
	clear_struct(&diffuse_texture_binding);
	diffuse_texture_binding.binding = 1;
	diffuse_texture_binding.descriptorCount = 1;
	diffuse_texture_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	diffuse_texture_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	diffuse_texture_binding.pImmutableSamplers = NULL;

	VkDescriptorSetLayoutBinding bindings[2];
	memset(bindings, 0, sizeof(bindings));
	bindings[0] = ubo_layout_binding;
	bindings[1] = diffuse_texture_binding;

	VkDescriptorSetLayoutCreateInfo layout_info;
	clear_struct(&layout_info);
	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_info.bindingCount = cnt_of_array(bindings);
	layout_info.pBindings = bindings;

	if (vkCreateDescriptorSetLayout(layout->dev->handle, &layout_info, NULL, &layout->handle) != VK_SUCCESS)
	{
		FATAL("Failed to create descriptor set layout.");
	}
}

/**
Creates a descriptor pool for this layout. Descriptor sets are allocated
from the pool.
*/
static void create_descriptor_pool(_vlk_descriptor_layout_t* layout)
{
	VkDescriptorPoolSize pool_sizes[2];
	memset(pool_sizes, 0, sizeof(pool_sizes));
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[0].descriptorCount = MAX_NUM_MATERIALS;
	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[1].descriptorCount = MAX_NUM_MATERIALS;

	VkDescriptorPoolCreateInfo pool_info;
	clear_struct(&pool_info);
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = cnt_of_array(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	// TOOD : what should maxSets be??
	pool_info.maxSets = MAX_NUM_MATERIALS;

	if (vkCreateDescriptorPool(layout->dev->handle, &pool_info, NULL, &layout->pool_handle) != VK_SUCCESS) 
	{
		FATAL("Failed to create descriptor pool.");
	}
}

/**
destroy_descriptor_pool
*/
static void destroy_descriptor_pool(_vlk_descriptor_layout_t* layout)
{
	vkDestroyDescriptorPool(layout->dev->handle, layout->pool_handle, NULL);
}

/**
destroy_layout
*/
static void destroy_layout(_vlk_descriptor_layout_t* layout)
{
	vkDestroyDescriptorSetLayout(layout->dev->handle, layout->handle, NULL);
}
