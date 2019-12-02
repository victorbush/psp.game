#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

/*=========================================================
INCLUDES
=========================================================*/

#include "ecs/ecs.h"
#include "engine/engine.h"
#include "platforms/common.h"

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

void render_system_run(engine_t* eng, ecs_t* ecs);

#endif /* RENDER_SYSTEM_H */