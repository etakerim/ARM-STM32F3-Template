#include "queue.h"


static inline bool is_queue_full(struct queue *q)
{
    return ((q->w + 1) % QUEUE_SIZE) == q->r;
}

static inline bool is_queue_empty(struct queue *q)
{
    return q->w == q->r;
}


bool queue_write(struct queue *q, uint8_t data)
{
    if (is_queue_full(q))
        return false;

    q->buf[q->w] = data;
    q->w = (q->w + 1 == QUEUE_SIZE) ? 0: q->w + 1;
    return true;
}

bool queue_read(struct queue *q, uint8_t *data)
{
    if (is_queue_empty(q))
        return false;

    *data = q->buf[q->r];
    q->r = (q->r + 1 == QUEUE_SIZE) ? 0: q->r + 1;
    return true;
}

bool queue_peak(struct queue *q, uint8_t *data)
{
    if (is_queue_empty(q))
        return false;

    *data = q->buf[q->r];
    return true;
}

/*
Disable ISR - Write by 2 producers - or lock
#include <core_cmInstr.h>
uint32_t prim = __get_PRIMASK();

__disable_irq();
//LOCKED
if (!prim)
    __enable_irq();
*/
