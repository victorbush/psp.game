#ifndef GEO_PLANE_H
#define GEO_PLANE_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "geo_plane_.h"
#include "gpu/gpu_.h"
#include "gpu/gpu_frame_.h"
#include "gpu/gpu_material_.h"
#include "gpu/gpu_window_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_math.h"
#include "gpu/gpu_plane.h"
#include "lua/lua_script.h"

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

void geo_plane__construct(geo_plane_t* plane, gpu_t* gpu);

void geo_plane__destruct(geo_plane_t* plane);

/*=========================================================
FUNCTIONS
=========================================================*/

void geo_plane__load(geo_plane_t* plane, lua_script_t* script);

void geo_plane__render(geo_plane_t* plane, gpu_t* gpu, gpu_window_t* window, gpu_frame_t* frame);

#endif /* GEO_PLANE_H */