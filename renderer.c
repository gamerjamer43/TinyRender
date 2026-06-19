#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// buffer api (private)
/**
 * create a buffer of u16 width and u16 height
 */ 
static Buffer* make_buffer(u16 width, u16 height) {
    Buffer* buf = malloc(sizeof(Buffer));

    buf->width = width;
    buf->height = height;

    buf->data = malloc((u32)width * (u32)height * sizeof(u32));

    return buf;
}

/**
 * safe free for buffers
 */ 
static void destroy_buffer(Buffer* buf) {
    if (!buf) return;

    free(buf->data);
    free(buf);
}

/**
 * write a color to a chosen buffer
 */ 
static void clear_buffer(Buffer* buf, u32 color) {
    u32 n_pixels = buf->width * buf->height;
    for (u32 i = 0; i < n_pixels; i++) 
        buf->data[i] = color;
}

// renderer api (public)
/**
 * create a renderer of u16 width and u16 height
 */ 
Renderer* make_renderer(u16 width, u16 height) {
    Renderer* r = malloc(sizeof(Renderer));

    r->width = width;
    r->height = height;

    r->back = make_buffer(width, height);
    r->front = make_buffer(width, height);

    return r;
}

/**
 * safe free for the renderer
 */ 
void destroy_renderer(Renderer* r) {
    if (!r) return;

    destroy_buffer(r->back);
    destroy_buffer(r->front);
    free(r);
}

/**
 * write a color to the back buffer of the renderer
 */ 
void clear_renderer(Renderer* r, u32 color) {
    clear_buffer(r->back, color);
}