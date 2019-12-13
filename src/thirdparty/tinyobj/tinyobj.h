#ifndef TINYOBJ_H
#define TINYOBJ_H

/*=========================================================
INCLUDES
=========================================================*/

#include "tinyobj_loader_c.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct
{
	tinyobj_attrib_t		attrib;			/* Model attributes and data. */
	tinyobj_shape_t*		shapes;			/* Array of shapes (mesh groups). */
	int						shapes_cnt;		/* The number of mesh groups (shapes). */
	tinyobj_material_t*		materials;		/* Array of materials. */
	int						materials_cnt;	/* The number of materials. */

} tinyobj_t;

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* TINYOBJ_H */