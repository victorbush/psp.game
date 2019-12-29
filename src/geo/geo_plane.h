#ifndef GEO_PLANE_H
#define GEO_PLANE_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "gpu/gpu_material.h"
#include "gpu/gpu_plane.h"
#include "utl/utl_math.h"

/*=========================================================
CONSTANTS
=========================================================*/

extern const char* GEO_PLANE_NAME;

/*=========================================================
TYPES
=========================================================*/

struct geo_plane_s
{
	gpu_t*				gpu;
	gpu_material_t*		material;

	/*
	Create/destroy
	*/
	gpu_plane_t			plane;
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

void geo_plane__construct(struct geo_plane_s* plane, gpu_t* gpu);

void geo_plane__destruct(struct geo_plane_s* plane);

/*=========================================================
FUNCTIONS
=========================================================*/

void geo_plane__load(struct geo_plane_s* plane, struct lua_script_s* script);

#endif /* GEO_PLANE_H */