#ifndef UTL_RINGBUF_H
#define UTL_RINGBUF_H

/*=========================================================
INCLUDES
=========================================================*/

#include "platforms/common.h"

/*=========================================================
CONSTANTS
=========================================================*/

/*=========================================================
TYPES
=========================================================*/

typedef struct
{
	uint32_t 	front_idx;
	uint32_t 	back_idx;
	uint32_t 	max_items;
	uint32_t	count;

} utl_ringbuf_t;

/*=========================================================
FUNCTIONS
=========================================================*/

/**
Initializes a ring buffer.
@param buf The buffer to initialize.
@param max_items The max number of items in the buffer.
*/
void utl_ringbuf_init(utl_ringbuf_t* buf, uint32_t max_items);

/**
Returns the next free index in the buffer.
@param buf The buffer to initialize.
@return The next free index.
*/
uint32_t utl_ringbuf_enqueue(utl_ringbuf_t* buf);

/**
Checks if the buffer is empty.
@param buf The buffer to check.
@return TRUE if empty, FALSE otherwise.
*/
boolean utl_ringbuf_is_empty(utl_ringbuf_t*buf);

/**
Checks if the buffer is full.
@param buf The buffer to initialize.
@return TRUE if full, FALSE otherwise.
*/
boolean utl_ringbuf_is_full(utl_ringbuf_t*buf);

/** 
Returns the oldest index in the buffer and frees that slot.
@param buf The buffer to initialize.
@return The index of the item just dequeued.
*/
uint32_t utl_ringbuf_dequeue(utl_ringbuf_t* buf);

#endif /* UTL_RINGBUF_H */