#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "gpu/gpu_window_.h"
#include "gpu/gpu_frame_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "ecs/ecs.h"
#include "engine/engine.h"

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

void render_system__run(engine_t* eng, ecs_t* ecs, gpu_window_t* window, gpu_frame_t* frame);

#endif /* RENDER_SYSTEM_H */