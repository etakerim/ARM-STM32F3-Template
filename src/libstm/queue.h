#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
#include <stdbool.h>

#define QUEUE_SIZE      256

struct queue {
    uint16_t w;
    uint16_t r;
    uint8_t buf[QUEUE_SIZE];
};

bool queue_write(struct queue *q, uint8_t data);
bool queue_read(struct queue *q, uint8_t *data);
bool queue_peak(struct queue *q, uint8_t *data);

#endif
