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
DECLARATIONS
=========================================================*/

/**
Creates the command pool.
*/
static void create_command_pool(_vlk_dev_t* dev);

/**
Creates the logical device.
*/
static void create_logical_device
	(
	_vlk_dev_t*						dev,				/* the logical device to initialize */
	utl_array_t(string)*			req_dev_ext,		/* required device extensions */
	utl_array_t(string)*			req_inst_layers		/* required instance layers */
	);

/**
Creates a texture sampler.
*/
static void create_texture_sampler(_vlk_dev_t* dev);

/**
Destroys the command pool.
*/
static void destroy_command_pool(_vlk_dev_t* dev);

/**
Destroys the logical device.
*/
static void destroy_logical_device(_vlk_dev_t* dev);

/**
Destroys a texture sampler.
*/
static void destroy_texture_sampler(_vlk_dev_t* dev);

/**
Checks if a format has a stencil component.
*/
static boolean has_stencil_component(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

/*=========================================================
CONSTRUCTORS
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

	create_logical_device(dev, req_dev_ext, req_inst_layers);
	create_command_pool(dev);
	create_texture_sampler(dev);
}

/**
_vlk_device__term
*/
void _vlk_device__term
	(
	_vlk_dev_t*						dev
	)
{
	destroy_texture_sampler(dev);
	destroy_command_pool(dev);
	destroy_logical_device(dev);

	utl_array_destroy(&dev->used_queue_families);
}

/*=========================================================
FUNCTIONS
=========================================================*/

/**
_vlk_device__begin_one_time_cmd_buf
*/
VkCommandBuffer _vlk_device__begin_one_time_cmd_buf(_vlk_dev_t* dev)
{
	VkCommandBufferAllocateInfo alloc_info;
	clear_struct(&alloc_info);
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = dev->command_pool;
	alloc_info.commandBufferCount = 1;

	VkCommandBuffer cmd_buf;
	clear_struct(&cmd_buf);
	vkAllocateCommandBuffers(dev->handle, &alloc_info, &cmd_buf);

	VkCommandBufferBeginInfo begin_info;
	clear_struct(&begin_info);
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmd_buf, &begin_info);

	return cmd_buf;
}

/**
_vlk_device__end_one_time_cmd_buf
*/
void _vlk_device__end_one_time_cmd_buf(_vlk_dev_t* dev, VkCommandBuffer cmd_buf)
{
	vkEndCommandBuffer(cmd_buf);

	VkSubmitInfo submit_info;
	clear_struct(&submit_info);
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &cmd_buf;

	vkQueueSubmit(dev->gfx_queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(dev->gfx_queue);

	vkFreeCommandBuffers(dev->handle, dev->command_pool, 1, &cmd_buf);
}

/**
_vlk_device__transition_image_layout
*/
void _vlk_device__transition_image_layout
	(
	_vlk_dev_t*				dev, 
	VkImage					image, 
	VkFormat				format, 
	VkImageLayout			old_layout,
	VkImageLayout			new_layout
	)
{
	VkCommandBuffer cmd_buf = _vlk_device__begin_one_time_cmd_buf(dev);

	VkImageMemoryBarrier barrier;
	clear_struct(&barrier);
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = old_layout;
	barrier.newLayout = new_layout;

	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image = image;

	if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (has_stencil_component(format)) 
		{
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else 
	{
		FATAL("Usupported layout transition.");
	}

	vkCmdPipelineBarrier(
		cmd_buf,
		sourceStage, destinationStage,
		0,
		0, NULL,
		0, NULL,
		1, &barrier
	);

	_vlk_device__end_one_time_cmd_buf(dev, cmd_buf);
}

/**
create_command_pool
*/
static void create_command_pool
	(
	_vlk_dev_t*						dev
	)
{
	VkCommandPoolCreateInfo pool_info;
	clear_struct(&pool_info);
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = dev->gfx_family_idx;
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // 0; // Optional

	if (vkCreateCommandPool(dev->handle, &pool_info, NULL, &dev->command_pool) != VK_SUCCESS)
	{
		FATAL("Failed to create command pool.");
	}
}

/**
create_logical_device
*/
static void create_logical_device
	(
	_vlk_dev_t*						dev,				/* the logical device to initialize */
	utl_array_t(string)*			req_dev_ext,		/* required device extensions */
	utl_array_t(string)*			req_inst_layers		/* required instance layers */
	)
{
	uint32_t i;
	_vlk_gpu_t *gpu = dev->gpu;

	/*
	Specify what queues to create
	*/
	utl_array_create(VkDeviceQueueCreateInfo, queues);

	/* Check for graphics family */
	if (gpu->queue_family_indices.graphics_families.count == 0)
	{
		FATAL("No graphics family queue found.");
	}

	dev->gfx_family_idx = gpu->queue_family_indices.graphics_families.data[0];

	/* Check for present family */
	if (gpu->queue_family_indices.present_families.count == 0)
	{
		FATAL("No present family queue found.");
	}

	dev->present_family_idx = gpu->queue_family_indices.present_families.data[0];

	/* Create list of used queue families */
	utl_array_push(&dev->used_queue_families, dev->gfx_family_idx);
	utl_array_push(&dev->used_queue_families, dev->present_family_idx);

	float queuePriority = 1.0f;

	/* create multiple queues if needed based on QF properties */
	for (i = 0; i < dev->used_queue_families.count; ++i)
	{
		VkDeviceQueueCreateInfo queue_info;
		clear_struct(&queue_info);
		queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info.queueFamilyIndex = dev->used_queue_families.data[i];
		queue_info.queueCount = 1;
		queue_info.pQueuePriorities = &queuePriority;
		utl_array_push(&queues, queue_info);
	}

	/*
	* Specify device features to use
	*/
	VkPhysicalDeviceFeatures device_features;
	clear_struct(&device_features);
	device_features.samplerAnisotropy = gpu->supported_features.samplerAnisotropy;

	/*
	* Create the logical device
	*/
	VkDeviceCreateInfo create_info;
	clear_struct(&create_info);
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	/* queues */
	create_info.queueCreateInfoCount = queues.count;
	create_info.pQueueCreateInfos = queues.data;

	/* device features */
	create_info.pEnabledFeatures = &device_features;

	/* extensions */
	create_info.enabledExtensionCount = req_dev_ext->count;
	create_info.ppEnabledExtensionNames = req_dev_ext->data;

	/* layers */
	create_info.enabledLayerCount = req_inst_layers->count;
	create_info.ppEnabledLayerNames = req_inst_layers->data;

	/* create the logical device */
	if (vkCreateDevice(gpu->handle, &create_info, NULL, &dev->handle) != VK_SUCCESS)
	{
		FATAL("Failed to create logical device.");
	}

	/*
	* Get queue handles
	*/
	vkGetDeviceQueue(dev->handle, dev->gfx_family_idx, 0, &dev->gfx_queue);
	vkGetDeviceQueue(dev->handle, dev->present_family_idx, 0, &dev->present_queue);

	/*
	Cleanup
	*/
	utl_array_destroy(&queues);
}

/**
create_texture_sampler
*/
static void create_texture_sampler
	(
	_vlk_dev_t*						dev
	)
{
	VkSamplerCreateInfo sampler_info;
	clear_struct(&sampler_info);
	sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_info.magFilter = VK_FILTER_LINEAR;
	sampler_info.minFilter = VK_FILTER_LINEAR;

	sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	if (dev->gpu->supported_features.samplerAnisotropy)
	{
		sampler_info.anisotropyEnable = VK_TRUE;
		sampler_info.maxAnisotropy = 16.0f;
	}
	else
	{
		sampler_info.anisotropyEnable = VK_FALSE;
		sampler_info.maxAnisotropy = 1.0f;
	}

	sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_info.unnormalizedCoordinates = VK_FALSE;
	sampler_info.compareEnable = VK_FALSE;
	sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;

	sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_info.mipLodBias = 0.0f;
	sampler_info.minLod = 0.0f;
	sampler_info.maxLod = 0.0f;

	if (vkCreateSampler(dev->handle, &sampler_info, NULL, &dev->texture_sampler) != VK_SUCCESS) 
	{
		FATAL("Failed to create texture sampler.");
	}
}

/**
destroy_command_pool
*/
static void destroy_command_pool
	(
	_vlk_dev_t*						dev
	)
{
	vkDestroyCommandPool(dev->handle, dev->command_pool, NULL);
}

/**
destroy_logical_device
*/
static void destroy_logical_device
	(
	_vlk_dev_t*						dev
	)
{
	vkDestroyDevice(dev->handle, NULL);
}

/**
Destroys a texture sampler.
*/
static void destroy_texture_sampler
	(
	_vlk_dev_t*						dev
	)
{
	vkDestroySampler(dev->handle, dev->texture_sampler, NULL);
}