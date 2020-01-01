/*=========================================================
INCLUDES
=========================================================*/

#include <pspgu.h>
#include <pspgum.h>

#include "common.h"
#include "gpu/pspgu/pspgu_prv.h"

/*=========================================================
MACROS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/** Index buffer for planes. */
static uint8_t s_plane_indices[6] =
{
	0, 1, 3,
	1, 2, 3
};

/*=========================================================
DECLARATIONS
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void _pspgu_plane__construct
	(
	_pspgu_plane_t*				plane,
	_pspgu_t*					ctx
	)
{
	clear_struct(plane);
}

void _pspgu_plane__destruct(_pspgu_plane_t* plane)
{
}

/*=========================================================
FUNCTIONS
=========================================================*/

void _pspgu_plane__render
	(
	_pspgu_plane_t*				plane,
	_pspgu_t*					ctx
	)
{
 	sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_INDEX_8BIT | GU_TRANSFORM_3D, 6, s_plane_indices, plane->verts);	
}

void _pspgu_plane__update_verts
	(
	_pspgu_plane_t* 			plane,
	_pspgu_t* 					ctx,
	vec3_t 						verts[4]
	)
{
	plane->verts[0].x = verts[0].x;
	plane->verts[0].y = verts[0].y;
	plane->verts[0].z = verts[0].z;
	plane->verts[0].color = 0xffffffff;
	plane->verts[0].u = 0.0f;
	plane->verts[0].v = 0.0f;

	plane->verts[1].x = verts[1].x;
	plane->verts[1].y = verts[1].y;
	plane->verts[1].z = verts[1].z;
	plane->verts[1].color = 0xffffffff;
	plane->verts[1].u = 1.0f;
	plane->verts[1].v = 0.0f;

	plane->verts[2].x = verts[2].x;
	plane->verts[2].y = verts[2].y;
	plane->verts[2].z = verts[2].z;
	plane->verts[2].color = 0xffffffff;
	plane->verts[2].u = 1.0f;
	plane->verts[2].v = 1.0f;

	plane->verts[3].x = verts[3].x;
	plane->verts[3].y = verts[3].y;
	plane->verts[3].z = verts[3].z;
	plane->verts[3].color = 0xffffffff;
	plane->verts[3].u = 0.0f;
	plane->verts[3].v = 1.0f;
}