/*=========================================================
INCLUDES
=========================================================*/

#include <assert.h>

#include "utl/utl_ringbuf.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

uint32_t utl_ringbuf_calc_next_idx(utl_ringbuf_t* buf, uint32_t idx)
{
	/* Check if index out of range */
	assert(idx < buf->max_items);

	return (idx + 1) % buf->max_items;
}

uint32_t utl_ringbuf_calc_prev_idx(utl_ringbuf_t* buf, uint32_t idx)
{
	/* Check if index out of range */
	assert(idx < buf->max_items);

	return (idx - 1 + buf->max_items) % buf->max_items;
}

/**
utl_ringbuf_dequeue
*/
uint32_t utl_ringbuf_dequeue(utl_ringbuf_t* buf)
{
	if (buf->count == 0)
	{
		/* Buffer is empty */
		return buf->back_idx;
	}

	uint32_t idx = buf->back_idx;
	buf->back_idx = utl_ringbuf_calc_next_idx(buf, buf->back_idx);
	buf->count--;
	return idx;
}

/**
utl_ringbuf_dequeue_front
*/
uint32_t utl_ringbuf_dequeue_front(utl_ringbuf_t* buf)
{
	if (buf->count == 0)
	{
		/* Buffer is empty */
		return buf->back_idx;
	}

	/* Front_idx points to the next free slot, so return one less to get the front used slot */
	buf->front_idx = utl_ringbuf_calc_prev_idx(buf, buf->front_idx);
	buf->count--;
	return buf->front_idx;
}

/**
utl_ringbuf_enqueue
*/
uint32_t utl_ringbuf_enqueue(utl_ringbuf_t* buf)
{
	if (buf->count == buf->max_items)
	{
		/* If buffer is full, will keep overwriting current index */
		return buf->front_idx;
	}

	uint32_t idx = buf->front_idx;
	buf->front_idx = utl_ringbuf_calc_next_idx(buf, buf->front_idx);
	buf->count++;
	return idx;
}

/**
utl_ringbuf_init
*/
void utl_ringbuf_init(utl_ringbuf_t* buf, uint32_t max_items)
{
	buf->back_idx = 0;
	buf->front_idx = 0;
	buf->count = 0;
	buf->max_items = max_items;
}

/**
utl_ringbuf_is_empty
*/
boolean utl_ringbuf_is_empty(utl_ringbuf_t*buf)
{
	return (buf->count == 0);
}

/**
utl_ringbuf_is_full
*/
boolean utl_ringbuf_is_full(utl_ringbuf_t*buf)
{
	return (buf->count == buf->max_items);
}
