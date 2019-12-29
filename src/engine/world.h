#ifndef WORLD_H
#define WORLD_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct ecs_s ecs_t;

typedef struct 
{
	int i;
	struct geo_s*	geo;	/* World geometry */
	
} world_t;

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Loads a world.
@param world The world to construct.
@param ecs The entity component system to use.
@param filename The name of the world file.
*/
void world__construct(world_t* world, ecs_t* ecs, const char* filename);

/**
Destructs a world.
@param world The world to destruct.
*/
void world__destruct(world_t* world);

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* WORLD_H */