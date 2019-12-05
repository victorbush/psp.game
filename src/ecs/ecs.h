#ifndef ECS_H
#define ECS_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/components.h"
#include "utl/utl_ringbuf.h"

/*=========================================================
CONSTANTS
=========================================================*/

#define ECS_INVALID_ID (0xFFFFFFFF)
#define MAX_NUM_ENT (500)

/*=========================================================
TYPES
=========================================================*/

typedef uint32_t entity_id_t;

typedef struct
{
	static_model_comp_t		static_model_comp[MAX_NUM_ENT];
	//anim_model_comp			anim_model_comp[MAX_NUM_ENT];
	transform_comp_t		transform_comp[MAX_NUM_ENT];

	entity_id_t				recycled_ids[MAX_NUM_ENT];
	utl_ringbuf_t			recycled_ids_ringbuf;

	entity_id_t				next_free_id;

} ecs_t;

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Initializes an ecs context.
@param ecs The context to init.
*/
void ecs__construct(ecs_t* ecs);

/**
Destructs an ecs context.
@param ecs The context to destruct.
*/
void ecs__destruct(ecs_t* ecs);

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Allocates an entity id.
@param ecs The ECS context.
@return The allocated entity id. If none availabe, ECS_INVALID_ID.
*/
entity_id_t ecs__alloc_entity(ecs_t* ecs);

/**
Frees an entity id.
@param ecs The ECS context.
@param id The id to free.
*/
void ecs__free_entity(ecs_t* ecs, entity_id_t id);

#endif /* ECS_H */