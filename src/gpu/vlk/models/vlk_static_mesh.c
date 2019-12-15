/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"
#include "thirdparty/vma/vma.h"
#include "thirdparty/tinyobj/tinyobj.h"
#include "utl/utl_log.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/** Create buffers for the meshes. */
static void create_buffers(_vlk_static_mesh_t* mesh, _vlk_dev_t* dev, const tinyobj_t* obj, const tinyobj_shape_t* obj_shape);

/** Destroys the mesh buffers. */
static void destroy_buffers(_vlk_static_mesh_t* mesh);

/*=========================================================
CONSTRUCTORS
=========================================================*/

void _vlk_static_mesh__construct
	(
	_vlk_static_mesh_t*			mesh,
	_vlk_dev_t*					device,
	const tinyobj_t*			obj,
	const tinyobj_shape_t*		obj_shape
	)
{
	clear_struct(mesh);

	create_buffers(mesh, device, obj, obj_shape);
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
	const tinyobj_t*			obj,
	const tinyobj_shape_t*		obj_shape
	)
{
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
	for (int i = first_face_idx; i <= last_face_idx; ++i)
	{
		int v_idx = obj->attrib.faces[i].v_idx;
		int vt_idx = obj->attrib.faces[i].vt_idx;
		int vn_idx = obj->attrib.faces[i].vn_idx;

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
