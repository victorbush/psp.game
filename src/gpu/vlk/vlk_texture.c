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
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Creates the texture image. */
static void create_image(_vlk_texture_t* tex, const _vlk_texture_create_info_t* create_info);

/** Creates the texture image view. */
static void create_image_view(_vlk_texture_t* tex);

/** Initializes the image info data for the texture. */
static void init_image_info(_vlk_texture_t* tex);

/** Destroys the texture image. */
static void destroy_image(_vlk_texture_t* tex);

/** Destroys the texture image view. */
static void destroy_image_view(_vlk_texture_t* tex);

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
_vlk_texture__construct
*/
void _vlk_texture__construct
	(
	_vlk_texture_t*				tex,
	_vlk_dev_t*					device,
	const _vlk_texture_create_info_t* create_info
	)
{
	clear_struct(tex);
	tex->dev = device;

	create_image(tex, create_info);
	create_image_view(tex);
	init_image_info(tex);
}

/**
_vlk_texture__destruct
*/
void _vlk_texture__destruct(_vlk_texture_t* tex)
{
	destroy_image_view(tex);
	destroy_image(tex);
}

/*=========================================================
FUNCTIONS
=========================================================*/

static void create_image(_vlk_texture_t* tex, const _vlk_texture_create_info_t* create_info)
{
	/*
	Create staging buffer with texture data
	*/
	_vlk_buffer_t staging_buffer;
	_vlk_buffer__construct(
		&staging_buffer,
		tex->dev,
		create_info->size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VMA_MEMORY_USAGE_CPU_ONLY);

	_vlk_buffer__update(&staging_buffer, create_info->data, 0, create_info->size);

	/*
	Create the image
	*/
	VkImageCreateInfo image_info;
	clear_struct(&image_info);
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = create_info->width;
	image_info.extent.height = create_info->height;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;

	// TODO : assuming the format for now
	tex->format = VK_FORMAT_R8G8B8A8_UNORM;

	image_info.format = tex->format;
	image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_info.flags = 0;

	VmaAllocationCreateInfo alloc_info;
	clear_struct(&alloc_info);
	alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	alloc_info.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

	VkResult result = vmaCreateImage(tex->dev->allocator, &image_info, &alloc_info, &tex->image, &tex->image_allocation, NULL);
	if (result != VK_SUCCESS) 
	{
		FATAL("Failed to create texture image.");
	}

	/*
	Copy texture from staging buffer to GPU
	*/
	_vlk_device__transition_image_layout(tex->dev, tex->image, tex->format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	_vlk_device__copy_buffer_to_img_now(tex->dev, &staging_buffer, tex->image, create_info->width, create_info->height);
	_vlk_device__transition_image_layout(tex->dev, tex->image, tex->format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void create_image_view(_vlk_texture_t* tex)
{
	VkImageViewCreateInfo view_info;
	clear_struct(&view_info);
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = tex->image;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = tex->format;
	view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;

	if (vkCreateImageView(tex->dev->handle, &view_info, NULL, &tex->image_view) != VK_SUCCESS) 
	{
		FATAL("Failed to create texture image view.");
	}
}

void init_image_info(_vlk_texture_t* tex)
{
	tex->image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	tex->image_info.imageView = tex->image_view;
	tex->image_info.sampler = tex->dev->texture_sampler;
}

void destroy_image(_vlk_texture_t* tex)
{
	vmaDestroyImage(tex->dev->allocator, tex->image, tex->image_allocation);
}

void destroy_image_view(_vlk_texture_t* tex)
{
	vkDestroyImageView(tex->dev->handle, tex->image_view, NULL);
}
