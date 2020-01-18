/*=========================================================
INCLUDES
=========================================================*/

#include <assert.h>

#include "tests/tests.h"
#include "utl/utl_ringbuf.h"

/*=========================================================
TYPES
=========================================================*/

/*=========================================================
VARIABLES
=========================================================*/

/*=========================================================
FUNCTIONS
=========================================================*/

static void test_calc_next_idx()
{
	utl_ringbuf_t buf;
	utl_ringbuf_init(&buf, 5);

	assert(1 == utl_ringbuf_calc_next_idx(&buf, 0));
	assert(2 == utl_ringbuf_calc_next_idx(&buf, 1));
	assert(4 == utl_ringbuf_calc_next_idx(&buf, 3));
	assert(0 == utl_ringbuf_calc_next_idx(&buf, 4));
}

static void test_calc_prev_idx()
{
	utl_ringbuf_t buf;
	utl_ringbuf_init(&buf, 5);

	assert(4 == utl_ringbuf_calc_prev_idx(&buf, 0));
	assert(0 == utl_ringbuf_calc_prev_idx(&buf, 1));
	assert(2 == utl_ringbuf_calc_prev_idx(&buf, 3));
	assert(3 == utl_ringbuf_calc_prev_idx(&buf, 4));
}

static void test_init()
{
	utl_ringbuf_t zero_buf;
	utl_ringbuf_init(&zero_buf, 0);
	assert(zero_buf.back_idx == 0);
	assert(zero_buf.front_idx == 0);
	assert(zero_buf.count == 0);
	assert(zero_buf.max_items == 0);

	utl_ringbuf_t buf;
	utl_ringbuf_init(&buf, 5);
	assert(buf.back_idx == 0);
	assert(buf.front_idx == 0);
	assert(buf.count == 0);
	assert(buf.max_items == 5);
}

static void test_enqueue()
{
	utl_ringbuf_t buf;
	utl_ringbuf_init(&buf, 5);

	assert(utl_ringbuf_enqueue(&buf) == 0);
	assert(buf.count == 1);
	assert(utl_ringbuf_enqueue(&buf) == 1);
	assert(buf.count == 2);
	assert(utl_ringbuf_enqueue(&buf) == 2);
	assert(buf.count == 3);
	assert(utl_ringbuf_enqueue(&buf) == 3);
	assert(buf.count == 4);
	assert(utl_ringbuf_enqueue(&buf) == 4);
	assert(buf.count == 5);
	assert(utl_ringbuf_enqueue(&buf) == 0);
	assert(buf.count == 5);
	assert(utl_ringbuf_enqueue(&buf) == 0);
}

static void test_is_empty()
{
	utl_ringbuf_t buf;
	utl_ringbuf_init(&buf, 5);

	assert(utl_ringbuf_is_empty(&buf) == TRUE);
	utl_ringbuf_enqueue(&buf);
	assert(utl_ringbuf_is_empty(&buf) == FALSE);
}

static void test_is_full()
{
	utl_ringbuf_t buf;
	utl_ringbuf_init(&buf, 2);

	assert(utl_ringbuf_is_full(&buf) == FALSE);
	utl_ringbuf_enqueue(&buf);
	assert(utl_ringbuf_is_full(&buf) == FALSE);
	utl_ringbuf_enqueue(&buf);
	assert(utl_ringbuf_is_full(&buf) == TRUE);
}

static void test_dequeue()
{
	utl_ringbuf_t buf;
	utl_ringbuf_init(&buf, 5);

	utl_ringbuf_enqueue(&buf);
	utl_ringbuf_enqueue(&buf);
	utl_ringbuf_enqueue(&buf);

	assert(buf.count == 3);
	assert(utl_ringbuf_dequeue(&buf) == 0);
	assert(buf.count == 2);
	assert(utl_ringbuf_dequeue(&buf) == 1);
	assert(buf.count == 1);
	assert(utl_ringbuf_dequeue(&buf) == 2);
	assert(buf.count == 0);
	assert(utl_ringbuf_dequeue(&buf) == 3);
	assert(buf.count == 0);
	assert(utl_ringbuf_dequeue(&buf) == 3);
	assert(buf.count == 0);

	/* Enqueue to wraparound, then dequeue all */
	utl_ringbuf_enqueue(&buf);
	utl_ringbuf_enqueue(&buf);
	assert(utl_ringbuf_enqueue(&buf) == 0);

	assert(buf.count == 3);
	assert(utl_ringbuf_dequeue(&buf) == 3);
	assert(buf.count == 2);
	assert(utl_ringbuf_dequeue(&buf) == 4);
	assert(buf.count == 1);
	assert(utl_ringbuf_dequeue(&buf) == 0);
	assert(buf.count == 0);
}

static void test_dequeue_front()
{
	utl_ringbuf_t buf;
	utl_ringbuf_init(&buf, 5);

	utl_ringbuf_enqueue(&buf);
	utl_ringbuf_enqueue(&buf);
	utl_ringbuf_enqueue(&buf);

	assert(buf.count == 3);
	assert(utl_ringbuf_dequeue_front(&buf) == 2);
	assert(buf.count == 2);
	assert(utl_ringbuf_dequeue_front(&buf) == 1);
	assert(buf.count == 1);
	assert(utl_ringbuf_dequeue_front(&buf) == 0);
	assert(buf.count == 0);
	assert(utl_ringbuf_dequeue_front(&buf) == 0);
	assert(buf.count == 0);
	assert(utl_ringbuf_dequeue_front(&buf) == 0);
	assert(buf.count == 0);

	/* Enqueue to wraparound, then dequeue all */
	utl_ringbuf_enqueue(&buf);
	utl_ringbuf_enqueue(&buf);
	utl_ringbuf_enqueue(&buf);
	utl_ringbuf_enqueue(&buf);
	utl_ringbuf_enqueue(&buf);
	utl_ringbuf_dequeue(&buf); /* Dequeue to get wraparound */
	assert(utl_ringbuf_enqueue(&buf) == 0);

	assert(buf.count == 5);
	assert(utl_ringbuf_dequeue_front(&buf) == 0);
	assert(buf.count == 4);
	assert(utl_ringbuf_dequeue_front(&buf) == 4);
	assert(buf.count == 3);
	assert(utl_ringbuf_dequeue_front(&buf) == 3);
	assert(buf.count == 2);
	assert(utl_ringbuf_dequeue_front(&buf) == 2);
	assert(buf.count == 1);
	assert(utl_ringbuf_dequeue_front(&buf) == 1);
	assert(buf.count == 0);
}

void utl_ringbuf_tests()
{
	RUN_TEST_CASE(test_calc_next_idx);
	RUN_TEST_CASE(test_calc_prev_idx);
	RUN_TEST_CASE(test_init);
	RUN_TEST_CASE(test_enqueue);
	RUN_TEST_CASE(test_is_empty);
	RUN_TEST_CASE(test_is_full);
	RUN_TEST_CASE(test_dequeue);
	RUN_TEST_CASE(test_dequeue_front);
}