/*=========================================================
INCLUDES
=========================================================*/

#include <string.h>

#include "common.h"
#include "global.h"
#include "gpu/gpu.h"
#include "gpu/gpu_material.h"
#include "utl/utl_log.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/**
Finds the length of the string until a specified ending character.
If the end char is not found, the length is 0.

Example buffer:  

If the buffer contains: abcdefg
And end_char is 'f', then function returns 5.				

@param start The buffer to read.
@param end The end of the buffer to read.
@param end_char The character to find.
*/
static int read_until(char* start, char* end, char end_char);

/*=========================================================
FUNCTIONS
=========================================================*/

boolean gpu__parse_material
	(
	const char*					filename,
	gpu_material_t*				out__material
	)
{
	const char* AMBIENT_COLOR_CMD = "a";
	const char* DIFFUSE_COLOR_CMD = "d";
	const char* SPECULAR_COLOR_CMD = "s";
	const char* DIFFUSE_TEXTURE_CMD = "dt";

	long size = 0;
	char* buffer;

//#define DIFFUSE_COLOR_CMD "a"

	/* Read the file */
	if (!g_platform->load_file(filename, FALSE, &size, &buffer))
	{
		FATAL("Failed to open material file.");
	}


	char* cmd_start = buffer;
	char* cmd_end = buffer;

	/* Walk through file until end */
	while (cmd_end <= buffer + size && *cmd_end != 0)
	{
		size_t end_idx = read_until(cmd_start, cmd_end, ' ');
		cmd_end = cmd_start + end_idx;

		if (!strncmp(cmd_start, DIFFUSE_COLOR_CMD, end_idx))
		{
			
		}


	}


	/* Free the file buffer */
	free(buffer);
}

static int read_until(char* start, char* end, char end_char)
{
	int i = 0;
	char* c = start;

	while (c < end && *c != end_char)
	{
		i++;
		c++;
	}

	return i;
}

static boolean read_vec3(char* start, char* end, vec3_t* out)
{
	char* c = start;
	int end_idx = read_until(start, end, ' ');
	atof(c);
}