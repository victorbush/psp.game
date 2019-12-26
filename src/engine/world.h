#ifndef WORLD_H
#define WORLD_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"

/*=========================================================
TYPES
=========================================================*/

typedef struct 
{
	int i;
} world_t;

/*=========================================================
CONSTRUCTORS
=========================================================*/

/**
Loads a world.
@param world The world to construct.
@param filename The name of the world file.
*/
void world__construct(world_t* world, const char* filename);

/**
Destructs a world.
@param world The world to destruct.
*/
void world__destruct(world_t* world);

/*=========================================================
FUNCTIONS
=========================================================*/

#endif /* WORLD_H */