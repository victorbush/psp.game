#ifndef GEO_H
#define GEO_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "geo/geo_.h"
#include "gpu/gpu_frame_.h"
#include "gpu/gpu_window_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "geo/geo_plane.h"
#include "utl/utl_array.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

utl_array_declare_type(geo_plane_t);

struct geo_s
{
	utl_array_t(geo_plane_t)	planes;
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Initializes a geometry context.
*/
void geo__construct(geo_t* geo);

/**
Destructs a geometry context.
*/
void geo__destruct(geo_t* geo);

/*=========================================================
FUNCTIONS
=========================================================*/

struct geo_plane_s* geo__alloc_plane(geo_t* geo);

void geo__render(geo_t* geo, gpu_window_t* window, gpu_frame_t* frame);

#endif /* GEO_H */