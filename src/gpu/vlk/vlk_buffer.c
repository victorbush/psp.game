/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_log.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "thirdparty/vma/vma.h"
#include "utl/utl_array.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Create the buffer */
static void create_buffer(_vlk_buffer_t* buffer);

/** Maps host memory directly to the buffer and updates the data */
static void update_direct
	(
	_vlk_buffer_t*			buffer,
	void*					data,
	VkDeviceSize			offset,
	VkDeviceSize			data_size
	);

/** Updates a GPU memory buffer using a stagning buffer */
static void update_via_staging_buffer
	(
	_vlk_buffer_t*			buffer,
	void*					data,
	VkDeviceSize			offset,
	VkDeviceSize			data_size
	);

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
_vlk_buffer__create
*/
void _vlk_buffer__construct
	(
	_vlk_buffer_t*					buffer,
	_vlk_dev_t*						device,
	VkDeviceSize					size,
	VkBufferUsageFlags				buffer_usage,
	VmaMemoryUsage					memory_usage
	)
{
	clear_struct(buffer);
	buffer->dev = device;
	buffer->size = size;
	buffer->buffer_usage = buffer_usage;
	buffer->memory_usage = memory_usage;

	create_buffer(buffer);
}

/**
_vlk_buffer__destroy
*/
void _vlk_buffer__destruct(_vlk_buffer_t* buffer)
{
	vmaDestroyBuffer(buffer->dev->allocator, buffer->handle, buffer->allocation);
}

/*=========================================================
FUNCTIONS
=========================================================*/

/**
_vlk_buffer__get_buffer_info
*/
VkDescriptorBufferInfo _vlk_buffer__get_buffer_info(_vlk_buffer_t* buffer)
{
	VkDescriptorBufferInfo info;
	clear_struct(&info);
	info.buffer = buffer->handle;
	info.offset = 0;
	info.range = buffer->size;
	return info;
}

/**
_vlk_buffer__update
*/
void _vlk_buffer__update(_vlk_buffer_t* buffer, void* data, VkDeviceSize offset, VkDeviceSize size)
{
	switch (buffer->memory_usage)
	{
	case VMA_MEMORY_USAGE_GPU_ONLY:
		update_via_staging_buffer(buffer, data, offset, size);
		break;

	default:
		update_direct(buffer, data, offset, size);
		break;
	}
}

/**
create_buffer
*/
static void create_buffer(_vlk_buffer_t* buffer)
{
	if (buffer->memory_usage & VMA_MEMORY_USAGE_GPU_ONLY)
	{
		/* Force transfer destination since will be using with staging buffer */
		buffer->buffer_usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}

	VkBufferCreateInfo info;
	clear_struct(&info);
	info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	info.size = buffer->size;
	info.usage = buffer->buffer_usage;
	info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo alloc_info;
	clear_struct(&alloc_info);
	alloc_info.usage = buffer->memory_usage;

	VkResult result = vmaCreateBuffer(buffer->dev->allocator, &info, &alloc_info, &buffer->handle, &buffer->allocation, NULL);
	if (result != VK_SUCCESS) 
	{
		kk_log__fatal("Failed to create buffer.");
	}
}

/**
update_direct
*/
static void update_direct
	(
	_vlk_buffer_t*			buffer,
	void*					data,
	VkDeviceSize			offset,
	VkDeviceSize			data_size
	)
{
	void* buf;
	VkResult result = vmaMapMemory(buffer->dev->allocator, buffer->allocation, &buf);
	if (result != VK_SUCCESS)
	{
		kk_log__fatal("Unable to map Vulkan memory.");
	}

	buf = (char*)buf + offset;
	memcpy(buf, data, data_size);
	vmaUnmapMemory(buffer->dev->allocator, buffer->allocation);
}

/**
update_via_staging_buffer
*/
static void update_via_staging_buffer
	(
	_vlk_buffer_t*			buffer,
	void*					data, 
	VkDeviceSize			offset, 
	VkDeviceSize			data_size
	)
{
	/*
	Create staging buffer
	*/
	_vlk_buffer_t staging_buffer;
	_vlk_buffer__construct(
		&staging_buffer, 
		buffer->dev, 
		data_size, 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VMA_MEMORY_USAGE_CPU_ONLY);

	/*
	Load data to staging buffer
	*/
	_vlk_buffer__update(&staging_buffer, data, 0, data_size);

	/*
	Copy staging -> GPU
	*/
	VkCommandBuffer cmd_buf = _vlk_device__begin_one_time_cmd_buf(buffer->dev);

	VkBufferCopy copy_region;
	clear_struct(&copy_region);
	copy_region.size = data_size;
	copy_region.dstOffset = offset;
	copy_region.srcOffset = 0;
	vkCmdCopyBuffer(cmd_buf, staging_buffer.handle, buffer->handle, 1, &copy_region);

	_vlk_device__end_one_time_cmd_buf(buffer->dev, cmd_buf);

	/*
	Free staging buffer
	*/
	_vlk_buffer__destruct(&staging_buffer);
}