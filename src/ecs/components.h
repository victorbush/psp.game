#ifndef COMPONENTS_H
#define COMPONENTS_H

/*=========================================================
INCLUDES
=========================================================*/

#include "platforms/platform.h"
#include "platforms/common.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct
{
	boolean 	is_used;
	char		file_name[ MAX_FILENAME_CHARS ];

	

} static_model_comp;

typedef struct
{
	boolean 	is_used;
	vec3_t		pos;

} transform_comp;

#endif /* COMPONENTS_H */