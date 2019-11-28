/*=========================================================
INCLUDES
=========================================================*/

#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "platforms/common.h"
#include "utl/utl_array.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/**
Chooses best swap extent based on surface capabilties.
*/
VkExtent2D choose_swap_extent(_vlk_swapchain_t* swap, VkSurfaceCapabilitiesKHR *capabilities);

/**
Creates the swap chain and all associated resources.
*/
static void create_all(_vlk_swapchain_t* swap);

/**
Creates command buffers.
*/
static void create_command_buffers(_vlk_swapchain_t* swap);

/**
Creates the depth buffer.
*/
static void create_depth_buffer(_vlk_swapchain_t* swap);

/**
Creates frame buffers for the swapchain.
*/
static void create_framebuffers(_vlk_swapchain_t* swap);

/**
Creates image views for the swapchain images.
*/
static void create_image_views(_vlk_swapchain_t* swap);

/**
Creates the render pass.
*/
static void create_render_pass(_vlk_swapchain_t* swap);

/**
Creates semaphores.
*/
static void create_semaphores(_vlk_swapchain_t* swap);

/**
Creates the swap chain.
*/
static void create_swapchain(_vlk_swapchain_t* swap);

/**
Destroys the swap chain and all associated resources.
*/
static void destroy_all(_vlk_swapchain_t* swap);

/**
Destroys command buffers.
*/
static void destroy_command_buffers(_vlk_swapchain_t* swap);

/**
Destroys the depth buffer.
*/
static void destroy_depth_buffer(_vlk_swapchain_t* swap);

/**
Destroys the framebuffers.
*/
static void destroy_framebuffers(_vlk_swapchain_t* swap);

/**
Destroys the swapchain imageviews.
*/
static void destroy_image_views(_vlk_swapchain_t* swap);

/**
Destroys the render pass.
*/
static void destroy_render_pass(_vlk_swapchain_t* swap);

/**
Destroys semaphores.
*/
static void destroy_semaphores(_vlk_swapchain_t* swap);

/**
Destroys the swap chain.
*/
static void destroy_swapchain(_vlk_swapchain_t* swap);

/**
Recreates the swap chain and associated resources.
*/
static void resize(_vlk_swapchain_t* swap);

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
	GLFWwindow*						window			/* window the surface will present to */
	)
{
	clear_struct(swap);
	swap->dev = dev;
	swap->gpu = dev->gpu;
	swap->surface = surface;
	swap->window = window;

	/* create everything */
	create_all(swap);

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
void _vlk_swapchain__begin_frame(_vlk_swapchain_t* swap)
{
	_vlk_frame_t* frame = &swap->frame;
	clear_struct(frame);
	frame->frame_idx = swap->frame_idx;
	frame->status = _VLK_FRAME_STATUS_VALID;

	vkWaitForFences(swap->dev->handle, 1, &swap->in_flight_fences[swap->frame_idx], VK_TRUE, UINT64_MAX);
	vkResetFences(swap->dev->handle, 1, &swap->in_flight_fences[swap->frame_idx]);

	VkResult result = vkAcquireNextImageKHR(swap->dev->handle, swap->handle, UINT64_MAX, swap->image_avail_semaphores[swap->frame_idx], VK_NULL_HANDLE, &frame->image_idx);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		resize(swap);
		frame->status = _VLK_FRAME_STATUS_SWAPCHAIN_OUT_OF_DATE;
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		FATAL("Failed to acquire swapchain image.");
	}

	/*
	Begin command buffer
	*/
	VkCommandBuffer cmd = swap->cmd_bufs[frame->image_idx];

	VkCommandBufferBeginInfo begin_info;
	clear_struct(&begin_info);
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	begin_info.pInheritanceInfo = NULL; // Optional

	if (vkBeginCommandBuffer(cmd, &begin_info) != VK_SUCCESS)
	{
		FATAL("Failed to begin recording command buffer.");
	}

	VkRenderPassBeginInfo render_pass_info;
	clear_struct(&render_pass_info);
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = swap->render_pass;
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

	vkCmdBeginRenderPass(cmd, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

	swap->frame_idx = (swap->frame_idx + 1) % NUM_FRAMES;

	double curTime = glfwGetTime();
	frame->delta_time = curTime - swap->last_time;
	swap->last_time = curTime;

	frame->width = swap->extent.width;
	frame->height = swap->extent.height;
}

/**
_vlk_swapchain__end_frame
*/
void _vlk_swapchain__end_frame(_vlk_swapchain_t* swap)
{
	_vlk_frame_t* frame = &swap->frame;
	uint32_t img_idx = frame->image_idx;
	VkCommandBuffer cmd = swap->cmd_bufs[img_idx];

	vkCmdEndRenderPass(cmd);

	auto result = vkEndCommandBuffer(cmd);
	if (result != VK_SUCCESS)
	{
		FATAL("Failed to record command buffer.");
	}

	VkSubmitInfo submit_info;
	clear_struct(&submit_info);
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore swait_semaphores[] = { swap->image_avail_semaphores[frame->frame_idx] };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = swait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &swap->cmd_bufs[img_idx];

	VkSemaphore signal_semaphores[] = { swap->render_finished_semaphores[frame->frame_idx] };
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	result = vkQueueSubmit(swap->dev->gfx_queue, 1, &submit_info, swap->in_flight_fences[frame->frame_idx]);

	if (result != VK_SUCCESS)
	{
		FATAL("Failed to submit draw command buffer.");
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
		resize(swap);
		return;
	}
	else if (result != VK_SUCCESS)
	{
		FATAL("Failed to acquire swap chain image.");
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
_vlk_swapchain__get_render_pass
*/
VkRenderPass _vlk_swapchain__get_render_pass(_vlk_swapchain_t* swap)
{
	return swap->render_pass;
}

/**
_vlk_swapchain__recreate
*/
void _vlk_swapchain__recreate(_vlk_swapchain_t* swap, int width, int height)
{
	// do it now
	resize(swap);
}

/*=========================================================
PRIVATE FUNCTIONS
=========================================================*/

/**
choose_swap_extent
*/
VkExtent2D choose_swap_extent(_vlk_swapchain_t* swap, VkSurfaceCapabilitiesKHR *capabilities)
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
        int width, height;
        glfwGetFramebufferSize(swap->window, &width, &height);

        extent.width = (uint32_t)width;
        extent.height = (uint32_t)height;
    }

    return extent;
}

/**
create_all
*/
static void create_all(_vlk_swapchain_t* swap)
{
	/*
	Order matters for these
	*/
	create_swapchain(swap);		/* Recreated on resize */
	create_image_views(swap);	/* Recreated on resize */
	create_depth_buffer(swap);	/* Recreated on resize */
	create_render_pass(swap);	/* Does not need recreated on resize */
	create_framebuffers(swap);	/* Recreated on resize */

	/*
	Order doesn't matter for these. Does not need recreated on resize.
	*/
	create_command_buffers(swap);
	create_semaphores(swap);
}

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
	alloc_info.commandBufferCount = cnt_of_array(swap->cmd_bufs);;

	if (vkAllocateCommandBuffers(swap->dev->handle, &alloc_info, swap->cmd_bufs) != VK_SUCCESS)
	{
		FATAL("Failed to allocate command buffers.");
	}
}

/**
create_depth_buffer
*/
static void create_depth_buffer(_vlk_swapchain_t* swap)
{
	VkResult result;
	VkFormat depth_format = _vlk_gpu__get_depth_format(swap->dev->gpu);
	memset(swap->depth_images, 0, sizeof(swap->depth_images));
	memset(swap->depth_image_views, 0, sizeof(swap->depth_image_views));
	memset(swap->depth_image_memory, 0, sizeof(swap->depth_image_memory));

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

		result = vkCreateImage(swap->dev->handle, &image_info, NULL, &swap->depth_images[i]);
		if (result != VK_SUCCESS)
		{
			FATAL("Failed to create depth image.");
		}

		/* 
		Allocate image memory 
		*/
		VkMemoryRequirements mem_req;
		clear_struct(&mem_req);
		vkGetImageMemoryRequirements(swap->dev->handle, swap->depth_images[i], &mem_req);

		VkMemoryAllocateInfo mem_alloc_info;
		clear_struct(&mem_alloc_info);
		mem_alloc_info.allocationSize = mem_req.size;
		mem_alloc_info.memoryTypeIndex = _vlk_gpu__find_memory_type_idx(swap->gpu, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		result = vkAllocateMemory(swap->dev->handle, &mem_alloc_info, NULL, &swap->depth_image_memory[i]);
		if (result != VK_SUCCESS)
		{
			FATAL("Failed to allocate depth image memory.");
		}
		
		result = vkBindImageMemory(swap->dev->handle, swap->depth_images[i], swap->depth_image_memory[i], 0);
		if (result != VK_SUCCESS)
		{
			FATAL("Failed to bind depth image memory.");
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
			FATAL("Failed to create depth texture image view.");
		}

		_vlk_device__transition_image_layout(swap->dev, swap->depth_images[i], depth_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}
}

/**
create_framebuffers
*/
static void create_framebuffers(_vlk_swapchain_t* swap)
{
	uint32_t i;
	memset(swap->frame_bufs, 0, sizeof(swap->frame_bufs));

	for (i = 0; i < cnt_of_array(swap->frame_bufs); i++) 
	{
		VkImageView attachments[2];
		memset(attachments, 0, sizeof(attachments));
		attachments[0] = swap->image_views[i];
		attachments[1] = swap->depth_image_views[i];

		VkFramebufferCreateInfo framebuf_info;
		clear_struct(&framebuf_info);
		framebuf_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuf_info.renderPass = swap->render_pass;
		framebuf_info.attachmentCount = cnt_of_array(attachments);
		framebuf_info.pAttachments = attachments;
		framebuf_info.width = swap->extent.width;
		framebuf_info.height = swap->extent.height;
		framebuf_info.layers = 1;

		if (vkCreateFramebuffer(swap->dev->handle, &framebuf_info, NULL, &swap->frame_bufs[i]) != VK_SUCCESS)
		{
			FATAL("Failed to create framebuffer.");
		}
	}
}

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
			FATAL("Failed to create image views.");
		}
	}
}

/**
create_render_pass
*/
static void create_render_pass(_vlk_swapchain_t* swap)
{
	VkAttachmentDescription color_attach;
	clear_struct(&color_attach);
	color_attach.format = swap->surface_format.format;
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

	VkAttachmentDescription depth_attach;
	clear_struct(&depth_attach);
	depth_attach.format = _vlk_gpu__get_depth_format(swap->gpu);
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

	VkSubpassDescription subpass;
	clear_struct(&subpass);
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attach_ref;
	subpass.pDepthStencilAttachment = &depth_attach_ref;

	VkSubpassDependency dependency;
	clear_struct(&dependency);
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // implicit subpass before the render pass
	dependency.dstSubpass = 0; // 0 is index of our first subpass
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkAttachmentDescription attachments[2];
	memset(attachments, 0, sizeof(attachments));
	attachments[0] = color_attach;
	attachments[1] = depth_attach;

	VkRenderPassCreateInfo render_pass_info;
	clear_struct(&render_pass_info);
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = cnt_of_array(attachments);
	render_pass_info.pAttachments = attachments;
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies = &dependency;

	if (vkCreateRenderPass(swap->dev->handle, &render_pass_info, NULL, &swap->render_pass) != VK_SUCCESS)
	{
		FATAL("Failed to create render pass.");
	}
}

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
			FATAL("Failed to create semaphores for a frame.");
		}
	}
}

/**
create_swapchain
*/
static void create_swapchain(_vlk_swapchain_t* swap)
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
		FATAL("Failed to create semaphores for a frame.");
	}

	/*
	Determine swapchain properties
	*/
	swap->present_mode = swap->dev->gpu->optimal_present_mode;
	swap->surface_format = swap->dev->gpu->optimal_surface_format;
	swap->extent = choose_swap_extent(swap, &surface_capabilities);

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
	if (vkCreateSwapchainKHR(swap->dev->handle, &create_info, NULL, &swap->handle) != VK_SUCCESS)
	{
		FATAL("Failed to create swap chain.");
	}

	/*
	Get swap chain images
	*/
	vkGetSwapchainImagesKHR(swap->dev->handle, swap->handle, &image_count, swap->images);
}

/**
destroy_all
*/
static void destroy_all(_vlk_swapchain_t* swap)
{
	destroy_semaphores(swap);
	destroy_command_buffers(swap);

	destroy_framebuffers(swap);
	destroy_render_pass(swap);
	destroy_depth_buffer(swap);
	destroy_image_views(swap);
	destroy_swapchain(swap);
}

/**
destroy_command_buffers
*/
static void destroy_command_buffers(_vlk_swapchain_t* swap)
{
	/*
	Do nothing. Command buffers are cleaned up automatically by their command pool.
	*/
}

/**
destroy_depth_buffer
*/
static void destroy_depth_buffer(_vlk_swapchain_t* swap)
{
	uint32_t i;

	for (i = 0; i < cnt_of_array(swap->images); i++)
	{
		vkDestroyImageView(swap->dev->handle, swap->depth_image_views[i], NULL);
		vkUnmapMemory(swap->dev->handle, swap->depth_image_memory[i]);
		vkFreeMemory(swap->dev->handle, swap->depth_image_memory[i], NULL);
		vkDestroyImage(swap->dev->handle, swap->depth_images[i], NULL);
	}
}

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

/**
destroy_render_pass
*/
static void destroy_render_pass(_vlk_swapchain_t* swap)
{
	vkDestroyRenderPass(swap->dev->handle, swap->render_pass, NULL);
}

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

/**
destroy_swapchain
*/
static void destroy_swapchain(_vlk_swapchain_t* swap)
{
	vkDestroySwapchainKHR(swap->dev->handle, swap->handle, NULL);
}

/**
resize
*/
static void resize(_vlk_swapchain_t* swap)
{
	// TODO : probably shouldn't stall program here waiting?

	/* width/height will be 0 if minimzed, so wait... */
	int width = 0, height = 0;
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(swap->window, &width, &height);
		glfwWaitEvents();
	}

	/* wait until device is idle before recreation */
	vkDeviceWaitIdle(swap->dev->handle);

	/* destroy things that need recreated */
	destroy_framebuffers(swap);
	destroy_depth_buffer(swap);
	destroy_image_views(swap);
	destroy_swapchain(swap);

	/* re-create resources */
	create_swapchain(swap);
	create_image_views(swap);
	create_depth_buffer(swap);
	create_framebuffers(swap);
}