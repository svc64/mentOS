#include <stdint.h>
#include "alloc.h"
#include "exceptions.h"
#include "stdlib.h"

#define DEFAULT_SIZE PAGE_SIZE

struct input_buffer {
    char *buf;
    size_t buf_size;
    uintptr_t read_pos;
    uintptr_t write_pos;
};

struct input_buffer *input_buffer_new(void) {
    struct input_buffer *ibuf = malloc(sizeof(struct input_buffer));
    if (!ibuf) {
        return NULL;
    }
    ibuf->buf = malloc(DEFAULT_SIZE);
    if (!ibuf->buf) {
        free(ibuf);
        return NULL;
    }
    ibuf->buf_size = DEFAULT_SIZE;
    ibuf->read_pos = 0;
    ibuf->write_pos = 0;
    return ibuf;
}

void wipe_buffer(struct input_buffer *ibuf) {
    if (ibuf->buf_size > DEFAULT_SIZE) {
        char *new_input_buffer = realloc(ibuf->buf, DEFAULT_SIZE);
        if (new_input_buffer == NULL) {
            /* this makes no sense. this should never happen because
            we're resizing the buffer to less than the current size */
            panic("wipe_buffer: realloc to DEFAULT_SIZE failed - this makes no sense");
        }
        ibuf->buf = new_input_buffer;
        ibuf->buf_size = DEFAULT_SIZE;
    }
    ibuf->read_pos = 0;
    ibuf->write_pos = 0;
}

void check_reset_buffer(struct input_buffer *ibuf) {
    if (ibuf->read_pos == ibuf->write_pos) {
        // end of the buffer - we can restore it to the default size and restore pos
        wipe_buffer(ibuf);
    } else {
        ibuf->read_pos = ibuf->read_pos % ibuf->buf_size;
    }
}

void input_buffer_push(struct input_buffer *ibuf, char c) {
    if (ibuf->write_pos >= ibuf->buf_size) {
        size_t new_size = ibuf->buf_size + DEFAULT_SIZE;
        char *new_input_buffer = realloc(ibuf->buf, new_size);
        if (new_input_buffer == NULL) {
            // wipe the buffer -- too much input
            wipe_buffer(ibuf);
        } else {
            ibuf->buf_size = new_size;
            ibuf->buf = new_input_buffer;
        }
    }
    ibuf->buf[ibuf->write_pos % ibuf->buf_size] = c;
    ibuf->write_pos++;
}

char input_buffer_pop(struct input_buffer *ibuf) {
    while (!read64(&ibuf->write_pos)); // TODO: idle here
    char c = ibuf->buf[ibuf->read_pos++];
    check_reset_buffer(ibuf);
    return c;
}

char input_buffer_last(struct input_buffer *ibuf) {
    while (!read64(&ibuf->write_pos)); // TODO: idle here
    return ibuf->buf[ibuf->write_pos - 1];
}

void input_buffer_remove(struct input_buffer *ibuf) {
    if (ibuf->write_pos) {
        ibuf->write_pos--;
        check_reset_buffer(ibuf);
    }
}

void input_buffer_free(struct input_buffer *ibuf) {
    free(ibuf->buf);
    free(ibuf);
}
