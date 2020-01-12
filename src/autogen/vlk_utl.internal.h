
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
;

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
;
