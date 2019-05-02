#ifndef ECS_H
#define ECS_H

/*=========================================================
INCLUDES
=========================================================*/

#include "ecs/components.h"
#include "platforms/common.h"
#include "utl/utl_ringbuf.h"

/*=========================================================
CONSTANTS
=========================================================*/

#define ECS_INVALID_ID (0xFFFFFFFF)
#define MAX_NUM_ENT (500)

/*=========================================================
TYPES
=========================================================*/

typedef uint32_t entity_id_type;

typedef struct
{
	static_model_comp		static_model_comp[MAX_NUM_ENT];
	//anim_model_comp			anim_model_comp[MAX_NUM_ENT];
	transform_comp			transform_comp[MAX_NUM_ENT];

	entity_id_type			recycled_ids[MAX_NUM_ENT];
	utl_ringbuf_type		recycled_ids_ringbuf;

	entity_id_type			next_free_id;

} ecs_type;

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Initializes an ecs context.
@param ecs The context to init.
*/
void ecs_init(ecs_type* ecs);

/**
Allocates an entity id.
@param ecs The ECS context.
@return The allocated entity id. If none availabe, ECS_INVALID_ID.
*/
entity_id_type ecs_alloc_entity(ecs_type* ecs);

/**
Frees an entity id.
@param ecs The ECS context.
@param id The id to free.
*/
void ecs_free_entity(ecs_type* ecs, entity_id_type id);

#endif /* ECS_H */