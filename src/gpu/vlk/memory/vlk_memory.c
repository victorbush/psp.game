/*=========================================================
INCLUDES
=========================================================*/

#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_private.h"
#include "gpu/vlk/memory/vlk_memory.h"
#include "platforms/common.h"
#include "utl/utl_array.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/**
Terminates a memory allocation.
*/
static void term_alloc(_vlk_mem_alloc_t* alloc);

/**
Terminates a memory pool.
*/
static void term_pool(_vlk_mem_pool_t* pool);

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
_vlk_memory__init
*/
void _vlk_memory__init(_vlk_mem_t* mem, _vlk_dev_t* dev)
{
	clear_struct(mem);
	mem->dev = dev;
	utl_array_init(&mem->pools);

	/* 
	Create memory pools for each memory type reported by the GPU
	*/
	uint32_t num_types = dev->gpu->mem_properties.memoryTypeCount;
	utl_array_resize(&mem->pools, num_types);

	for (uint32_t i = 0; i < num_types; ++i)
	{
		_vlk_mem_pool_t* pool = &mem->pools.data[i];
		pool->memory_type = dev->gpu->mem_properties.memoryTypes[i];
		pool->memory_type_idx = i;
	}
}

void _vlk_memory__term(_vlk_mem_t* mem)
{
	for (uint32_t i = 0; i < mem->pools.count; ++i)
	{
		_vlk_mem_pool_t* pool = &mem->pools.data[i];
		term_pool(pool);
	}

	utl_array_destroy(&mem->pools);
}

/*=========================================================
FUNCTIONS
=========================================================*/

/**
_vlk_memory__alloc_buffer
*/
void _vlk_memory__alloc_buffer
	(
	_vlk_mem_t*					mem,
	VkDeviceSize				size,
	VkBufferUsageFlags			usage,
	_vlk_buffer_t*				out__buffer
	)
{
	clear_struct(out__buffer);
	out__buffer->size = size;

	/* Create the buffer */
	VkBufferCreateInfo buffer_info;
	clear_struct(&buffer_info);
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = size;
	buffer_info.usage = usage;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	vkCreateBuffer(mem->dev->handle, &buffer_info, NULL, &out__buffer->handle);

	/* Determine memory requirments */
	VkMemoryRequirements mem_req;
	clear_struct(&mem_req);
	vkGetBufferMemoryRequirements(mem->dev->handle, out__buffer->handle, &mem_req);

	/* Find the memory type index */
	uint32_t mem_idx = _vlk_gpu__find_memory_type_idx(mem->dev->gpu, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	/* Get the memory pool for the memory type */
	_vlk_mem_pool_t* pool = &mem->pools.data[mem_idx];

	/* Allocate memory within the pool */
	alloc_in_pool(pool, out__buffer->size, &out__buffer->alloc, &out__buffer->offset);

	/* Bind the buffer to the memory */
	vkBindBufferMemory(mem->dev->handle, out__buffer->handle, out__buffer->alloc->handle, out__buffer->offset);
}

void _vlk_memory__free_buffer(_vlk_buffer_t* buffer)
{

}

static void alloc_in_pool
	(
	_vlk_mem_pool_t*			pool,
	VkDeviceSize				size,
	VkDeviceSize				alignment,
	_vlk_mem_alloc_t**			out__alloc,
	VkDeviceSize				out__offset
	)
{
	out__alloc = NULL;
	out__offset = 0;

	/* Go through allocations in pool */
	for (uint32_t i = 0; i < pool->allocations.count; ++i)
	{
		_vlk_mem_alloc_t* alloc = &pool->allocations.data[i];

		if (alloc->size < size)
		{
			/* Allocation too small, skip */
			continue;
		}

		/* Compute offset based on alignment requirements */
		uint32_t offset = alloc->next_free;
		uint32_t bytes_over = offset % alignment;
		if (bytes_over > 0)
		{
			offset += (alignment - bytes_over);
		}

		if (alloc->size - offset < size)
		{
			/* No more room in this allocation */
			continue;
		}

		/* Found space */
		out__alloc = alloc;
		out__offset = offset;
		alloc->next_free = offset + size;
		return;
	}

	/* No free space found, create a new allocation */
	VkDeviceSize alloc_size = max(MIN_ALLOC_SIZE, size);
	utl_array_resize(&pool->allocations, pool->allocations.count + 1);
	out__alloc = &pool->allocations.data[pool->allocations.count - 1];
	init_alloc(out__alloc, alloc_size, , );
}

static void init_alloc
	(
	_vlk_mem_alloc_t*			alloc,
	VkDeviceSize				size,
	uint32_t					memory_type_idx,
	_vlk_dev_t*					dev
	)
{
	clear_struct(alloc);
	alloc->dev = dev;
	alloc->next_free = 0;
	alloc->size = size;

	VkMemoryAllocateInfo info;
	clear_struct(&info);
	info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	info.memoryTypeIndex = memory_type_idx;
	info.allocationSize = size;
	vkAllocateMemory(dev, &info, NULL, &alloc->handle);
}

/**
term_alloc
*/
static void term_alloc(_vlk_mem_alloc_t* alloc)
{
	vkUnmapMemory(alloc->dev->handle, alloc->handle);
	vkFreeMemory(alloc->dev, alloc->handle, NULL);
	utl_array_destroy(&alloc->free_chunks);
}

/**
term_pool
*/
static void term_pool(_vlk_mem_pool_t* pool)
{
	for (uint32_t i = 0; i < pool->allocations.count; ++i)
	{
		_vlk_mem_alloc_t* alloc = &pool->allocations.data[i];
		term_alloc(alloc);
	}

	utl_array_destroy(&pool->allocations);
}
