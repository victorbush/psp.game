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
void geo__construct(geo_t* geo)
{
	utl_array_init(&geo->planes);
}

/**
Destructs a geometry context.
*/
void geo__destruct(geo_t* geo)
{
	for (int i = 0; i < geo->planes.count; ++i)
	{
		geo_plane__destruct(&geo->planes.data[i]);
	}

	utl_array_destroy(&geo->planes);
}

/*=========================================================
FUNCTIONS
=========================================================*/

struct geo_plane_s* geo__alloc_plane(geo_t* geo)
{
	struct geo_plane_s plane;
	clear_struct(&plane);
	utl_array_push(&geo->planes, plane);
	return &geo->planes.data[geo->planes.count - 1];
}

void geo__render(geo_t* geo)
{
	for (int i = 0; i < geo->planes.count; ++i)
	{
		geo_plane__render(&geo->planes.data[i], &g_engine->gpu);
	}
}