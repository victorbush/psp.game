/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "global.h"
#include "geo/geo.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Initializes a geometry context.
*/
void geo__construct(struct geo_s* geo)
{
	utl_array_init(&geo->planes);
}

/**
Destructs a geometry context.
*/
void geo__destruct(struct geo_s* geo)
{
	utl_array_destroy(&geo->planes);
}

/*=========================================================
FUNCTIONS
=========================================================*/

struct geo_plane_s* geo__alloc_plane(struct geo_s* geo);