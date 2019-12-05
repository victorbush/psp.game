/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/ecs.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
CONSTRUCTORS
=========================================================*/

void ecs__construct(ecs_t* ecs)
{
	memset(ecs, 0, sizeof(*ecs));
	utl_ringbuf_init(&ecs->recycled_ids_ringbuf, MAX_NUM_ENT);
}

void ecs__destruct(ecs_t* ecs)
{
}

/*=========================================================
FUNCTIONS
=========================================================*/

entity_id_t ecs__alloc_entity(ecs_t* ecs)
{
	entity_id_t id = ECS_INVALID_ID;

	/* Check recycled list */
	if (!utl_ringbuf_is_empty(&ecs->recycled_ids_ringbuf))
	{
		/* Use the first available recycled id */
		uint32_t idx = utl_ringbuf_dequeue(&ecs->recycled_ids_ringbuf);
		return ecs->recycled_ids[idx];
	}

	/* Check if buffer full */
	if (ecs->next_free_id == MAX_NUM_ENT)
	{
		return ECS_INVALID_ID;
	}

	/* Use next available id */
	id = ecs->next_free_id;
	ecs->next_free_id++;
	return id;
}

void ecs__free_entity(ecs_t* ecs, entity_id_t id)
{
	if (ecs->next_free_id - 1 == id)
	{
		/* This is the top most id, just decrease next free id */
		ecs->next_free_id--;
		return;
	}

	/* Add to recycled id list */
	uint32_t idx = utl_ringbuf_enqueue(&ecs->recycled_ids_ringbuf);
	ecs->recycled_ids[idx] = id;
}