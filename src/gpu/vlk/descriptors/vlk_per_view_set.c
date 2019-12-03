/*=========================================================
INCLUDES
=========================================================*/

#include "engine/camera.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "platforms/common.h"
#include "thirdparty/vma/vma.h"
#include "thirdparty/cglm/include/cglm/cam.h"
#include "utl/utl_array.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Creates buffers for the set. */
static void create_buffers(_vlk_per_view_set_t* set);

/** Creates the descriptor sets. */
static void create_sets(_vlk_per_view_set_t* set);

/** Destroys buffers for the set. */
static void destroy_buffers(_vlk_per_view_set_t* set);

/** Destroys the descriptor sets. */
static void destroy_sets(_vlk_per_view_set_t* set);

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
_vlk_per_view_set__construct
*/
void _vlk_per_view_set__construct
	(
	_vlk_per_view_set_t*		set,
	_vlk_per_view_layout_t*		layout
	)
{
	clear_struct(set);
	set->layout = layout;

	create_buffers(set);
	create_sets(set);
}

/**
_vlk_per_view_set__destruct
*/
void _vlk_per_view_set__destruct(_vlk_per_view_set_t* set)
{
	destroy_sets(set);
	destroy_buffers(set);
}

/**
_vlk_per_view_set__bind
*/
void _vlk_per_view_set__bind
	(
	_vlk_per_view_set_t*			set,
	_vlk_frame_t*					frame,
	VkPipelineLayout				pipelineLayout
	)
{
	uint32_t setNum = 0; // TODO : hardcoded for now
	vkCmdBindDescriptorSets(frame->cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, setNum, 1, &set->sets[frame->image_idx], 0, NULL);
}

/**
_vlk_per_view_set__update
*/
void _vlk_per_view_set__update
	(
	_vlk_per_view_set_t*			set,
	_vlk_frame_t*					frame,
	camera_t*						camera,
	VkExtent2D						extent
	)
{
	_vlk_per_view_ubo_t ubo;
	clear_struct(&ubo);

	/* View matrix */
	vec3 look_at;
	glm_vec3_add(&camera->pos, &camera->dir, look_at);
	glm_lookat(&camera->pos, look_at, &camera->up, &ubo.view);

	/* Projection matrix */
	glm_perspective(glm_rad(45.0f), extent.width / (float)extent.height, 0.1f, 1000.0f, &ubo.proj);
	ubo.proj.y.y *= -1;

	/* Camera position */
	//camera__get_pos(camera, &ubo.camera_pos);
	glm_vec3_copy(&camera->pos, &ubo.camera_pos);
	
	/* Update the UBO */
	_vlk_buffer__update(&set->buffers[frame->image_idx], (void*)&ubo, 0, sizeof(ubo));
}

/*=========================================================
FUNCTIONS
=========================================================*/

/**
create_buffers
*/
static void create_buffers(_vlk_per_view_set_t* set)
{
	VkDeviceSize buffer_size = sizeof(_vlk_per_view_ubo_t);

	for (uint32_t i = 0; i < cnt_of_array(set->buffers); ++i)
	{
		_vlk_buffer__construct(&set->buffers[i], set->layout->dev, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	}
}

/**
create_sets
*/
void create_sets(_vlk_per_view_set_t* set)
{
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
		FATAL("Failed to allocate descriptor sets.");
	}

	for (uint32_t i = 0; i < NUM_FRAMES; i++)
	{
		VkDescriptorBufferInfo buffer_info = _vlk_buffer__get_buffer_info(&set->buffers[i]);

		VkWriteDescriptorSet descriptor_writes[1];
		memset(descriptor_writes, 0, sizeof(descriptor_writes));

		descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[0].dstSet = set->sets[i];
		descriptor_writes[0].dstBinding = 0;
		descriptor_writes[0].dstArrayElement = 0;
		descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptor_writes[0].descriptorCount = 1;
		descriptor_writes[0].pBufferInfo = &buffer_info;

		vkUpdateDescriptorSets(set->layout->dev->handle, cnt_of_array(descriptor_writes), descriptor_writes, 0, NULL);
	}
}

/**
destroy_buffers
*/
void destroy_buffers(_vlk_per_view_set_t* set)
{
	for (uint32_t i = 0; i < cnt_of_array(set->buffers); ++i)
	{
		_vlk_buffer__destruct(&set->buffers[i]);
	}
}

/**
destroy_sets
*/
void destroy_sets(_vlk_per_view_set_t* set)
{
	/* 
	Descriptor sets are destroyed automatically when the parent descriptor pool 
	is destroyed.
	*/
}
