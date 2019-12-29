/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "thirdparty/vma/vma.h"
#include "utl/utl_log.h"

/*=========================================================
VARIABLES
=========================================================*/

/* A plane is a rectangle so 6 indices (2 triangles * 3 verts) */
static const uint32_t NUM_INDICES = 6;

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
	vkCmdDrawIndexed(cmd, NUM_INDICES, 1, 0, 0, 0);
}

void _vlk_plane__set_verts(_vlk_plane_t* plane, const vec3_t verts[4])
{

}

static void create_buffer(_vlk_plane_t* plane, _vlk_dev_t* dev)
{
	VkDeviceSize index_array_size = sizeof(uint16_t) * NUM_INDICES;

	/* Create temp index array for staging */
	uint16_t* index_array = malloc(index_array_size);
	if (!index_array)
	{
		FATAL("Failed to allocate memory for mesh indices.");
	}

	index_array[0] = 0;
	index_array[1] = 1;
	index_array[2] = 3;
	index_array[3] = 1;
	index_array[4] = 2;
	index_array[5] = 3;

	/* Create index buffer and load data to GPU */
	_vlk_buffer__construct(&plane->index_buffer, dev, index_array_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	_vlk_buffer__update(&plane->index_buffer, index_array, 0, index_array_size);

	/* Create vertex buffer */
	VkDeviceSize vert_array_size = sizeof(uint16_t) * NUM_INDICES;
	_vlk_buffer__construct(&plane->vertex_buffer, dev, vert_array_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	//_vlk_buffer__update(&mesh->vertex_buffer, vert_array, 0, vert_array_size);




	/* For now going wasteful and not eliminating duplicate vertex data */
	int num_faces = obj_shape->length;
	int num_verts = num_faces * 3;

	/* Allocate a temp vertex buffer to send to the GPU */
	VkDeviceSize vert_array_size = sizeof(_vlk_static_mesh_vertex_t) * num_verts;
	_vlk_static_mesh_vertex_t* vert_array = malloc(vert_array_size);
	if (!vert_array)
	{
		FATAL("Failed to allocate memory for mesh vertices.");
	}

	/* Allocate a temp index buffer to send to the GPU */
	mesh->num_indices = num_faces * 3;
	VkDeviceSize index_array_size = sizeof(uint16_t) * mesh->num_indices;
	uint16_t* index_array = malloc(index_array_size);
	if (!index_array)
	{
		FATAL("Failed to allocate memory for mesh indices.");
	}

	/* Go through faces and determine the number of vertices we actually need */
	int first_face_idx = obj_shape->face_offset;
	int last_face_idx = first_face_idx + obj_shape->length;
	uint16_t index = 0;

	/* Populate buffers with data */
	for (int i = first_face_idx; i < last_face_idx; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			int v_idx = obj->attrib.faces[i * 3 + j].v_idx;
			int vt_idx = obj->attrib.faces[i * 3 + j].vt_idx;
			int vn_idx = obj->attrib.faces[i * 3 + j].vn_idx;

			vert_array[index].pos.x = obj->attrib.vertices[v_idx * 3 + 0];
			vert_array[index].pos.y = obj->attrib.vertices[v_idx * 3 + 1];
			vert_array[index].pos.z = obj->attrib.vertices[v_idx * 3 + 2];
							
			vert_array[index].normal.x = obj->attrib.normals[vn_idx * 3 + 0];
			vert_array[index].normal.y = obj->attrib.normals[vn_idx * 3 + 1];
			vert_array[index].normal.z = obj->attrib.normals[vn_idx * 3 + 2];
							
			vert_array[index].tex.x = obj->attrib.texcoords[vt_idx * 2 + 0];
			vert_array[index].tex.y = obj->attrib.texcoords[vt_idx * 2 + 1];

			/* Index buffer is superfulous at this point until duplicate vertices handled */
			index_array[index] = index;
			index++;
		}
	}

	/* Create index buffer and load data to GPU */
	_vlk_buffer__construct(&mesh->index_buffer, dev, index_array_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	_vlk_buffer__update(&mesh->index_buffer, index_array, 0, index_array_size);

	/* Create vertex buffer and load data to GPU */
	_vlk_buffer__construct(&mesh->vertex_buffer, dev, vert_array_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	_vlk_buffer__update(&mesh->vertex_buffer, vert_array, 0, vert_array_size);

	/* Free the temp arrays */
	free(vert_array);
	free(index_array);
}

static void destroy_buffers(_vlk_static_mesh_t* mesh)
{
	_vlk_buffer__destruct(&mesh->index_buffer);
	_vlk_buffer__destruct(&mesh->vertex_buffer);
}
