#ifndef GPU_FRAME_H
#define GPU_FRAME_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "gpu/gpu_.h"
#include "gpu/gpu_frame_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"

/*=========================================================
TYPES
=========================================================*/

/**
Contains info about the current frame.
*/
struct gpu_frame_s
{
	void*				derived;			/* GPU-specific implementation data. */

	/*
	Dependencies
	*/
	gpu_t*				gpu;				/* GPU context */

	/*
	Other
	*/
	uint8_t				frame_idx;
};

/*=========================================================
CONSTRUCTORS
=========================================================*/

void gpu_frame__construct(gpu_frame_t* frame, gpu_t* gpu);

void gpu_frame__destruct(gpu_frame_t* frame, gpu_t* gpu);

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* GPU_FRAME_H */