#ifndef GEO_H
#define GEO_H

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

typedef struct geo_plane_s geo_plane_t;
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
void geo__construct(struct geo_s* geo);

/**
Destructs a geometry context.
*/
void geo__destruct(struct geo_s* geo);

/*=========================================================
FUNCTIONS
=========================================================*/

struct geo_plane_s* geo__alloc_plane(struct geo_s* geo);

void geo__render(struct geo_s* geo);

#endif /* GEO_H */