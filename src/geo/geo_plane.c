/*=========================================================
INCLUDES
=========================================================*/

#include <string.h>

#include "common.h"
#include "global.h"
#include "geo/geo.h"
#include "geo/geo_plane.h"
#include "gpu/gpu.h"
#include "lua/lua_script.h"
#include "utl/utl_log.h"

/*=========================================================
CONSTANTS
=========================================================*/

const char* GEO_PLANE_NAME = "plane";
static const char* MATERIAL_NAME = "material";
static const char* VERTS_NAME = "verts";

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void geo_plane__construct(struct geo_plane_s* plane, gpu_t* gpu)
{
	clear_struct(plane);
	plane->gpu = gpu;

	gpu_plane__construct(&plane->plane, gpu);
}

void geo_plane__destruct(struct geo_plane_s* plane)
{
	gpu_plane__destruct(&plane->plane, plane->gpu);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void geo_plane__load(struct geo_plane_s* plane, struct lua_script_s* lua)
{
	/* Loop through members */
	boolean loop = lua_script__start_loop(lua);
	while (loop && lua_script__next(lua))
	{
		/* Get next member name */
		char key[MAX_COMPONENT_NAME];
		if (!lua_script__get_key(lua, key, sizeof(key)))
		{
			LOG_ERROR("Expected key.");
		}

		/* Vertices */
		if (!strncmp(key, VERTS_NAME, sizeof(key)))
		{
			vec3_t verts[4];

			/* Array of floats represnting (x,y,z) for each of the 4 corners of the plane (12 values total) */
			if (!lua_script__get_array_of_float(lua, (float*)verts, 12))
			{
				LOG_ERROR("Invalid plane vertices.");
				continue;
			}

			/* Update plane verts */
			gpu_plane__update_verts(&plane->plane, plane->gpu, verts);
		}

		/* Material */
		if (!strncmp(key, MATERIAL_NAME, sizeof(key)))
		{
			char material_file[MAX_FILENAME_CHARS];
			if (!lua_script__get_string(lua, material_file, sizeof(material_file)))
			{
				LOG_ERROR("Invalid material filename.");
			}

			/* Load material */
			plane->material = gpu__load_material(&g_engine->gpu, material_file);
		}
	}
}

void geo_plane__render(struct geo_plane_s* plane, gpu_t* gpu)
{
	gpu_plane__render(&plane->plane, gpu, plane->material);
}