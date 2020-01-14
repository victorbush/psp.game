/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/gpu_frame.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "log/log.h"
#include "utl/utl_array.h"

#include "autogen/vlk_swapchain.static.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
_vlk_swapchain__init
*/
void _vlk_swapchain__init
	(
	_vlk_swapchain_t*				swap,			/* swapchain to init */
	_vlk_dev_t*						dev,			/* Logical device the surface is tied to    */
	VkSurfaceKHR					surface,		/* surface handle */
	VkExtent2D						extent			/* Desired swapchain extent */
	)
{
	clear_struct(swap);
	swap->dev = dev;
	swap->gpu = dev->gpu;
	swap->surface = surface;

	/* create everything */
	create_all(swap, extent);

	swap->last_time = glfwGetTime();
}

/**
_vlk_swapchain__term
*/
void _vlk_swapchain__term(_vlk_swapchain_t* swap)
{
	destroy_all(swap);
}

/*=========================================================
FUNCTIONS
=========================================================*/

/**
_vlk_swapchain__begin_frame
*/
void _vlk_swapchain__begin_frame(_vlk_swapchain_t* swap, _vlk_t* vlk, _vlk_frame_t* frame)
{
	_vlk_frame_status_t frame_status = _VLK_FRAME_STATUS_VALID;

	vkWaitForFences(swap->dev->handle, 1, &swap->in_flight_fences[frame->frame_idx], VK_TRUE, UINT64_MAX);
	vkResetFences(swap->dev->handle, 1, &swap->in_flight_fences[frame->frame_idx]);

	VkResult result = vkAcquireNextImageKHR(swap->dev->handle, swap->handle, UINT64_MAX, swap->image_avail_semaphores[frame->frame_idx], VK_NULL_HANDLE, &frame->image_idx);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		resize(swap, swap->extent);
		frame_status = _VLK_FRAME_STATUS_SWAPCHAIN_OUT_OF_DATE;
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		log__fatal("Failed to acquire swapchain image.");
	}

	/* Start render passes */
	begin_primary_render_pass(swap, frame);
	begin_picker_render_pass(swap, frame);

	/* Calc frame timing */
	double curTime = glfwGetTime();
	frame->delta_time = curTime - swap->last_time;
	swap->last_time = curTime;
}

/**
_vlk_swapchain__end_frame
*/
void _vlk_swapchain__end_frame(_vlk_swapchain_t* swap, _vlk_frame_t* frame)
{
	uint32_t img_idx = frame->image_idx;
	VkCommandBuffer cmd = swap->cmd_bufs[img_idx];

	/* End the primary render pass */
	vkCmdEndRenderPass(cmd);

	VkResult result = vkEndCommandBuffer(cmd);
	if (result != VK_SUCCESS)
	{
		log__fatal("Failed to record command buffer.");
	}

	/* End the picker buffer render pass */
	vkCmdEndRenderPass(swap->picker_cmd_bufs[img_idx]);

	result = vkEndCommandBuffer(swap->picker_cmd_bufs[img_idx]);
	if (result != VK_SUCCESS)
	{
		log__fatal("Failed to record command buffer.");
	}

	VkCommandBuffer cmd_buffers[] =
	{
		swap->cmd_bufs[img_idx],
		swap->picker_cmd_bufs[img_idx],
	};


	VkSubmitInfo submit_info;
	clear_struct(&submit_info);
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore swait_semaphores[] = { swap->image_avail_semaphores[frame->frame_idx] };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = swait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;

	submit_info.commandBufferCount = cnt_of_array(cmd_buffers);
	submit_info.pCommandBuffers = cmd_buffers;

	VkSemaphore signal_semaphores[] = { swap->render_finished_semaphores[frame->frame_idx] };
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	result = vkQueueSubmit(swap->dev->gfx_queue, 1, &submit_info, swap->in_flight_fences[frame->frame_idx]);

	if (result != VK_SUCCESS)
	{
		log__fatal("Failed to submit draw command buffer.");
	}

	VkPresentInfoKHR present_info;
	clear_struct(&present_info);
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;

	VkSwapchainKHR swapchains[] = { swap->handle };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;
	present_info.pImageIndices = &img_idx;
	present_info.pResults = NULL; // Optional

	result = vkQueuePresentKHR(swap->dev->present_queue, &present_info);

	if (result == VK_ERROR_OUT_OF_DATE_KHR
		|| result == VK_SUBOPTIMAL_KHR)
	{
		resize(swap, swap->extent);
		return;
	}
	else if (result != VK_SUCCESS)
	{
		log__fatal("Failed to acquire swap chain image.");
	}
}

/**
_vlk_swapchain__get_cmd_buf
*/
VkCommandBuffer _vlk_swapchain__get_cmd_buf(_vlk_swapchain_t* swap, _vlk_frame_t *frame)
{
	return swap->cmd_bufs[frame->image_idx];
}

/**
_vlk_swapchain__get_extent
*/
VkExtent2D _vlk_swapchain__get_extent(_vlk_swapchain_t* swap)
{
	return swap->extent;
}

/**
_vlk_swapchain__recreate
*/
void _vlk_swapchain__recreate(_vlk_swapchain_t* swap, int width, int height)
{
	VkExtent2D extent;
	extent.width = width;
	extent.height = height;

	resize(swap, extent);
}

/*=========================================================
STATIC FUNCTIONS
=========================================================*/

//## static
static void begin_picker_render_pass(_vlk_swapchain_t* swap, _vlk_frame_t* frame)
{
	/*
	Begin picker buffer render pass command buffer
	*/
	frame->picker_cmd_buf = swap->picker_cmd_bufs[frame->image_idx];

	VkCommandBufferBeginInfo begin_info;
	clear_struct(&begin_info);
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	begin_info.pInheritanceInfo = NULL; // Optional

	if (vkBeginCommandBuffer(frame->picker_cmd_buf, &begin_info) != VK_SUCCESS)
	{
		log__fatal("Failed to begin recording command buffer.");
	}

	VkRenderPassBeginInfo render_pass_info;
	clear_struct(&render_pass_info);
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = swap->dev->picker_render_pass;
	render_pass_info.framebuffer = swap->picker_frame_bufs[frame->image_idx];
	render_pass_info.renderArea.offset.x = 0;
	render_pass_info.renderArea.offset.y = 0;
	render_pass_info.renderArea.extent = swap->extent;

	/* Initialize picker buffer to 0xFFFFFFFF. This is the invalid entity id. */
	VkClearValue clear_values[1];
	memset(&clear_values, 0, sizeof(clear_values));
	clear_values[0].color.float32[0] = 1.0f;
	clear_values[0].color.float32[1] = 1.0f;
	clear_values[0].color.float32[2] = 1.0f;
	clear_values[0].color.float32[3] = 1.0f;

	render_pass_info.clearValueCount = cnt_of_array(clear_values);
	render_pass_info.pClearValues = clear_values;

	vkCmdBeginRenderPass(frame->picker_cmd_buf, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}

//## static
static void begin_primary_render_pass(_vlk_swapchain_t* swap, _vlk_frame_t* frame)
{
	/*
	Begin command buffer
	*/
	frame->cmd_buf = swap->cmd_bufs[frame->image_idx];

	VkCommandBufferBeginInfo begin_info;
	clear_struct(&begin_info);
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	begin_info.pInheritanceInfo = NULL; // Optional

	if (vkBeginCommandBuffer(frame->cmd_buf, &begin_info) != VK_SUCCESS)
	{
		log__fatal("Failed to begin recording command buffer.");
	}

	VkRenderPassBeginInfo render_pass_info;
	clear_struct(&render_pass_info);
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = swap->dev->render_pass;
	render_pass_info.framebuffer = swap->frame_bufs[frame->image_idx];
	render_pass_info.renderArea.offset.x = 0;
	render_pass_info.renderArea.offset.y = 0;
	render_pass_info.renderArea.extent = swap->extent;

	VkClearValue clear_values[2];
	memset(&clear_values, 0, sizeof(clear_values));
	clear_values[0].color.float32[0] = 0.0f;
	clear_values[0].color.float32[1] = 0.0f;
	clear_values[0].color.float32[2] = 0.0f;
	clear_values[0].color.float32[3] = 1.0f;
	clear_values[1].depthStencil.depth = 1.0f;
	clear_values[1].depthStencil.stencil = 0;

	render_pass_info.clearValueCount = cnt_of_array(clear_values);
	render_pass_info.pClearValues = clear_values;

	vkCmdBeginRenderPass(frame->cmd_buf, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}

//## static
/**
Chooses best swap extent based on surface capabilties.
*/
static VkExtent2D choose_swap_extent(_vlk_swapchain_t* swap, VkExtent2D desired_extent, VkSurfaceCapabilitiesKHR *capabilities)
{
    VkExtent2D extent;

	if (capabilities->currentExtent.width != UINT32_MAX)
	{
		/* extent already defined by surface */
		extent = capabilities->currentExtent;
	}
	else
	{
		/* swap chain must set extent */
		extent = desired_extent;
	}

    return extent;
}

//## static
/**
Creates the swap chain and all associated resources.
*/
static void create_all(_vlk_swapchain_t* swap, VkExtent2D extent)
{
	/*
	Order matters for these. These are recreated on resize.
	*/
	create_swapchain(swap, extent);
	create_image_views(swap);
	create_depth_buffer(swap);
	create_picker_buffers(swap);
	create_framebuffers(swap);

	/*
	Order doesn't matter for these. Does not need recreated on resize.
	*/
	create_command_buffers(swap);
	create_semaphores(swap);
}

//## static
/**
create_command_buffers
*/
static void create_command_buffers(_vlk_swapchain_t* swap)
{
	memset(swap->cmd_bufs, 0, sizeof(swap->cmd_bufs));

	VkCommandBufferAllocateInfo alloc_info;
	clear_struct(&alloc_info);
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = swap->dev->command_pool;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = cnt_of_array(swap->cmd_bufs);

	if (vkAllocateCommandBuffers(swap->dev->handle, &alloc_info, swap->cmd_bufs) != VK_SUCCESS)
	{
		log__fatal("Failed to allocate command buffers.");
	}

	/*
	Create command buffers for picker buffer render pass
	*/
	memset(swap->picker_cmd_bufs, 0, sizeof(swap->picker_cmd_bufs));

	//VkCommandBufferAllocateInfo alloc_info;
	clear_struct(&alloc_info);
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = swap->dev->command_pool;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = cnt_of_array(swap->picker_cmd_bufs);

	if (vkAllocateCommandBuffers(swap->dev->handle, &alloc_info, swap->picker_cmd_bufs) != VK_SUCCESS)
	{
		log__fatal("Failed to allocate command buffers.");
	}
}

//## static
/**
create_depth_buffer
*/
static void create_depth_buffer(_vlk_swapchain_t* swap)
{
	VkResult result;
	VkFormat depth_format = _vlk_gpu__get_depth_format(swap->dev->gpu);
	memset(swap->depth_images, 0, sizeof(swap->depth_images));
	memset(swap->depth_image_views, 0, sizeof(swap->depth_image_views));
	memset(swap->depth_image_allocations, 0, sizeof(swap->depth_image_allocations));

	for (size_t i = 0; i < cnt_of_array(swap->depth_images); i++)
	{
		/*
		Create image
		*/
		VkImageCreateInfo image_info;
		clear_struct(&image_info);
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.extent.width = swap->extent.width;
		image_info.extent.height = swap->extent.height;
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;

		image_info.format = depth_format;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.flags = 0; // Optional

		VmaAllocationCreateInfo alloc_info;
		clear_struct(&alloc_info);
		alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		/* https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/issues/34 */
		alloc_info.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

		result = vmaCreateImage(swap->dev->allocator, &image_info, &alloc_info, &swap->depth_images[i], &swap->depth_image_allocations[i], NULL);
		if (result != VK_SUCCESS) 
		{
			log__fatal("Failed to create depth buffer image.");
		}

		/*
		Create image view
		*/
		VkImageViewCreateInfo view_info;
		clear_struct(&view_info);
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = swap->depth_images[i];
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = depth_format;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(swap->dev->handle, &view_info, NULL, &swap->depth_image_views[i]) != VK_SUCCESS)
		{
			log__fatal("Failed to create depth texture image view.");
		}

		_vlk_device__transition_image_layout(swap->dev, swap->depth_images[i], depth_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}
}

//## static
/**
create_framebuffers
*/
static void create_framebuffers(_vlk_swapchain_t* swap)
{
	uint32_t i;
	memset(swap->frame_bufs, 0, sizeof(swap->frame_bufs));

	for (i = 0; i < cnt_of_array(swap->frame_bufs); i++) 
	{
		VkImageView attachments[] =
		{
			swap->image_views[i],
			swap->depth_image_views[i],
		};

		VkFramebufferCreateInfo framebuf_info;
		clear_struct(&framebuf_info);
		framebuf_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuf_info.renderPass = swap->dev->render_pass;
		framebuf_info.attachmentCount = cnt_of_array(attachments);
		framebuf_info.pAttachments = attachments;
		framebuf_info.width = swap->extent.width;
		framebuf_info.height = swap->extent.height;
		framebuf_info.layers = 1;

		if (vkCreateFramebuffer(swap->dev->handle, &framebuf_info, NULL, &swap->frame_bufs[i]) != VK_SUCCESS)
		{
			log__fatal("Failed to create framebuffer.");
		}
	}
}

//## static
/**
create_image_views
*/
static void create_image_views(_vlk_swapchain_t* swap)
{
	uint32_t i;
	memset(swap->image_views, 0, sizeof(swap->image_views));

	for (i = 0; i < cnt_of_array(swap->image_views); i++) 
	{
		VkImageViewCreateInfo create_info;
		clear_struct(&create_info);
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = swap->images[i];
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = swap->surface_format.format;
		create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(swap->dev->handle, &create_info, NULL, &swap->image_views[i]) != VK_SUCCESS)
		{
			log__fatal("Failed to create image views.");
		}
	}
}

//## static
/**
Creates images and image views for the picker buffers. Objects are rendered
to the a picker buffer using a unique color. The buffer can then be sampled at a certain
pixel to determine what object is drawn at that pixel. It allows "picking" objects on the screen.
*/
static void create_picker_buffers(_vlk_swapchain_t* swap)
{
	for (int i = 0; i < cnt_of_array(swap->picker_images); ++i)
	{
		/*
		Create image
		*/
		VkImageCreateInfo image_info;
		clear_struct(&image_info);
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.extent.width = swap->extent.width;
		image_info.extent.height = swap->extent.height;
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;

		image_info.format = VK_FORMAT_B8G8R8A8_UNORM;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.flags = 0;

		VmaAllocationCreateInfo alloc_info;
		clear_struct(&alloc_info);
		alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		alloc_info.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

		VkResult result = vmaCreateImage(swap->dev->allocator, &image_info, &alloc_info, &swap->picker_images[i], &swap->picker_image_allocations[i], NULL);
		if (result != VK_SUCCESS) 
		{
			log__fatal("Failed to create picker buffer image.");
		}
		
		_vlk_device__transition_image_layout(swap->dev, swap->picker_images[i], image_info.format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		/*
		Create image view
		*/
		VkImageViewCreateInfo view_info;
		clear_struct(&view_info);
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = swap->picker_images[i];
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = image_info.format;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(swap->dev->handle, &view_info, NULL, &swap->picker_image_views[i]) != VK_SUCCESS)
		{
			log__fatal("Failed to create picker buffer image view.");
		}

		/*
		Create frame buffers
		*/
		VkImageView attachments[] =
		{
			swap->picker_image_views[i],
		};

		VkFramebufferCreateInfo framebuf_info;
		clear_struct(&framebuf_info);
		framebuf_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuf_info.renderPass = swap->dev->picker_render_pass;
		framebuf_info.attachmentCount = cnt_of_array(attachments);
		framebuf_info.pAttachments = attachments;
		framebuf_info.width = swap->extent.width;
		framebuf_info.height = swap->extent.height;
		framebuf_info.layers = 1;

		if (vkCreateFramebuffer(swap->dev->handle, &framebuf_info, NULL, &swap->picker_frame_bufs[i]) != VK_SUCCESS)
		{
			log__fatal("Failed to create framebuffer.");
		}
	}
}

//## static
/**
create_semaphores
*/
static void create_semaphores(_vlk_swapchain_t* swap)
{
	VkDevice device = swap->dev->handle;
	memset(swap->image_avail_semaphores, 0, sizeof(swap->image_avail_semaphores));
	memset(swap->render_finished_semaphores, 0, sizeof(swap->render_finished_semaphores));
	memset(swap->in_flight_fences, 0, sizeof(swap->in_flight_fences));

	VkSemaphoreCreateInfo semaphore_info;
	clear_struct(&semaphore_info);
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info;
	clear_struct(&fence_info);
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < cnt_of_array(swap->in_flight_fences); i++)
	{
		if (vkCreateSemaphore(device, &semaphore_info, NULL, &swap->image_avail_semaphores[i]) != VK_SUCCESS
			|| vkCreateSemaphore(device, &semaphore_info, NULL, &swap->render_finished_semaphores[i]) != VK_SUCCESS
			|| vkCreateFence(device, &fence_info, NULL, &swap->in_flight_fences[i]) != VK_SUCCESS)
		{
			log__fatal("Failed to create semaphores for a frame.");
		}
	}
}

//## static
/**
create_swapchain
*/
static void create_swapchain(_vlk_swapchain_t* swap, VkExtent2D extent)
{
	uint32_t image_count = NUM_FRAMES;

	/*
	Get surface capabilties
	*/
	VkSurfaceCapabilitiesKHR surface_capabilities;
	clear_struct(&surface_capabilities);
	VkResult result = _vlk_gpu__query_surface_capabilties(swap->dev->gpu, swap->surface, &surface_capabilities);

	if (result != VK_SUCCESS)
	{
		log__fatal("Failed to create semaphores for a frame.");
	}

	/*
	Determine swapchain properties
	*/
	swap->present_mode = swap->dev->gpu->optimal_present_mode;
	swap->surface_format = swap->dev->gpu->optimal_surface_format;
	swap->extent = choose_swap_extent(swap, extent, &surface_capabilities);

	/*
	Build create info for the swap chain
	*/
	VkSwapchainCreateInfoKHR create_info;
	clear_struct(&create_info);
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = swap->surface;

	create_info.minImageCount = image_count;
	create_info.imageFormat = swap->surface_format.format;
	create_info.imageColorSpace = swap->surface_format.colorSpace;
	create_info.imageExtent = swap->extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	/* queue family sharing */
	uint32_t queueFamilyIndices[] = 
	{
		swap->dev->gfx_family_idx, 
		swap->dev->present_family_idx
	};

	if (swap->dev->gfx_family_idx != swap->dev->present_family_idx)
	{
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queueFamilyIndices;
	}
	else 
	{
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0; // Optional
		create_info.pQueueFamilyIndices = NULL; // Optional
	}

	create_info.preTransform = surface_capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = swap->present_mode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	/*
	Create the swap chain
	*/
	result = vkCreateSwapchainKHR(swap->dev->handle, &create_info, NULL, &swap->handle);
	if (result != VK_SUCCESS)
	{
		log__fatal("Failed to create swap chain.");
	}

	/*
	Get swap chain images
	*/
	vkGetSwapchainImagesKHR(swap->dev->handle, swap->handle, &image_count, NULL);
	vkGetSwapchainImagesKHR(swap->dev->handle, swap->handle, &image_count, swap->images);
}

//## static
/**
Destroys the swap chain and all associated resources.
*/
static void destroy_all(_vlk_swapchain_t* swap)
{
	destroy_semaphores(swap);
	destroy_command_buffers(swap);

	destroy_framebuffers(swap);
	destroy_picker_buffers(swap);
	destroy_depth_buffer(swap);
	destroy_image_views(swap);
	destroy_swapchain(swap);
}

//## static
/**
destroy_command_buffers
*/
static void destroy_command_buffers(_vlk_swapchain_t* swap)
{
	/*
	Do nothing. Command buffers are cleaned up automatically by their command pool.
	*/
}

//## static
/**
destroy_depth_buffer
*/
static void destroy_depth_buffer(_vlk_swapchain_t* swap)
{
	uint32_t i;

	for (i = 0; i < cnt_of_array(swap->images); i++)
	{
		vkDestroyImageView(swap->dev->handle, swap->depth_image_views[i], NULL);
		vmaDestroyImage(swap->dev->allocator, swap->depth_images[i], swap->depth_image_allocations[i]);
	}
}

//## static
/**
destroy_framebuffers
*/
static void destroy_framebuffers(_vlk_swapchain_t* swap)
{
	uint32_t i;

	for (i = 0; i < cnt_of_array(swap->frame_bufs); ++i)
	{
		vkDestroyFramebuffer(swap->dev->handle, swap->frame_bufs[i], NULL);
	}
}

//## static
/**
destroy_image_views
*/
static void destroy_image_views(_vlk_swapchain_t* swap)
{
	uint32_t i;

	for (i = 0; i < cnt_of_array(swap->image_views); ++i)
	{
		vkDestroyImageView(swap->dev->handle, swap->image_views[i], NULL);
	}
}

//## static
/**
Destroys the picker buffer images and image views.
*/
static void destroy_picker_buffers(_vlk_swapchain_t* swap)
{
	uint32_t i;

	for (i = 0; i < cnt_of_array(swap->picker_images); i++)
	{
		vkDestroyFramebuffer(swap->dev->handle, swap->picker_frame_bufs[i], NULL);
		vkDestroyImageView(swap->dev->handle, swap->picker_image_views[i], NULL);
		vmaDestroyImage(swap->dev->allocator, swap->picker_images[i], swap->picker_image_allocations[i]);
	}
}

//## static
/**
destroy_semaphores
*/
static void destroy_semaphores(_vlk_swapchain_t* swap) 
{
	for (size_t i = 0; i < cnt_of_array(swap->in_flight_fences); i++) {
		vkDestroySemaphore(swap->dev->handle, swap->render_finished_semaphores[i], NULL);
		vkDestroySemaphore(swap->dev->handle, swap->image_avail_semaphores[i], NULL);
		vkDestroyFence(swap->dev->handle, swap->in_flight_fences[i], NULL);
	}
}

//## static
/**
destroy_swapchain
*/
static void destroy_swapchain(_vlk_swapchain_t* swap)
{
	vkDestroySwapchainKHR(swap->dev->handle, swap->handle, NULL);
}

//## static
/**
Recreates the swap chain and associated resources.
*/
static void resize(_vlk_swapchain_t* swap, VkExtent2D extent)
{
	/* wait until device is idle before recreation */
	vkDeviceWaitIdle(swap->dev->handle);

	/* destroy things that need recreated */
	destroy_framebuffers(swap);
	destroy_picker_buffers(swap);
	destroy_depth_buffer(swap);
	destroy_image_views(swap);
	destroy_swapchain(swap);

	/* re-create resources */
	create_swapchain(swap, extent);
	create_image_views(swap);
	create_depth_buffer(swap);
	create_picker_buffers(swap);
	create_framebuffers(swap);
}