/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_log.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "thirdparty/vma/vma.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Create a buffer for the plane. */
static void create_buffer(_vlk_plane_t* plane, _vlk_dev_t* dev);

/** Destroys the plane buffer. */
static void destroy_buffer(_vlk_plane_t* plane);

/*=========================================================
CONSTRUCTORS
=========================================================*/

void _vlk_plane__construct
	(
	_vlk_plane_t*				plane,
	_vlk_dev_t*					device
	)
{
	clear_struct(plane);

	create_buffer(plane, device);
}

void _vlk_plane__destruct(_vlk_plane_t* plane)
{
	destroy_buffer(plane);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void _vlk_plane__render
	(
	_vlk_plane_t*				plane,
	VkCommandBuffer				cmd
	)
{
	VkBuffer vertBufs[] = { plane->vertex_buffer.handle };
	VkDeviceSize vertBufOffsets[] = { 0 };

	vkCmdBindVertexBuffers(cmd, 0, 1, vertBufs, vertBufOffsets);
	vkCmdBindIndexBuffer(cmd, plane->index_buffer.handle, 0, VK_INDEX_TYPE_UINT16);
	vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);
}

void _vlk_plane__update_verts(_vlk_plane_t* plane, const kk_vec3_t verts[4])
{
	/* Create temp buffer for vert data */
	VkDeviceSize vert_data_size = sizeof(_vlk_plane_vertex_t) * 4;
	_vlk_plane_vertex_t* vert_data = malloc(vert_data_size);
	if (!vert_data)
	{
		kk_log__fatal("Failed to allocate memory.");
	}

	vert_data[0].pos = verts[0];
	vert_data[0].tex_coord.x = 0.0f;
	vert_data[0].tex_coord.y = 0.0f;

	vert_data[1].pos = verts[1];
	vert_data[1].tex_coord.x = 1.0f;
	vert_data[1].tex_coord.y = 0.0f;

	vert_data[2].pos = verts[2];
	vert_data[2].tex_coord.x = 1.0f;
	vert_data[2].tex_coord.y = 1.0f;

	vert_data[3].pos = verts[3];
	vert_data[3].tex_coord.x = 0.0f;
	vert_data[3].tex_coord.y = 1.0f;

	_vlk_buffer__update(&plane->vertex_buffer, (void*)vert_data, 0, vert_data_size);

	/* Free temp buffer */
	free(vert_data);
}

static void create_buffer(_vlk_plane_t* plane, _vlk_dev_t* dev)
{
	uint16_t index_array[6] =
	{
		0, 1, 3,
		1, 2, 3
	};

	/* Create index buffer and load data to GPU */
	_vlk_buffer__construct(&plane->index_buffer, dev, sizeof(index_array), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	_vlk_buffer__update(&plane->index_buffer, index_array, 0, sizeof(index_array));

	/* Create vertex buffer */
	VkDeviceSize vert_array_size = sizeof(_vlk_plane_vertex_t) * 4;
	_vlk_buffer__construct(&plane->vertex_buffer, dev, vert_array_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
}

static void destroy_buffer(_vlk_plane_t* plane)
{
	_vlk_buffer__destruct(&plane->index_buffer);
	_vlk_buffer__destruct(&plane->vertex_buffer);
}
