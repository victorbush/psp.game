/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_camera.h"
#include "engine/kk_log.h"
#include "gpu/gpu_window.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "gpu/vlk/vlk_utl.h"
#include "thirdparty/vma/vma.h"
#include "utl/utl.h"
#include "utl/utl_array.h"

#include "autogen/vlk_window.static.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

static void create_descriptors(_vlk_window_t* window, _vlk_dev_t* dev);

static void create_pipelines(_vlk_window_t* window, _vlk_t* vlk);

static void create_surface(_vlk_window_t* window, _vlk_t* vlk);

static void create_swapchain(_vlk_window_t* window, _vlk_t* vlk, uint32_t width, uint32_t height);

static void destroy_descriptors(_vlk_window_t* window);

static void destroy_pipelines(_vlk_window_t* window);

static void destroy_surface(_vlk_window_t* window, _vlk_t* vlk);

static void destroy_swapchain(_vlk_window_t* window, _vlk_t* vlk);

/*=========================================================
CONSTRUCTORS
=========================================================*/

void vlk_window__construct(gpu_window_t* window, gpu_t* gpu, uint32_t width, uint32_t height)
{
	_vlk_t* vlk = _vlk__from_base(window->gpu);

	/* Allocate implementation context */
	window->data = malloc(sizeof(_vlk_window_t));
	_vlk_window_t* vlk_window = _vlk_window__from_base(window);
	vlk_window->base = window;

	create_surface(vlk_window, vlk);
	create_swapchain(vlk_window, vlk, width, height);
	create_pipelines(vlk_window, vlk);
	create_descriptors(vlk_window, &vlk->dev);
}

void vlk_window__destruct(gpu_window_t* window, gpu_t* gpu)
{
	_vlk_t* vlk = _vlk__from_base(window->gpu);
	_vlk_window_t* vlk_window = _vlk_window__from_base(window);

	destroy_descriptors(vlk_window);
	destroy_pipelines(vlk_window);
	destroy_swapchain(vlk_window, vlk);
	destroy_surface(vlk_window, vlk);

	/* Free implementation context */
	free(window->data);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void vlk_window__begin_frame(gpu_window_t* window, gpu_frame_t* frame, kk_camera_t* camera)
{
	_vlk_t* vlk = _vlk__from_base(window->gpu);
	_vlk_window_t* vlk_window = _vlk_window__from_base(window);
	_vlk_frame_t* vlk_frame = _vlk_frame__from_base(frame);
	vlk_frame->frame_idx = frame->frame_idx;

	/* Setup render pass, command buffer, etc. */
	_vlk_swapchain__begin_frame(&vlk_window->swapchain, vlk, vlk_frame);

	/* Setup per-view descriptor set data */
	_vlk_per_view_set__update(&vlk_window->per_view_set, vlk_frame, camera, vlk_window->swapchain.extent);
}

void vlk_window__end_frame(gpu_window_t* window, gpu_frame_t* frame)
{
	_vlk_t* vlk = _vlk__from_base(window->gpu);
	_vlk_window_t* vlk_window = _vlk_window__from_base(window);
	_vlk_frame_t* vlk_frame = _vlk_frame__from_base(frame);

	/* End render pass, submit command buffer, preset swapchain */
	_vlk_swapchain__end_frame(&vlk_window->swapchain, vlk_frame);
}

int vlk_window__get_picker_id(gpu_window_t* window, gpu_frame_t* frame, float x, float y)
{
	_vlk_t* vlk = _vlk__from_base(window->gpu);
	_vlk_window_t* vlk_window = _vlk_window__from_base(window);
	_vlk_frame_t* vlk_frame = _vlk_frame__from_base(frame);
	
	return read_pixel(
		&vlk->dev,
		vlk_window->swapchain.picker_images[vlk_frame->image_idx],
		VK_FORMAT_B8G8R8A8_UNORM,
		vlk_window->swapchain.extent.width,
		vlk_window->swapchain.extent.height,
		x, y);
}

void vlk_window__render_imgui(gpu_window_t* window, gpu_frame_t* frame, ImDrawData* draw_data)
{
	_vlk_t* vlk = _vlk__from_base(window->gpu);
	_vlk_window_t* vlk_window = _vlk_window__from_base(window);
	_vlk_frame_t* vlk_frame = _vlk_frame__from_base(frame);

	/* Draw imgui */
	_vlk_imgui_pipeline__render(&vlk_window->imgui_pipeline, vlk_frame, draw_data);
}

void vlk_window__resize(gpu_window_t* window, uint32_t width, uint32_t height)
{
	_vlk_t* vlk = _vlk__from_base(window->gpu);
	_vlk_window_t* vlk_window = _vlk_window__from_base(window);
	_vlk_swapchain__recreate(&vlk_window->swapchain, width, height);

	// TODO : Dynamic state instead of recreating pipelines
	destroy_pipelines(vlk_window);
	create_pipelines(vlk_window, vlk);
}

_vlk_window_t* _vlk_window__from_base(gpu_window_t* window)
{
	return (_vlk_window_t*)window->data;
}

static void create_descriptors(_vlk_window_t* window, _vlk_dev_t* dev)
{
	_vlk_per_view_set__construct(&window->per_view_set, &dev->per_view_layout);
}

static void create_pipelines(_vlk_window_t* window, _vlk_t* vlk)
{
	_vlk_md5_pipeline__construct(&window->md5_pipeline, &vlk->dev, vlk->dev.render_pass, window->swapchain.extent);
	_vlk_obj_pipeline__construct(&window->obj_pipeline, &vlk->dev, vlk->dev.render_pass, window->swapchain.extent);
	_vlk_plane_pipeline__construct(&window->plane_pipeline, &vlk->dev, vlk->dev.render_pass, window->swapchain.extent);
	_vlk_imgui_pipeline__construct(&window->imgui_pipeline, &vlk->dev, vlk->dev.render_pass, window->swapchain.extent);
	_vlk_picker_pipeline__construct(&window->picker_pipeline, &vlk->dev, vlk->dev.picker_render_pass, window->swapchain.extent);
}

static void create_surface(_vlk_window_t* window, _vlk_t* vlk)
{
	VkResult result = vlk->create_surface_func(window->base->platform_window, vlk->instance, &window->surface);
	if (result != VK_SUCCESS)
	{
		kk_log__fatal("Failed to create Vulkan surface.");
	}

	/* Must check to make sure surface supports presentation */
	uint32_t supported = FALSE;
	result = vkGetPhysicalDeviceSurfaceSupportKHR(vlk->gpu.handle, vlk->dev.present_family_idx, window->surface, &supported);
	if (result != VK_SUCCESS || !supported)
	{
		kk_log__fatal("Surface not supported for this GPU.");
	}
}

static void create_swapchain(_vlk_window_t* window, _vlk_t* vlk, uint32_t width, uint32_t height)
{
	VkExtent2D extent;
	extent.width = width;
	extent.height = height;

	_vlk_swapchain__init(&window->swapchain, &vlk->dev, window->surface, extent);
}

static void destroy_descriptors(_vlk_window_t* window)
{
	_vlk_per_view_set__destruct(&window->per_view_set);
}

static void destroy_pipelines(_vlk_window_t* window)
{
	_vlk_md5_pipeline__destruct(&window->md5_pipeline);
	_vlk_obj_pipeline__destruct(&window->obj_pipeline);
	_vlk_plane_pipeline__destruct(&window->plane_pipeline);
	_vlk_imgui_pipeline__destruct(&window->imgui_pipeline);
	_vlk_picker_pipeline__destruct(&window->picker_pipeline);
}

static void destroy_surface(_vlk_window_t* window, _vlk_t* vlk)
{
	vkDestroySurfaceKHR(vlk->instance, window->surface, NULL);
}

static void destroy_swapchain(_vlk_window_t* window, _vlk_t* vlk)
{
	_vlk_swapchain__term(&window->swapchain);
}

//## static
/**
Gets the pixel color value at the specified coordinate.

https://github.com/SaschaWillems/Vulkan/blob/master/examples/screenshot/screenshot.cpp
*/
static int read_pixel
	(
	_vlk_dev_t*					dev,
	VkImage						src_image,
	VkFormat					src_format,
	uint32_t					width,
	uint32_t					height,
	float						x,
	float						y
	)
{
	boolean supports_blit = TRUE;

	/* Check blit support for source and destination */
	VkFormatProperties format_props;

	/* Check if the device supports blitting from optimal images (the swapchain images are in optimal format) */
	vkGetPhysicalDeviceFormatProperties(dev->gpu->handle, src_format, &format_props);
	if (!(format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT))
	{
		//log__dbg("Device does not support blitting from optimal tiled images, using copy instead of blit.");
		supports_blit = FALSE;
	}

	/* Check if the device supports blitting to linear images */
	vkGetPhysicalDeviceFormatProperties(dev->gpu->handle, VK_FORMAT_R8G8B8A8_UNORM, &format_props);
	if (!(format_props.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT))
	{
		//log__dbg("Device does not support blitting to linear tiled images, using copy instead of blit.");
		supports_blit = FALSE;
	}

	/*
	Create the linear tiled destination image to copy to and to read the memory from.
	Note that vkCmdBlitImage (if supported) will also do format conversions if the swapchain color format would differ.
	*/
	VkImageCreateInfo image_ci;
	clear_struct(&image_ci);
	image_ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_ci.imageType = VK_IMAGE_TYPE_2D;
	image_ci.format = VK_FORMAT_R8G8B8A8_UNORM;
	image_ci.extent.width = width;
	image_ci.extent.height = height;
	image_ci.extent.depth = 1;
	image_ci.arrayLayers = 1;
	image_ci.mipLevels = 1;
	image_ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_ci.samples = VK_SAMPLE_COUNT_1_BIT;
	image_ci.tiling = VK_IMAGE_TILING_LINEAR;
	image_ci.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	VmaAllocationCreateInfo image_alloc_ci;
	clear_struct(&image_alloc_ci);
	image_alloc_ci.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	image_alloc_ci.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;

	/* Create the image */
	VkImage dst_image;
	VmaAllocation dst_image_allocation;
	VmaAllocationInfo dst_image_allocation_info;

	if (VK_SUCCESS != vmaCreateImage(dev->allocator, &image_ci, &image_alloc_ci, &dst_image, &dst_image_allocation, &dst_image_allocation_info))
	{
		kk_log__fatal("Failed to create image.");
	}

	/* Do the actual blit from the swapchain image to our host visible destination image */
	VkCommandBuffer cmd_buf = _vlk_device__begin_one_time_cmd_buf(dev);

	VkImageSubresourceRange subresource_range;
	clear_struct(&subresource_range);
	subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource_range.baseMipLevel = 0;
	subresource_range.levelCount = 1;
	subresource_range.baseArrayLayer = 0;
	subresource_range.layerCount = 1;

	/* Transition destination image to transfer destination layout */
	_vlk_utl__insert_image_memory_barrier(
		cmd_buf,
		dst_image,
		0,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		subresource_range);

	/* Transition swapchain image from present to transfer source layout */
	_vlk_utl__insert_image_memory_barrier(
		cmd_buf,
		src_image,
		VK_ACCESS_MEMORY_READ_BIT,
		VK_ACCESS_TRANSFER_READ_BIT,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		subresource_range);

	/* 
	If source and destination support blit we'll blit as this also does
	automatic format conversion (e.g. from BGR to RGB)
	*/
	if (supports_blit)
	{
		/* Define the region to blit (we will blit the whole swapchain image) */
		VkOffset3D blit_size;
		blit_size.x = width;
		blit_size.y = height;
		blit_size.z = 1;

		VkImageBlit image_blit_region;
		clear_struct(&image_blit_region);
		image_blit_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_blit_region.srcSubresource.layerCount = 1;
		image_blit_region.srcOffsets[1] = blit_size;
		image_blit_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_blit_region.dstSubresource.layerCount = 1;
		image_blit_region.dstOffsets[1] = blit_size;

		/* Issue the blit command */
		vkCmdBlitImage(
			cmd_buf,
			src_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			dst_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&image_blit_region,
			VK_FILTER_NEAREST);
	}
	else
	{
		/* Otherwise use image copy (requires us to manually flip components) */
		VkImageCopy image_copy_region;
		clear_struct(&image_copy_region);
		image_copy_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_copy_region.srcSubresource.layerCount = 1;
		image_copy_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_copy_region.dstSubresource.layerCount = 1;
		image_copy_region.extent.width = width;
		image_copy_region.extent.height = height;
		image_copy_region.extent.depth = 1;

		/* Issue the copy command */
		vkCmdCopyImage(
			cmd_buf,
			src_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			dst_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&image_copy_region);
	}

	/* Transition destination image to general layout, which is the required layout for mapping the image memory later on */
	_vlk_utl__insert_image_memory_barrier(
		cmd_buf,
		dst_image,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_ACCESS_MEMORY_READ_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_GENERAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		subresource_range);

	/* Transition back the swap chain image after the blit is done */
	_vlk_utl__insert_image_memory_barrier(
		cmd_buf,
		src_image,
		VK_ACCESS_TRANSFER_READ_BIT,
		VK_ACCESS_MEMORY_READ_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		subresource_range);

	/* Submit command buffer */
	_vlk_device__end_one_time_cmd_buf(dev, cmd_buf);

	/* Get layout of the image (including row pitch) */
	VkImageSubresource subresource;
	clear_struct(&subresource);
	subresource.arrayLayer = 0;
	subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource.mipLevel = 0;

	VkSubresourceLayout subresource_layout;
	vkGetImageSubresourceLayout(dev->handle, dst_image, &subresource, &subresource_layout);

	/* Map image memory so we can access it */
	const char* data;
	if (VK_SUCCESS != vkMapMemory(dev->handle, dst_image_allocation_info.deviceMemory, dst_image_allocation_info.offset, dst_image_allocation_info.size, 0, (void**)&data))
	{
		kk_log__fatal("Failed to map Vulkan memory.");
	}

	/* 
	If source is BGR (destination is always RGB) and we can't use blit 
	(which does automatic conversion), we'll have to manually swizzle color components
	*/
	bool color_swizzle = false;

	/*
	Check if source is BGR 
	Note: Not complete, only contains most common and basic BGR surface formats for demonstation purposes
	*/
	if (!supports_blit)
	{
		VkFormat formats_bgr[] =
		{
			VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SNORM
		};

		for (int i = 0; i < cnt_of_array(formats_bgr); ++i)
		{
			if (formats_bgr[i] == src_format)
			{
				color_swizzle = TRUE;
				break;
			}
		}
	}

	/* Get the pixel value for the desired coordinate */
	int pixel_size = 4;
	char* pixel_addr = _vlk_utl__get_texel_addr(data, subresource_layout, pixel_size, (int)x, (int)y, 0, 0);

	/* Make sure address is valid */
	if (pixel_addr - data > subresource_layout.size)
	{
		kk_log__error("Pixel address outside bounds of image buffer.");
		return 0xFFFFFFFF;
	}

	/* Get pixel value as integer */
	int pixel_val = *((int*)pixel_addr);

	/* Deal with BGRA/RGBA if needed */
	if (color_swizzle)
	{
		/* Need to convert from BGRA to RGBA */
		kk_vec4_t bgra;
		utl_unpack_rgba_float(pixel_val, bgra);

		/* Swap Blue and Red. Green and Alpha don't change. */
		pixel_val = utl_pack_rgba_float(bgra.z, bgra.y, bgra.x, bgra.w);
	}

	/* Clean up resources */
	vkUnmapMemory(dev->handle, dst_image_allocation_info.deviceMemory);
	vmaDestroyImage(dev->allocator, dst_image, dst_image_allocation);

	return pixel_val;
}
