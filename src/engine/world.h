#ifndef WORLD_H
#define WORLD_H

/*=========================================================
DECLARATIONS
=========================================================*/

#include "world_.h"

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"
#include "ecs/ecs.h"
#include "geo/geo.h"

/*=========================================================
TYPES
=========================================================*/

struct world_s
{
	/**
	Create/destroy
	*/
	geo_t			geo;	/* World geometry */
};

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