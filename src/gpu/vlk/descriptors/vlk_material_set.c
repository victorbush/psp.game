/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_camera.h"
#include "engine/kk_log.h"
#include "gpu/gpu.h"
#include "gpu/gpu_material.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_material.h"
#include "gpu/vlk/vlk_prv.h"
#include "thirdparty/vma/vma.h"
#include "utl/utl_array.h"

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Creates the descriptor sets. */
static void create_sets
	(
	_vlk_material_set_t*		set,
	_vlk_t*						vlk,
	gpu_material_t*				material_array,
	uint32_t					material_cnt
	);

/** Destroys the descriptor sets. */
static void destroy_sets(_vlk_material_set_t* set);

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
_vlk_material_set__construct
*/
void _vlk_material_set__construct
	(
	_vlk_material_set_t*		set,
	_vlk_t*						vlk,
	_vlk_descriptor_layout_t*	layout,
	gpu_material_t*				material_array,
	uint32_t					material_cnt
	)
{
	clear_struct(set);
	set->layout = layout;

	create_sets(set, vlk, material_array, material_cnt);
}

/**
_vlk_material_set__destruct
*/
void _vlk_material_set__destruct(_vlk_material_set_t* set)
{
	destroy_sets(set);
}

/**
_vlk_material_set__bind
*/
void _vlk_material_set__bind
	(
	_vlk_material_set_t*			set,
	_vlk_frame_t*					frame,
	VkPipelineLayout				pipelineLayout
	)
{
	uint32_t firstSetNum = 1; // TODO ??
	vkCmdBindDescriptorSets(frame->cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, firstSetNum, 1, &set->sets[frame->image_idx], 0, NULL);
}

/*=========================================================
FUNCTIONS
=========================================================*/

/**
create_sets
*/
static void create_sets
	(
	_vlk_material_set_t*		set,
	_vlk_t*						vlk,
	gpu_material_t*				material_array,
	uint32_t					material_cnt
	)
{
	if (material_cnt > MAX_NUM_MATERIALS_PER_SET)
	{
		kk_log__fatal("Number of materials exceeds max allowed per set.");
	}

	/* 
	Create a descriptor set for each possible concurrent frame 
	*/
	VkDescriptorSetLayout layouts[NUM_FRAMES];
	memset(layouts, 0, sizeof(layouts));
	layouts[0] = set->layout->handle;
	layouts[1] = set->layout->handle;

	VkDescriptorSetAllocateInfo alloc_info;
	clear_struct(&alloc_info);
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = set->layout->pool_handle;
	alloc_info.descriptorSetCount = NUM_FRAMES;
	alloc_info.pSetLayouts = layouts;

	VkResult result = vkAllocateDescriptorSets(set->layout->dev->handle, &alloc_info, &set->sets[0]);
	if (result != VK_SUCCESS)
	{
		kk_log__fatal("Failed to allocate descriptor sets.");
	}

	/*
	Update each descriptor set. There is a set for each concurrent frame. For example, if double
	buffered there would be two sets.
	*/
	for (uint32_t i = 0; i < NUM_FRAMES; i++)
	{
		/*
		There are multiple materials that can be stored within a material descriptor set.
		If a material slot is unused, assign it to a default material (same for textures).

		In the shader, the UBO and texture samplers are defined as arrays. The
		dstArrayElement field defines the index in the array to update.

		The Vulkan shaders require each slot to be defined, even if not used. For example,
		a texture sampler needs a texture even if not used. So we use a simple default
		texture for such cases.
		*/
		for (uint32_t mat_idx = 0; mat_idx < MAX_NUM_MATERIALS_PER_SET; ++mat_idx)
		{
			_vlk_material_t* mat = NULL;

			if (mat_idx >= material_cnt)
			{
				/* Use default material for this slot */
				mat = _vlk_material__from_base(gpu__get_default_material(vlk->base));
			}
			else
			{
				mat = _vlk_material__from_base(&material_array[mat_idx]);
			}

			VkDescriptorBufferInfo buffer_info = _vlk_buffer__get_buffer_info(&mat->buffer);
			VkWriteDescriptorSet writes[2];
			memset(writes, 0, sizeof(writes));

			/* Material UBO */
			writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writes[0].dstSet = set->sets[i];
			writes[0].dstBinding = 0;
			writes[0].dstArrayElement = mat_idx;
			writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writes[0].descriptorCount = 1;
			writes[0].pBufferInfo = &buffer_info; /* remember we are pointing to a struct on the stack here, don't overwrite before doing the descriptor write */

			/* Diffuse texture */
			writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writes[1].dstSet = set->sets[i];
			writes[1].dstBinding = 1;
			writes[1].dstArrayElement = mat_idx;
			writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writes[1].descriptorCount = 1;
			writes[1].pImageInfo = &mat->diffuse_texture->image_info;

			vkUpdateDescriptorSets(set->layout->dev->handle, cnt_of_array(writes), writes, 0, NULL);
		}
	}
}

/**
destroy_sets
*/
void destroy_sets(_vlk_material_set_t* set)
{
	/* 
	Descriptor sets are destroyed automatically when the parent descriptor pool 
	is destroyed.
	*/
}
