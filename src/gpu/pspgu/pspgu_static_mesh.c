/*=========================================================
INCLUDES
=========================================================*/

#include <pspgu.h>
#include <pspgum.h>

#include "common.h"
#include "engine/kk_log.h"
#include "gpu/pspgu/pspgu.h"
#include "thirdparty/tinyobj/tinyobj.h"

#include "autogen/pspgu_static_mesh.static.h"

/*=========================================================
MACROS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

//## internal
void _pspgu_static_mesh__construct
	(
	_pspgu_static_mesh_t*		mesh,
	_pspgu_t*					ctx,
	const tinyobj_t*			obj,
	const tinyobj_shape_t*		obj_shape
	)
{
	clear_struct(mesh);
	load_mesh(mesh, ctx, obj, obj_shape);
}

//## internal
void _pspgu_static_mesh__destruct(_pspgu_static_mesh_t* mesh)
{
	free(mesh->vertex_array);
}

/*=========================================================
FUNCTIONS
=========================================================*/

//## internal
void _pspgu_static_mesh__render
	(
	_pspgu_static_mesh_t*		mesh,
	_pspgu_t*					ctx
	)
{
 	// draw cube
 	sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, mesh->num_verts, 0, mesh->vertex_array);	
}

//## static
/** Loads mesh data. */
static void load_mesh
	(
	_pspgu_static_mesh_t*		mesh,
	_pspgu_t*					ctx,
	const tinyobj_t*			obj,
	const tinyobj_shape_t*		obj_shape
	)
{
	/* For now going wasteful and not eliminating duplicate vertex data */
	int num_faces = obj_shape->length;
	int num_verts = num_faces * 3;
	mesh->num_verts = num_verts;

	/* Allocate memory for the vertex data */
	uint32_t vert_array_size = sizeof(_pspgu_vertex_t) * num_verts;
	mesh->vertex_array = malloc(vert_array_size);
	if (!mesh->vertex_array)
	{
		kk_log__fatal("Failed to allocate memory for mesh vertices.");
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
			//int vn_idx = obj->attrib.faces[i * 3 + j].vn_idx;

			mesh->vertex_array[index].x = obj->attrib.vertices[v_idx * 3 + 0];
			mesh->vertex_array[index].y = obj->attrib.vertices[v_idx * 3 + 1];
			mesh->vertex_array[index].z = obj->attrib.vertices[v_idx * 3 + 2];
							
			//mesh->vertex_array[index].normal.x = obj->attrib.normals[vn_idx * 3 + 0];
			//mesh->vertex_array[index].normal.y = obj->attrib.normals[vn_idx * 3 + 1];
			//mesh->vertex_array[index].normal.z = obj->attrib.normals[vn_idx * 3 + 2];
							
			mesh->vertex_array[index].u = obj->attrib.texcoords[vt_idx * 2 + 0];
			mesh->vertex_array[index].v = obj->attrib.texcoords[vt_idx * 2 + 1];

			mesh->vertex_array[index].color = 0xffffffff;

			index++;
		}
	}
}
