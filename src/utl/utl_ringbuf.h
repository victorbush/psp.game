#ifndef UTL_RINGBUF_H
#define UTL_RINGBUF_H

/*=========================================================
INCLUDES
=========================================================*/

#include "common.h"

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
Calculates the next index in front of the specified index, accounting for wrap-around.
Does not care if the index is in use or not.

@param buf The buffer metadata.
@param idx The next to get the next index from.
*/
uint32_t utl_ringbuf_calc_next_idx(utl_ringbuf_t* buf, uint32_t idx);

/**
Calculates the previous index behind the specified index, accounting for wrap-around.
Does not care if the index is in use or not.

@param buf The buffer metadata.
@param idx The next to get the previous index from.
*/
uint32_t utl_ringbuf_calc_prev_idx(utl_ringbuf_t* buf, uint32_t idx);

/**
Returns the oldest index in the buffer and frees that slot.

@param buf The buffer metadata.
@return The index of the item just dequeued.
*/
uint32_t utl_ringbuf_dequeue(utl_ringbuf_t* buf);

/**
Returns the newest index in the buffer and frees that slot.

@param buf The buffer metadata.
@return The index of the item just dequeued.
*/
uint32_t utl_ringbuf_dequeue_front(utl_ringbuf_t* buf);

/**
Returns the next free index in the buffer.

@param buf The buffer metadata.
@return The next free index.
*/
uint32_t utl_ringbuf_enqueue(utl_ringbuf_t* buf);

/**
Initializes a ring buffer.

@param buf The buffer metadata to initialize.
@param max_items The max number of items in the buffer.
*/
void utl_ringbuf_init(utl_ringbuf_t* buf, uint32_t max_items);

/**
Checks if the buffer is empty.

@param buf The buffer metadata to check.
@return TRUE if empty, FALSE otherwise.
*/
boolean utl_ringbuf_is_empty(utl_ringbuf_t*buf);

/**
Checks if the buffer is full.

@param buf The buffer metadata to check.
@return TRUE if full, FALSE otherwise.
*/
boolean utl_ringbuf_is_full(utl_ringbuf_t*buf);

#endif /* UTL_RINGBUF_H */