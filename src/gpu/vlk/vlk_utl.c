/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "gpu/vlk/vlk_utl.h"
#include "log/log.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

//## internal
/**
https://github.com/SaschaWillems/Vulkan
*/
void _vlk_utl__insert_image_memory_barrier
	(
	VkCommandBuffer			cmd_buf, 
	VkImage					image,
	VkAccessFlags			src_access_mask,
	VkAccessFlags			dst_access_mask,
	VkImageLayout			old_img_layout,
	VkImageLayout			new_img_layout,
	VkPipelineStageFlags	src_stage_mask,
	VkPipelineStageFlags	dst_stage_mask,
	VkImageSubresourceRange subresource_range
	)
{
	VkImageMemoryBarrier image_mem_barrier;
	clear_struct(&image_mem_barrier);
	image_mem_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_mem_barrier.srcAccessMask = src_access_mask;
	image_mem_barrier.dstAccessMask = dst_access_mask;
	image_mem_barrier.oldLayout = old_img_layout;
	image_mem_barrier.newLayout = new_img_layout;
	image_mem_barrier.image = image;
	image_mem_barrier.subresourceRange = subresource_range;

	vkCmdPipelineBarrier(
		cmd_buf,
		src_stage_mask,
		dst_stage_mask,
		0,
		0, NULL,
		0, NULL,
		1, &image_mem_barrier);
}

//## internal
/**
Creates a image memory barrier for changing the layout of an image and puts it into the
specified command buffer.

https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanTools.cpp
*/
void _vlk_utl__set_image_layout
	(
	VkCommandBuffer			cmd_buf,
	VkImage					image,
	VkImageLayout			old_img_layout,
	VkImageLayout			new_img_layout,
	VkImageSubresourceRange subresource_range,
	VkPipelineStageFlags	src_stage_mask,
	VkPipelineStageFlags	dst_stage_mask
	)
{
	/* Create an image barrier object */
	VkImageMemoryBarrier image_memory_barrier;
	clear_struct(&image_memory_barrier);
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.oldLayout = old_img_layout;
	image_memory_barrier.newLayout = new_img_layout;
	image_memory_barrier.image = image;
	image_memory_barrier.subresourceRange = subresource_range;

	/*
	Source layouts (old)
	Source access mask controls actions that have to be finished on the old layout
	before it will be transitioned to the new layout.
	*/
	switch (old_img_layout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		/*
		Image layout is undefined (or does not matter)
		Only valid as initial layout
		No flags required, listed only for completeness
		*/
		image_memory_barrier.srcAccessMask = 0;
		break;

	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		/*
		Image is preinitialized
		Only valid as initial layout for linear images, preserves memory contents
		Make sure host writes have been finished
		*/
		image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		/*
		Image is a color attachment
		Make sure any writes to the color buffer have been finished
		*/
		image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		/*
		Image is a depth/stencil attachment
		Make sure any writes to the depth/stencil buffer have been finished
		*/
		image_memory_barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		/*
		Image is a transfer source 
		Make sure any reads from the image have been finished
		*/
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		/*
		Image is a transfer destination
		Make sure any writes to the image have been finished
		*/
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		/*
		Image is read by a shader
		Make sure any shader reads from the image have been finished
		*/
		image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;

	default:
		/* Other source layouts aren't handled (yet) */
		log__fatal("Unknown image type for transition.");
		break;
	}

	/*
	Target layouts (new)
	Destination access mask controls the dependency for the new image layout
	*/
	switch (new_img_layout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		/*
		Image will be used as a transfer destination
		Make sure any writes to the image have been finished
		*/
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		/*
		Image will be used as a transfer source
		Make sure any reads from the image have been finished
		*/
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		/*
		Image will be used as a color attachment
		Make sure any writes to the color buffer have been finished
		*/
		image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		/*
		Image layout will be used as a depth/stencil attachment
		Make sure any writes to depth/stencil buffer have been finished
		*/
		image_memory_barrier.dstAccessMask = image_memory_barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		/*
		Image will be read in a shader (sampler, input attachment)
		Make sure any writes to the image have been finished
		*/
		if (image_memory_barrier.srcAccessMask == 0)
		{
			image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;

	default:
		/* Other source layouts aren't handled (yet) */
		log__fatal("Unknown image type for transition.");
		break;
	}

	/* Put barrier inside command buffer */
	vkCmdPipelineBarrier(
		cmd_buf,
		src_stage_mask,
		dst_stage_mask,
		0,
		0, NULL,
		0, NULL,
		1, &image_memory_barrier);
}

/*=========================================================
STATIC FUNCTIONS
=========================================================*/
