/**
 * buffer: reserved space to allow for certain byte values to be drawn
 * 
 * 2 reserved buffers:
 * back buffer (does not display to the user) -> draw into something without displaying the drawing process
 * front buffer (does display to the user)    -> display a fully drawn buffer to the user 
 */
#ifndef RENDERER_H
#define RENDERER_H

#include <inttypes.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

// color helper (packing 4 u8s into u32 RRGGBBAA)
static inline u32 pack_color(u8 r, u8 g, u8 b, u8 a) {
    return (r << 24 | g << 16 | b << 8 | a);
}

// color helpers
#define COLOR_BLACK pack_color(0,   0,   0,   255)
#define COLOR_WHITE pack_color(255, 255, 255, 255)
#define COLOR_RED   pack_color(255, 0,   0,   255)
#define COLOR_GREEN pack_color(0,   255, 0,   255)
#define COLOR_BLUE  pack_color(0,   0,   255, 255)

// vector types
typedef struct {
    u16 x, y;
} Vec2;

typedef struct {
    u16 x, y, z;
} Vec3; // unused for rn (will when z buffering comes in)


// buffer itself
typedef struct {
    u32* data;  // 8 bytes
    u16 width;  // 2 bytes
    u16 height; // 2 bytes

    // u8 padding[4]; // 4 bytes padding, explicit
} Buffer;

// sanity check
static_assert(sizeof(Buffer) == 16, "buffer size doesn't match the expected layout");

// renderer struct
typedef struct {
    // these two will be flipped and the one referenced to front will be displayed to the user
    Buffer* front; // 8 bytes
    Buffer* back;  // 8 bytes

    u16 width;     // 2 bytes
    u16 height;    // 2 bytes

    // potentially add an optional fps cap:
    // u16 fps; // 2 bytes

    // u8 padding[4]; // 4 bytes padding, explicit
} Renderer;

// another sanity check
static_assert(sizeof(Renderer) == 24, "buffer size doesn't match the expected layout");

// buffer lifecycle
Buffer* make_buffer(u16 width, u16 height);      // initialize
void    destroy_buffer(Buffer* buf);             // free
void    clear_buffer(Buffer* buf, u32 color);    // fill a provided buffer with a certain color

// renderer lifecycle
Renderer* make_renderer(u16 width, u16 height);   // initialize
void      destroy_renderer(Renderer* r);          // free
void      clear_renderer(Renderer* r, u32 color); // fills the back buffer with a certain color
void      flip_renderer(Renderer* r);             // swaps back and front

// drawing helpers
void draw_pixel(Renderer* r, u16 x, u16 y, u32 col);
void draw_rectangle(Renderer* r, u16 x, u16 y, u16 w, u16 h, u32 col);
void draw_tri(Renderer* r, Vec2 a, Vec2 b, Vec2 c, u32 col);

// helpers we might need
void renderer_write_ppm(Renderer* r);  // dump renderer content to stdout using a portable pixelmap


// simple check to make sure buffers work fine
static inline int test_renderer() {
    Renderer* r = make_renderer(800, 600);

    // ensure allocation succeeded
    assert(r != NULL);
    assert(r->back  != NULL && r->back->data  != NULL);
    assert(r->front != NULL && r->front->data != NULL);

    // check dimensions match what was requested
    assert(r->width  == 800);
    assert(r->height == 600);
    assert(r->back->width   == 800 && r->back->height  == 600);
    assert(r->front->width  == 800 && r->front->height == 600);

    // check that front and back are separate allocations
    assert(r->back != r->front);
    assert(r->back->data != r->front->data);

    // check writing to back doesn't affect front
    r->back->data[0] = 0xDEADBEEF;
    assert(r->front->data[0] != 0xDEADBEEF);

    // check renderer_clear actually fills the back buffer
    u32 n = (u32)r->back->width * (u32)r->back->height;

    // poison back buffer (to check that clear actually wrote over it)
    for (u32 i = 0; i < n; i++) r->back->data[i] = 0xDEADBEEF;
    clear_renderer(r, COLOR_RED);

    // check that it was overwritten
    for (u32 i = 0; i < n; i++) {
        assert(r->back->data[i] == COLOR_RED);
    }

    // check front buffer is untouched (clear should only target the back buffer)
    assert(r->front->data[0] != COLOR_RED);

    // check flip_renderer swaps pointers
    Buffer* old_front = r->front;
    Buffer* old_back  = r->back;

    flip_renderer(r);

    assert(r->front == old_back);
    assert(r->back  == old_front);

    // check that front now shows what the back was filled with
    assert(r->front->data[0] == COLOR_RED);

    // check that draw_pixel writes correctly (back buffer, right index)
    clear_renderer(r, COLOR_BLACK);
    draw_pixel(r, 10, 5, COLOR_GREEN);
    u32 expected_index = (u32)5 * r->width + 10;
    assert(r->back->data[expected_index] == COLOR_GREEN);

    // neighboring pixels should be untouched
    assert(r->back->data[expected_index - 1] == COLOR_BLACK);
    assert(r->back->data[expected_index + 1] == COLOR_BLACK);

    // check draw_pixel bounds using out of bounds values
    draw_pixel(r, r->width, 0, COLOR_GREEN);
    draw_pixel(r, 0, r->height, COLOR_GREEN);

    // all good!
    printf("sanity check passed\n");
    destroy_renderer(r);
    return 1;
}

#endif