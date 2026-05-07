#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 8   // Must be power of 2 for bitwise wrap

typedef struct {
    uint8_t buffer[BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} ringbuf_t;

/* Initialize ring buffer */
void ringbuf_init(ringbuf_t *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

/* Check if buffer is full */
bool ringbuf_is_full(const ringbuf_t *rb)
{
    return (rb->count == BUFFER_SIZE);
}

/* Check if buffer is empty */
bool ringbuf_is_empty(const ringbuf_t *rb)
{
    return (rb->count == 0);
}

/* Get number of bytes in buffer */
uint8_t ringbuf_count(const ringbuf_t *rb)
{
    return rb->count;
}

/*
 * Write one byte into ring buffer
 * Returns 0 on success, -1 if buffer is full
 */
int ringbuf_write(ringbuf_t *rb, uint8_t data)
{
    if (ringbuf_is_full(rb)) {
        return -1;
    }

    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) & (BUFFER_SIZE - 1); 
    rb->count++;

    return 0;
}

/*
 * Read one byte from ring buffer
 * Returns 0 on success, -1 if buffer is empty
 */
int ringbuf_read(ringbuf_t *rb, uint8_t *data)
{
    if (ringbuf_is_empty(rb)) {
        return -1;
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) & (BUFFER_SIZE - 1);
    rb->count--;

    return 0;
}

int main(void)
{
    ringbuf_t rb;
    uint8_t data;
    uint8_t i;

    ringbuf_init(&rb);

    /* Step 1: Write 8 bytes */
    for (i = 0; i < 8; i++) {
        uint8_t value = 0x41 + i;
        if (ringbuf_write(&rb, value) == 0) {
            printf("[WRITE] 0x%02X -> OK  (count=%d)%s\n",
                   value,
                   ringbuf_count(&rb),
                   ringbuf_is_full(&rb) ? " FULL" : "");
        }
    }

    /* Step 2: Attempt extra write */
    if (ringbuf_write(&rb, 0x99) != 0) {
        printf("[WRITE] 0x99 -> FAIL (buffer full)\n");
    }

    /* Step 3: Read 3 bytes */
    for (i = 0; i < 3; i++) {
        if (ringbuf_read(&rb, &data) == 0) {
            printf("[READ]        -> 0x%02X  (count=%d)\n",
                   data, ringbuf_count(&rb));
        }
    }

    /* Step 4: Write 3 more bytes */
    for (i = 0; i < 3; i++) {
        uint8_t value = 0x49 + i;
        if (ringbuf_write(&rb, value) == 0) {
            printf("[WRITE] 0x%02X -> OK  (count=%d)\n",
                   value, ringbuf_count(&rb));
        }
    }

    /* Step 5: Read all remaining bytes */
    while (!ringbuf_is_empty(&rb)) {
        ringbuf_read(&rb, &data);
        printf("[READ]        -> 0x%02X  (count=%d)\n",
               data, ringbuf_count(&rb));
    }

    /* Step 6: Attempt read on empty buffer */
    if (ringbuf_read(&rb, &data) != 0) {
        printf("[READ]  (empty) -> FAIL (buffer empty)\n");
    }

    return 0;
}
