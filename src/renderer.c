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

    buf->width  = width;
    buf->height = height;

    buf->data   = malloc((u32)width * (u32)height * sizeof(u32));

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

    r->width  = width;
    r->height = height;

    r->back   = make_buffer(width, height);
    r->front  = make_buffer(width, height);

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

/**
 * switch the back and front buffers
 */
void flip_renderer(Renderer* r) {
    Buffer* temp = r->front;
    r->front     = r->back;
    r->back      = temp;
}

// draw api (public)
/**
 * draw a single pixel to the screen
 */
void draw_pixel(Renderer* r, u16 x, u16 y, u32 color) {
    if (x >= r->width || y >= r->height) return;

    // y * width + x gets us the proper pixel index from a Vec2
    r->back->data[(u32)y * r->width + x] = color;
}

/**
 * draw a rectangle to the screen
 * clean the params up because what the fuckkk
 */
void draw_rectangle(Renderer* r, u16 x, u16 y, u16 width, u16 height, u32 color) {
    for (u16 row = y; row < y + height; row++) {
        for (u16 col = x; col < x + width; col++) {
            draw_pixel(r, col, row, color);
        }
    }
}

// all of this is for drawing triangles.
// i dont even know
/**
 * return the minimum of three numbers
 */
static u16 min3(u16 a, u16 b, u16 c) { return a < b ? (a < c ? a : c) : (b < c ? b : c); }

/**
 * return the maximum of 3 numbers
 */
static u16 max3(u16 a, u16 b, u16 c) { return a > b ? (a > c ? a : c) : (b > c ? b : c); }

/**
 * returns the 2D cross product of vector (a->b) and vector (a->p).
 * this is literal magic to me and i very much so had to look up a lot
 */
static int edge(Vec2 a, Vec2 b, u16 px, u16 py) {
    return (int)(b.x - a.x) * (int)(py - a.y) - (int)(b.y - a.y) * (int)(px - a.x);
}

/**
 * draw a triangle given three (x, y) points and a color.
 */
void draw_tri(Renderer* r, Vec2 a, Vec2 b, Vec2 c, u32 col) {
    u16 minx = min3(a.x, b.x, c.x), maxx = max3(a.x, b.x, c.x);
    u16 miny = min3(a.y, b.y, c.y), maxy = max3(a.y, b.y, c.y);

    for (u16 py = miny; py <= maxy; py++) {
        for (u16 px = minx; px <= maxx; px++) {
            int w0 = edge(a, b, px, py);
            int w1 = edge(b, c, px, py);
            int w2 = edge(c, a, px, py);

            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) ||
                (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
                draw_pixel(r, px, py, col);
            }
        }
    }
}

/**
 * draw a filled circle given a center, radius, and color
 */
void draw_circle(Renderer* r, u16 cx, u16 cy, u16 radius, u32 color) {
    int r2 = (int)radius * (int)radius;

    for (int py = cy - radius; py <= cy + radius; py++) {
        for (int px = cx - radius; px <= cx + radius; px++) {
            // underflow guard
            if (px < 0 || py < 0) continue;

            int dx = px - cx;
            int dy = py - cy;

            if (dx * dx + dy * dy <= r2) {
                draw_pixel(r, (u16)px, (u16)py, color);
            }
        }
    }
}

/**
 * dumps a portable pixelmap containing the front buffer (RRGGBB, alpha is ignored)
 * useful for debugging (maybe. idk)
 */
// void renderer_write_ppm(Renderer* r) {
//     printf("P6\n%d %d\n255\n", r->width, r->height);

//     Buffer* buf = r->front;
//     u32 n_pixels = (u32)buf->width * (u32)buf->height;

//     for (u32 i = 0; i < n_pixels; i++) {
//         u32 px = buf->data[i];
//         putchar((px >> 24) & 0xFF); // red
//         putchar((px >> 16) & 0xFF); // green
//         putchar((px >> 8)  & 0xFF); // blu
//     }
// }