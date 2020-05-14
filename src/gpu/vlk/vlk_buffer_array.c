/*=========================================================
A buffer array is simply one large Vulkan buffer logically
split into multiple components of the same size. The array
buffer is setup to manage memory alignments and element
updates without having to manually compute offsets, etc.

Uniform buffer offset must meet alignment requirements. 
When referencing a UBO in a descriptor write, Vulkan may
complain about buffer alignment. The buffer array will 
compute any required padding for each element in the buffer.

See: https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#VUID-VkWriteDescriptorSet-descriptorType-00327
=========================================================*/

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

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
_vlk_buffer_array__construct
*/
void _vlk_buffer_array__construct
	(
	_vlk_buffer_array_t*			buffer,
	_vlk_dev_t*						device,
	VkDeviceSize					element_size,
	uint32_t						num_elements,
	VkBufferUsageFlags				buffer_usage,
	VmaMemoryUsage					memory_usage
	)
{
	clear_struct(buffer);
	buffer->element_size = element_size;
	buffer->num_elements = num_elements;

	/*
	Get GPU device properties to determine alignment requirements. Offsets into
	uniform buffers must be a multiple of the specified offset alignment value.
	*/
	VkDeviceSize alignment = device->gpu->device_properties.limits.minUniformBufferOffsetAlignment;
	VkDeviceSize bytes_over = element_size % alignment;

	/* Determine the size of a single element in the UBO with padding */
	buffer->element_size_padded = element_size;
	if (bytes_over > 0)
	{
		buffer->element_size_padded += (alignment - bytes_over);
	}

	/* Determine total size of the UBO */
	VkDeviceSize total_size = buffer->element_size_padded * num_elements;

	/* Initialize the underlying buffer */
	_vlk_buffer__construct(&buffer->buffer, device, total_size, buffer_usage, memory_usage);
}

/**
_vlk_buffer_array__destruct
*/
void _vlk_buffer_array__destruct(_vlk_buffer_array_t* buffer)
{
	_vlk_buffer__destruct(&buffer->buffer);
}

/*=========================================================
FUNCTIONS
=========================================================*/

/**
_vlk_buffer_array__update
*/
void _vlk_buffer_array__update(_vlk_buffer_array_t* buffer, void* data, uint32_t index)
{
	/* Validate inputs */
	if (!buffer)
		kk_log__fatal("Null buffer ptr.");
	if (!data)
		kk_log__fatal("Null data ptr.");
	if (index >= buffer->num_elements)
		kk_log__fatal("Index out of range.");

	/* Compute offset */
	VkDeviceSize offset = buffer->element_size_padded * index;

	/* Update the data in the buffer (used un-padded size here) */
	_vlk_buffer__update(&buffer->buffer, data, offset, buffer->element_size);
}