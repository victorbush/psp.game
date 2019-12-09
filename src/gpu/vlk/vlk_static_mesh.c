/*=========================================================
MD5 static mesh. Uses code from:
http://tfc.duke.free.fr/coding/src/md5mesh.c
=========================================================*/

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/gpu_model.h"
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

/** Create buffers for the meshes. */
static void create_buffers(_vlk_static_mesh_t* mesh, _vlk_dev_t* dev);

/** Destroys the mesh buffers. */
static void destroy_buffers(_vlk_static_mesh_t* mesh);

/*=========================================================
CONSTRUCTORS
=========================================================*/

void _vlk_static_mesh__construct
(
	_vlk_static_mesh_t*			mesh,
	_vlk_dev_t*					device,
	md5_mesh_t*					md5
	)
{
	clear_struct(mesh);
	mesh->md5 = md5;

	create_buffers(mesh, device);
}

void _vlk_static_mesh__destruct(_vlk_static_mesh_t* mesh)
{
	destroy_buffers(mesh);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void _vlk_static_mesh__prepare
	(
	_vlk_static_mesh_t*			mesh,
	md5_joint_t*				md5_skeleton
	)
{
	/* Allocate a temp array of vertices that will be transferred to the GPU */
	VkDeviceSize vert_array_size = sizeof(_vlk_static_mesh_vertex_t) * mesh->md5->num_verts;
	_vlk_static_mesh_vertex_t* vert_array = malloc(vert_array_size);
	if (!vert_array)
	{
		FATAL("Failed to allocate memory for mesh vertices.");
	}

	/* Setup each vertex in the mesh */
	for (int i = 0; i < mesh->md5->num_verts; ++i)
	{
		_vlk_static_mesh_vertex_t* vert = &vert_array[i];
		clear_struct(vert);

		/* Calculate final vertex to draw with weights */
		for (int j = 0; j < mesh->md5->vertices[i].count; ++j)
		{
			const md5_weight_t* weight = &mesh->md5->weights[mesh->md5->vertices[i].start + j];
			const md5_joint_t* joint = &md5_skeleton[weight->joint];

			/* Calculate transformed vertex for this weight */
			vec3_t wv;
			Quat_rotatePoint(joint->orient, weight->pos, &wv);

			/* The sum of all weight->bias should be 1.0 */
			vert->pos.x += (joint->pos[0] + wv.x) * weight->bias;
			vert->pos.y += (joint->pos[1] + wv.y) * weight->bias;
			vert->pos.z += (joint->pos[2] + wv.z) * weight->bias;
		}
	}

	/* Update the GPU buffer */
	_vlk_buffer__update(&mesh->vertex_buffer, vert_array, 0, vert_array_size);

	/* Free the temp array */
	free(vert_array);
}

void _vlk_static_mesh__render
	(
	_vlk_static_mesh_t*			mesh,
	VkCommandBuffer				cmd,
	const transform_comp_t*		transform
	)
{
	VkBuffer vertBufs[] = { mesh->vertex_buffer.handle };
	VkDeviceSize vertBufOffsets[] = { 0 };
	
	// TODO : Model matrix transform

	vkCmdBindVertexBuffers(cmd, 0, 1, vertBufs, vertBufOffsets);
	vkCmdBindIndexBuffer(cmd, mesh->index_buffer.handle, 0, VK_INDEX_TYPE_UINT16);
	vkCmdDrawIndexed(cmd, mesh->num_indices, 1, 0, 0, 0);
}

static void create_buffers(_vlk_static_mesh_t* mesh, _vlk_dev_t* dev)
{
	/*
	Indices
	*/
	mesh->num_indices = mesh->md5->num_tris * 3;
	uint32_t index_data_size = sizeof(uint32_t) * mesh->num_indices;

	/* Allocate temp array of indices to be copied to GPU */
	vec3i_t* index_data = malloc(index_data_size * mesh->md5->num_tris);
	if (!index_data)
	{
		FATAL("Failed to allocate memory for mesh indices.");
	}

	/* Build list of indices */
	for (int i = 0; i < mesh->md5->num_verts; ++i)
	{
		index_data[i].x = mesh->md5->triangles[i].index[0];
		index_data[i].y = mesh->md5->triangles[i].index[1];
		index_data[i].z = mesh->md5->triangles[i].index[2];
	}

	/* Create the index buffer and update it */
	_vlk_buffer__construct(&mesh->index_buffer, dev, index_data_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	_vlk_buffer__update(&mesh->index_buffer, index_data, 0, index_data_size);

	/* Free temporary array */
	free(index_data);

	/*
	Vertices
	*/
	VkDeviceSize vertex_data_size = sizeof(_vlk_static_mesh_vertex_t);
	_vlk_buffer__construct(&mesh->vertex_buffer, dev, vertex_data_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
}

void destroy_buffers(_vlk_static_mesh_t* mesh)
{
	_vlk_buffer__destruct(&mesh->index_buffer);
	_vlk_buffer__destruct(&mesh->vertex_buffer);
}
