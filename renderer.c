#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Buffer *make_buffer(u16 width, u16 height) {
    Buffer* buf = malloc(sizeof(Buffer));

    buf->width = width;
    buf->height = height;

    buf->data = malloc((u32)width * (u32)height * sizeof(u32));

    return buf;
}

void destroy_buffer(Buffer *buf) {
    if (!buf) return;

    free(buf->data);
    free(buf);
}

Renderer *make_renderer(u16 width, u16 height) {
    Renderer* r = malloc(sizeof(Renderer));

    r->width = width;
    r->height = height;

    r->back = make_buffer(width, height);
    r->front = make_buffer(width, height);

    return r;
}

void destroy_renderer(Renderer *r) {
    if (!r) return;
    
    destroy_buffer(r->back);
    destroy_buffer(r->front);
    free(r);
}