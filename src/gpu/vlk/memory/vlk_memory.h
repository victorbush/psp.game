#ifndef VLK_MEMORY_H
#define VLK_MEMORY_H

/*=========================================================
INCLUDES
=========================================================*/

#include <vulkan/vulkan.h>

#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_private.h"
#include "utl/utl_array.h"

/*=========================================================
CONSTANTS
=========================================================*/

/**
The minimum size to create each memory allocation using 
*/
#define MIN_ALLOC_SIZE (1024 * 1024 * 256) // ????

/*=========================================================
TYPES
=========================================================*/

/**
A memory allocation. This is a large chunk of VkDeviceMemory that has
been allocated. It is sub-allocated to different resources (like buffers and images).
*/
typedef struct
{
	_vlk_dev_t*						dev;					/* logical device */
	VkDeviceSize					size;
	VkDeviceMemory					handle;
	VkDeviceSize					next_free;

} _vlk_mem_alloc_t;
utl_array_declare_type(_vlk_mem_alloc_t);

/**
A pool of memory allocations. A pool can only be used for one memory type.
Multiple pools are created per context (one pool for each memory type).
*/
typedef struct
{
	VkMemoryType					memory_type;		/* the memory type this pool is for */
	uint32_t						memory_type_idx;	/* the index of the memory type in the array returned by vkGetPhysicalDeviceMemoryProperties */
	utl_array_t(_vlk_mem_alloc_t)	allocations;		/* the memory allocations for this pool */

} _vlk_mem_pool_t;
utl_array_declare_type(_vlk_mem_pool_t);

typedef struct
{
	_vlk_mem_alloc_t*				alloc;			/* the allocation this buffer is in */
	VkBuffer						handle;			/* the buffer handle */
	VkDeviceSize					size;			/* the size of the buffer */
	VkDeviceSize					offset;			/* the offset into the allocation this buffer starts at */

} _vlk_buffer_t;

/**
Memmory context.
*/
typedef struct
{
	_vlk_dev_t*						dev;			/* device to use */
	utl_array_t(_vlk_mem_pool_t)	pools;			/* memory pools for each memory type */

} _vlk_mem_t;

/*=========================================================
FUNCTIONS
=========================================================*/

/*-------------------------------------
vlk_allocations.c
-------------------------------------*/


#endif /* VLK_MEMORY_H */