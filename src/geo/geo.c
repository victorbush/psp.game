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
	for (int i = 0; i < geo->planes.count; ++i)
	{
		geo_plane__destruct(&geo->planes.data[i]);
	}

	utl_array_destroy(&geo->planes);
}

/*=========================================================
FUNCTIONS
=========================================================*/

struct geo_plane_s* geo__alloc_plane(struct geo_s* geo)
{
	struct geo_plane_s plane;
	clear_struct(&plane);
	utl_array_push(&geo->planes, plane);
	return &geo->planes.data[geo->planes.count - 1];
}

void geo__render(struct geo_s* geo)
{
	for (int i = 0; i < geo->planes.count; ++i)
	{
		geo_plane__render(&geo->planes.data[i], &g_engine->gpu);
	}
}