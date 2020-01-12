/*=========================================================
INCLUDES
=========================================================*/

#include <stdio.h>

#include "common.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "log/log.h"
#include "utl/utl_array.h"

#include "autogen/vlk_device.static.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Creates a memory allocator using the Vulkan Memory Allocation library. */
static void create_allocator(_vlk_dev_t* dev);

/** Creates the command pool. */
static void create_command_pool(_vlk_dev_t* dev);

/** Creates descriptor set layouts. */
static void create_layouts(_vlk_dev_t* dev);

/** Creates the logical device. */
static void create_logical_device
	(
	_vlk_dev_t*						dev,				/* the logical device to initialize */
	utl_array_ptr_t(char)*			req_dev_ext,		/* required device extensions */
	utl_array_ptr_t(char)*			req_inst_layers		/* required instance layers */
	);

/** Creates render passes. */
static void create_render_pass(_vlk_dev_t* dev);

/** Creates a texture sampler. */
static void create_texture_sampler(_vlk_dev_t* dev);

/** Destroys the memory allocator. */
static void destroy_allocator(_vlk_dev_t* dev);

/** Destroys the command pool. */
static void destroy_command_pool(_vlk_dev_t* dev);

/** Destroys descriptor set layouts. */
static void destroy_layouts(_vlk_dev_t* dev);

/** Destroys the logical device. */
static void destroy_logical_device(_vlk_dev_t* dev);

/** Destroys render passes. */
static void destroy_render_pass(_vlk_dev_t* dev);

/** Destroys a texture sampler. */
static void destroy_texture_sampler(_vlk_dev_t* dev);

/** Checks if a format has a stencil component. */
static boolean has_stencil_component(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

/*=========================================================
CONSTRUCTORS
=========================================================*/

void _vlk_device__construct
	(
	_vlk_t*							vlk,				/* context */
	_vlk_dev_t*						dev,				/* the logical device to initialize */
	_vlk_gpu_t*						gpu,				/* physical device used by the logical device */
	utl_array_ptr_t(char)*			req_dev_ext,		/* required device extensions */
	utl_array_ptr_t(char)*			req_inst_layers		/* required instance layers */
	)
{
	clear_struct(dev);
	dev->gpu = gpu;
	dev->gfx_family_idx = -1;
	dev->present_family_idx = -1;
	utl_array_init(&dev->used_queue_families);

	create_logical_device(dev, req_dev_ext, req_inst_layers);
	create_command_pool(dev);
	create_allocator(dev);
	create_texture_sampler(dev);
	create_layouts(dev);
	create_render_pass(dev);
	create_picker_render_pass(dev);
}

void _vlk_device__destruct
	(
	_vlk_dev_t*						dev
	)
{
	destroy_picker_render_pass(dev);
	destroy_render_pass(dev);
	destroy_layouts(dev);
	destroy_texture_sampler(dev);
	destroy_allocator(dev);
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

void _vlk_device__copy_buffer_to_img_now
	(
	_vlk_dev_t*				dev,
	VkBuffer				buffer, 
	VkImage					image, 
	uint32_t				width, 
	uint32_t				height
	)
{
	VkCommandBuffer cmd_buf = _vlk_device__begin_one_time_cmd_buf(dev);

	VkBufferImageCopy region;
	clear_struct(&region);
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset.x = 0;
	region.imageOffset.y = 0;
	region.imageOffset.z = 0;
	region.imageExtent.width = width;
	region.imageExtent.height = height;
	region.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(
		cmd_buf,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	_vlk_device__end_one_time_cmd_buf(dev, cmd_buf);
}

/**
_vlk_device__create_shader_module
*/
VkShaderModule _vlk_device__create_shader(_vlk_dev_t* dev, const char* file)
{
	FILE* f;
	void* shader_code;
	long size;
	errno_t err;

	/* Open the compile shader binary (*.spv) */
	err = fopen_s(&f, file, "rb");
	if (err != 0 || !f)
	{
		log__fatal("Failed to open shader.");
	}

	/* Get file length */
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	if (size == 0)
	{
		log__fatal("Shader file is empty.");
	}

	/* Allocate memory */
	shader_code = malloc(size);
	if (!shader_code)
	{
		log__fatal("Failed to allocate shader code buffer.");
	}

	/* Read file into memory */
	fseek(f, 0, SEEK_SET);
	fread_s(shader_code, size, size, 1, f);

	/* Close file */
	fclose(f);

	/* Create the shader module */
	VkShaderModuleCreateInfo create_info;
	clear_struct(&create_info);
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = size;
	create_info.pCode = (uint32_t*)shader_code;

	VkShaderModule shader;
	if (vkCreateShaderModule(dev->handle, &create_info, NULL, &shader) != VK_SUCCESS)
	{
		log__fatal("Failed to create shader module.");
	}

	/* Cleanup memory */
	free(shader_code);

	return shader;
}

/**
_vlk_device__destroy_shader
*/
void _vlk_device__destroy_shader(_vlk_dev_t* dev, VkShaderModule shader)
{
	vkDestroyShaderModule(dev->handle, shader, NULL);
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
	else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else 
	{
		log__fatal("Unsupported layout transition.");
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
create_allocator
*/
static void create_allocator(_vlk_dev_t* dev)
{
	VmaAllocatorCreateInfo allocator_info;
	clear_struct(&allocator_info);
	allocator_info.physicalDevice = dev->gpu->handle;
	allocator_info.device = dev->handle;

	// TODO : can enable this later. Read docs to make sure to enable required extensions.
	//if (VK_KHR_dedicated_allocation_enabled)
	//{
	//    allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
	//}

	//VkAllocationCallbacks cpuAllocationCallbacks = {};
	//if (USE_CUSTOM_CPU_ALLOCATION_CALLBACKS)
	//{
	//    cpuAllocationCallbacks.pUserData = CUSTOM_CPU_ALLOCATION_CALLBACK_USER_DATA;
	//    cpuAllocationCallbacks.pfnAllocation = &CustomCpuAllocation;
	//    cpuAllocationCallbacks.pfnReallocation = &CustomCpuReallocation;
	//    cpuAllocationCallbacks.pfnFree = &CustomCpuFree;
	//    allocatorInfo.pAllocationCallbacks = &cpuAllocationCallbacks;
	//}

	if (vmaCreateAllocator(&allocator_info, &dev->allocator) != VK_SUCCESS)
	{
		log__fatal("Failed to create memory allocatory.");
	}
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
		log__fatal("Failed to create command pool.");
	}
}

/**
create_layouts
*/
void create_layouts(_vlk_dev_t* dev)
{
	_vlk_material_layout__construct(&dev->material_layout, dev);
	_vlk_per_view_layout__construct(&dev->per_view_layout, dev);
}

/**
create_logical_device
*/
static void create_logical_device
	(
	_vlk_dev_t*						dev,				/* the logical device to initialize */
	utl_array_ptr_t(char)*			req_dev_ext,		/* required device extensions */
	utl_array_ptr_t(char)*			req_inst_layers		/* required instance layers */
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
		log__fatal("No graphics family queue found.");
	}

	dev->gfx_family_idx = gpu->queue_family_indices.graphics_families.data[0];

	/* Check for present family */
	if (gpu->queue_family_indices.present_families.count == 0)
	{
		log__fatal("No present family queue found.");
	}

	dev->present_family_idx = gpu->queue_family_indices.present_families.data[0];

	/* Create list of used queue families */
	utl_array_push(&dev->used_queue_families, dev->gfx_family_idx);
	
	/* Queue family list must be unique */
	if (dev->gfx_family_idx != dev->present_family_idx)
	{
		utl_array_push(&dev->used_queue_families, dev->present_family_idx);
	}

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
		log__fatal("Failed to create logical device.");
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

//## static
static void create_picker_render_pass(_vlk_dev_t* dev)
{
	/*
	Attachment setup
	*/

	/* Primary color attachment */
	VkAttachmentDescription color_attach;
	clear_struct(&color_attach);
	color_attach.format = dev->gpu->optimal_surface_format.format;
	color_attach.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attach.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attach_ref;
	clear_struct(&color_attach_ref);
	color_attach_ref.attachment = 0;
	color_attach_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription attachments[] =
	{
		color_attach,
	};

	/*
	Subpass setup
	*/

	/* Primary subpass */
	VkSubpassDescription subpass;
	clear_struct(&subpass);
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attach_ref;

	VkSubpassDescription subpasses[] = { subpass };

	/*
	Subpass dependencies
	*/
	VkSubpassDependency dependency;
	clear_struct(&dependency);
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // implicit subpass before the render pass
	dependency.dstSubpass = 0; // 0 is index of our first subpass
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkSubpassDependency dependencies[] = { dependency };

	/*
	Renderpass setup
	*/
	VkRenderPassCreateInfo render_pass_info;
	clear_struct(&render_pass_info);
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = cnt_of_array(attachments);
	render_pass_info.pAttachments = attachments;
	render_pass_info.subpassCount = cnt_of_array(subpasses);
	render_pass_info.pSubpasses = &subpasses;
	render_pass_info.dependencyCount = cnt_of_array(dependencies);
	render_pass_info.pDependencies = &dependencies;

	if (vkCreateRenderPass(dev->handle, &render_pass_info, NULL, &dev->picker_render_pass) != VK_SUCCESS)
	{
		log__fatal("Failed to create render pass.");
	}
}

static void create_render_pass(_vlk_dev_t* dev)
{
	/*
	Attachment setup
	*/

	/* Primary color attachment */
	VkAttachmentDescription color_attach;
	clear_struct(&color_attach);
	color_attach.format = dev->gpu->optimal_surface_format.format;
	color_attach.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attach.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attach_ref;
	clear_struct(&color_attach_ref);
	color_attach_ref.attachment = 0;
	color_attach_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	/* Primary depth attachment */
	VkAttachmentDescription depth_attach;
	clear_struct(&depth_attach);
	depth_attach.format = _vlk_gpu__get_depth_format(dev->gpu);
	depth_attach.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attach.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depth_attach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attach.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_attach_ref;
	clear_struct(&depth_attach_ref);
	depth_attach_ref.attachment = 1;
	depth_attach_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription attachments[] =
	{
		color_attach,
		depth_attach,
	};

	/*
	Subpass setup
	*/

	/* Primary subpass */
	VkSubpassDescription subpass;
	clear_struct(&subpass);
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attach_ref;
	subpass.pDepthStencilAttachment = &depth_attach_ref;

	VkSubpassDescription subpasses[] = { subpass };

	/*
	Subpass dependencies
	*/
	VkSubpassDependency dependency;
	clear_struct(&dependency);
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // implicit subpass before the render pass
	dependency.dstSubpass = 0; // 0 is index of our first subpass
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkSubpassDependency dependencies[] = { dependency };

	/*
	Renderpass setup
	*/
	VkRenderPassCreateInfo render_pass_info;
	clear_struct(&render_pass_info);
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = cnt_of_array(attachments);
	render_pass_info.pAttachments = attachments;
	render_pass_info.subpassCount = cnt_of_array(subpasses);
	render_pass_info.pSubpasses = &subpasses;
	render_pass_info.dependencyCount = cnt_of_array(dependencies);
	render_pass_info.pDependencies = &dependencies;

	if (vkCreateRenderPass(dev->handle, &render_pass_info, NULL, &dev->render_pass) != VK_SUCCESS)
	{
		log__fatal("Failed to create render pass.");
	}
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
		log__fatal("Failed to create texture sampler.");
	}
}

/**
destroy_allocator
*/
static void destroy_allocator(_vlk_dev_t* dev)
{
	vmaDestroyAllocator(dev->allocator);
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
destroy_layouts
*/
void destroy_layouts(_vlk_dev_t* dev)
{
	_vlk_material_layout__destruct(&dev->material_layout);
	_vlk_per_view_layout__destruct(&dev->per_view_layout);
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

//## static
static void destroy_picker_render_pass(_vlk_dev_t* dev)
{
	vkDestroyRenderPass(dev->handle, dev->picker_render_pass, NULL);
}

static void destroy_render_pass(_vlk_dev_t* dev)
{
	vkDestroyRenderPass(dev->handle, dev->render_pass, NULL);
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