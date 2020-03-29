/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "engine/kk_log.h"
#include "gpu/gpu_frame.h"
#include "gpu/vlk/vlk.h"
#include "gpu/vlk/vlk_prv.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
DECLARATIONS
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void vlk_frame__construct(gpu_frame_t* frame, gpu_t* gpu)
{
	/* Allocate implementation memory */
	frame->derived = malloc(sizeof(_vlk_frame_t));
	if (!frame->derived)
	{
		log__fatal("Failed to allocate memory.");
	}

	_vlk_frame_t* vlk_frame = (_vlk_frame_t*)frame->derived;
	vlk_frame->base = frame;
}

void vlk_frame__destruct(gpu_frame_t* frame, gpu_t* gpu)
{
	/* Free implementation memory */
	free(frame->derived);
}

/*=========================================================
FUNCTIONS
=========================================================*/

_vlk_frame_t* _vlk_frame__from_base(gpu_frame_t* frame)
{
	return (_vlk_frame_t*)frame->derived;
}