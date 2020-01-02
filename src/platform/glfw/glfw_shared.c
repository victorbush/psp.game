/*=========================================================
INCLUDES
=========================================================*/

#include <stdio.h>

#include "common.h"
#include "log/log.h"
#include "platform/platform.h"
#include "platform/glfw/glfw.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

uint32_t glfw__get_time(platform_t* platform)
{
	return (uint32_t)glfwGetTime();
}

boolean glfw__load_file(const char* filename, boolean binary, long *out__size, void** out__buffer)
{
	FILE* f;
	errno_t err;

	/* Open the file */
	if (binary)
	{
		err = fopen_s(&f, filename, "rb");
	}
	else
	{
		err = fopen_s(&f, filename, "r");
	}

	/* Make sure file was opened */
	if (err != 0 || !f)
	{
		log__error("Failed to open file.");
		return FALSE;
	}

	/* Get file length */
	fseek(f, 0, SEEK_END);
	*out__size = ftell(f);

	/* Alloc buffer */
	*out__buffer = malloc(*out__size);
	if (!*out__buffer)
	{
		log__error("Failed to allocate file buffer.");
		return FALSE;
	}
	
	/* Read file into memory */
	fseek(f, 0, SEEK_SET);
	fread_s(*out__buffer, *out__size, *out__size, 1, f);

	/* Close file */
	fclose(f);

	/* Success */
	return TRUE;
}

void glfw__log_to_stdout(log_t* log, const char* msg)
{
	printf_s(msg);
}
