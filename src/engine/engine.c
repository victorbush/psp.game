/*=========================================================
INCLUDES
=========================================================*/

#include "ecs/ecs.h"
#include "engine/engine.h"

#include "ecs/systems/render_system.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

/**
engine_init 
*/
void engine_init(engine_type* eng)
{
    ecs_init(eng->ecs);
	eng->gpu->init(eng->gpu);


    // TODO : temp
    entity_id_type id;
    id = ecs_alloc_entity(&eng->ecs);
    eng->ecs->transform_comp[id].is_used = TRUE;
    eng->ecs->transform_comp[id].pos.x = 10.0f;
    eng->ecs->transform_comp[id].pos.y = 0.0f;
    eng->ecs->transform_comp[id].pos.z = -20.0f;

    id = ecs_alloc_entity(&eng->ecs);
    eng->ecs->transform_comp[id].is_used = TRUE;
    eng->ecs->transform_comp[id].pos.x = -10.0f;
    eng->ecs->transform_comp[id].pos.y = 0.0f;
    eng->ecs->transform_comp[id].pos.z = -50.0f;
}

/**
engine_term 
*/
void engine_term(engine_type* eng)
{
	eng->gpu->term(eng->gpu);
}

void engine_run_frame(engine_type* eng)
{
	eng->gpu->begin_frame(&eng->gpu);

	render_system_run(eng, &eng->ecs);

	eng->gpu->end_frame(&eng->gpu);
}