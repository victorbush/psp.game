/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "thirdparty/vma/vma.h"
#include "thirdparty/tinyobj/tinyobj_loader_c.h"
#include "utl/utl_log.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Create buffers for the meshes. */
static void create_buffers(_vlk_static_mesh_t* mesh, _vlk_dev_t* dev, const tinyobj_attrib_t* obj);

/** Destroys the mesh buffers. */
static void destroy_buffers(_vlk_static_mesh_t* mesh);

/*=========================================================
CONSTRUCTORS
=========================================================*/

void _vlk_static_mesh__construct
	(
	_vlk_static_mesh_t*			mesh,
	_vlk_dev_t*					device,
	const tinyobj_attrib_t*		obj
	)
{
	clear_struct(mesh);

	create_buffers(mesh, device, obj);
}

void _vlk_static_mesh__destruct(_vlk_static_mesh_t* mesh)
{
	destroy_buffers(mesh);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void _vlk_static_mesh__render
	(
	_vlk_static_mesh_t*			mesh,
	VkCommandBuffer				cmd
	)
{
	VkBuffer vertBufs[] = { mesh->vertex_buffer.handle };
	VkDeviceSize vertBufOffsets[] = { 0 };

	vkCmdBindVertexBuffers(cmd, 0, 1, vertBufs, vertBufOffsets);
	vkCmdBindIndexBuffer(cmd, mesh->index_buffer.handle, 0, VK_INDEX_TYPE_UINT16);
	vkCmdDrawIndexed(cmd, mesh->num_indices, 1, 0, 0, 0);
}

static void create_buffers
	(
	_vlk_static_mesh_t*			mesh,
	_vlk_dev_t*					dev,
	const tinyobj_attrib_t*		obj
	)
{
	/*
	Index buffer
	*/

	/* Allocate temp array of indices that will be transferred to GPU */
	mesh->num_indices = obj->num_faces * 3;
	VkDeviceSize index_array_size = sizeof(uint16_t) * mesh->num_indices;
	uint16_t* index_array = malloc(index_array_size);
	if (!index_array)
	{
		FATAL("Failed to allocate memory for mesh indices.");
	}

	/* Load indcies */
	for (int i = 0; i < obj->num_faces; ++i)
	{
		index_array[i] = obj->faces[i].v_idx;
	}

	/* Create buffer and load data to GPU */
	_vlk_buffer__construct(&mesh->index_buffer, dev, index_array_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	_vlk_buffer__update(&mesh->index_buffer, index_array, 0, index_array_size);

	/* Free the temp array */
	free(index_array);

	/*
	Vertex buffer
	*/

	/* Allocate a temp array of vertices that will be transferred to the GPU */
	VkDeviceSize vert_array_size = sizeof(_vlk_static_mesh_vertex_t) * obj->num_vertices;
	_vlk_static_mesh_vertex_t* vert_array = malloc(vert_array_size);
	if (!vert_array)
	{
		FATAL("Failed to allocate memory for mesh vertices.");
	}

	/* Setup each vertex in the mesh */
	for (int i = 0; i < obj->num_vertices; ++i)
	{
		vert_array[i].pos.x = obj->vertices[i * 3 + 0];
		vert_array[i].pos.y = obj->vertices[i * 3 + 1];
		vert_array[i].pos.z = obj->vertices[i * 3 + 2];

		vert_array[i].normal.x = obj->normals[i * 3 + 0];
		vert_array[i].normal.y = obj->normals[i * 3 + 1];
		vert_array[i].normal.z = obj->normals[i * 3 + 2];

		//vert_array[i].tex.x = obj->texcoords[i * 2 + 0];
		//vert_array[i].tex.y = obj->texcoords[i * 2 + 1];
	}

	/* Create buffer and load data to GPU */
	_vlk_buffer__construct(&mesh->vertex_buffer, dev, vert_array_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	_vlk_buffer__update(&mesh->vertex_buffer, vert_array, 0, vert_array_size);

	/* Free the temp array */
	free(vert_array);
}

static void destroy_buffers(_vlk_static_mesh_t* mesh)
{
	_vlk_buffer__destruct(&mesh->index_buffer);
	_vlk_buffer__destruct(&mesh->vertex_buffer);
}
