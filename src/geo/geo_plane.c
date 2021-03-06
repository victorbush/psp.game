/*=========================================================
INCLUDES
=========================================================*/

#include <string.h>

#include "common.h"
#include "global.h"
#include "ecs/ecs.h"
#include "engine/kk_log.h"
#include "geo/geo.h"
#include "geo/geo_plane.h"
#include "gpu/gpu.h"
#include "gpu/gpu_frame.h"
#include "lua/lua_script.h"

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

void geo_plane__construct(geo_plane_t* plane, gpu_t* gpu)
{
	clear_struct(plane);
	plane->gpu = gpu;

	gpu_plane__construct(&plane->plane, gpu);
}

void geo_plane__destruct(geo_plane_t* plane)
{
	gpu_plane__destruct(&plane->plane, plane->gpu);
}

/*=========================================================
FUNCTIONS
=========================================================*/

void geo_plane__load(geo_plane_t* plane, lua_script_t* lua)
{
	/* Loop through members */
	boolean loop = lua_script__start_loop(lua);
	while (loop && lua_script__next(lua))
	{
		/* Get next member name */
		char key[MAX_COMPONENT_NAME];
		if (!lua_script__get_key(lua, key, sizeof(key)))
		{
			kk_log__error("Expected key.");
		}

		/* Vertices */
		if (!strncmp(key, VERTS_NAME, sizeof(key)))
		{
			kk_vec3_t verts[4];

			/* Array of floats represnting (x,y,z) for each of the 4 corners of the plane (12 values total) */
			if (!lua_script__get_array_of_float(lua, (float*)verts, 12))
			{
				kk_log__error("Invalid plane vertices.");
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
				kk_log__error("Invalid material filename.");
			}

			/* Load material */
			plane->material = gpu__load_material(g_gpu, material_file);
		}
	}
}

void geo_plane__render(geo_plane_t* plane, gpu_t* gpu, gpu_window_t* window, gpu_frame_t* frame)
{
	gpu_plane__render(&plane->plane, gpu, window, frame, plane->material);
}