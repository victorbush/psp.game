/*=========================================================
INCLUDES
=========================================================*/

#include "utl/utl_ringbuf.h"

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

/** 
utl_ringbuf_dequeue
*/
uint32_t utl_ringbuf_dequeue(utl_ringbuf_type* buf)
{
	if (buf->back_idx == buf->front_idx)
	{
		/* Buffer is empty */
		return buf->back_idx;
	}

	uint32_t idx = buf->back_idx;
	buf->back_idx++;
	return idx;
}

/**
utl_ringbuf_enqueue
*/
uint32_t utl_ringbuf_enqueue(utl_ringbuf_type* buf)
{
	if (utl_ringbuf_is_full(buf))
	{
		/* If buffer is full, will keep overwriting current index */
		return buf->front_idx;
	}

	uint32_t idx = buf->front_idx;
	buf->front_idx = (buf->front_idx + 1) % buf->max_items;
	return idx;
}

/**
utl_ringbuf_init
*/
void utl_ringbuf_init(utl_ringbuf_type* buf, uint32_t max_items)
{
	buf->back_idx = 0;
	buf->front_idx = 0;
	buf->max_items = max_items;
}

/**
utl_ringbuf_is_empty
*/
boolean utl_ringbuf_is_empty(utl_ringbuf_type *buf)
{
	return (buf->front_idx == buf->back_idx);
}

/**
utl_ringbuf_is_full
*/
boolean utl_ringbuf_is_full(utl_ringbuf_type *buf)
{
	if ((buf->front_idx + 1) % buf->max_items == buf->back_idx)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
