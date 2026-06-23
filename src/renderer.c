#include "renderer.h"

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
    r->fps    = 0;

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

/**
 * load in a sprite given its path. it can then be drawn to the screen by reference passing
 */
Sprite* sprite_load(const char* path) {
    SDL_Surface* raw = IMG_Load(path);
    if (!raw) {
        fprintf(stderr, "sprite_load: %s\n", IMG_GetError());
        return NULL;
    }

    // RGBA32 aligns with what i have setup
    SDL_Surface* surf = SDL_ConvertSurfaceFormat(raw, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(raw);
    if (!surf) return NULL;

    // malloc and setup
    Sprite* s  = malloc(sizeof(Sprite));
    s->width   = (u16)surf->w;
    s->height  = (u16)surf->h;
    s->data    = malloc((u32)s->width * s->height * sizeof(u32));

    // copy pixels
    u8* src = (u8*)surf->pixels;
    for (u32 i = 0; i < (u32)s->width * s->height; i++) {
        u8 r = src[i * 4 + 0];
        u8 g = src[i * 4 + 1];
        u8 b = src[i * 4 + 2];
        u8 a = src[i * 4 + 3];
        s->data[i] = pack_color(r, g, b, a);
    }

    SDL_FreeSurface(surf);
    return s;
}

/**
 * remove a created sprite from the plane
 */
void sprite_destroy(Sprite *s) {
    if (!s) return;
    free(s->data);
    free(s);
}

// fill api (private)
Fill fill_solid(u32 color) {
    return (Fill){
        .type = FILL_SOLID,
        .color_a = color,
        .color_b = color
    };
}

Fill fill_linear(Vec2 start, Vec2 end, u32 a, u32 b) {
    return (Fill){
        .type = FILL_GRADIENT,
        .color_a = a,
        .color_b = b,
        .start = start,
        .end = end
    };
}

static u32 sample_fill(Fill fill, u16 x, u16 y) {
    // reject solid fills
    if (fill.type == FILL_SOLID) {
        return fill.color_a;
    }

    // direction vector
    f32 dx = (f32)fill.end.x - fill.start.x;
    f32 dy = (f32)fill.end.y - fill.start.y;
    f32 len2 = dx * dx + dy * dy;

    // start and end share the same point, reject
    if (len2 == 0.0f) {
        return fill.color_a;
    }

    // vector from gradient start to current pixel
    f32 px = (f32)x - fill.start.x;
    f32 py = (f32)y - fill.start.y;

    // dot product projection
    f32 t = (px * dx + py * dy) / len2;

    // truncate at 0 and 1
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    return lerp_color(fill.color_a, fill.color_b, t);
}

// draw api (public)
/**
 * draw a single pixel to the screen without checking bounds
 */
static inline void draw_pixel_unsafe(Renderer* r, u16 x, u16 y, u32 color) {
    r->back->data[(u32)y * r->width + x] = color;
}

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
void draw_rectangle(Renderer* r, u16 x, u16 y, u16 width, u16 height, Fill fill) {
    for (u16 row = y; row < y + height; row++) {
        for (u16 col = x; col < x + width; col++) {
            draw_pixel(r, col, row, sample_fill(fill, col, row));
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
static u32 edge(Vec2 a, Vec2 b, u16 px, u16 py) {
    return (u32)(b.x - a.x) * (u32)(py - a.y) - (u32)(b.y - a.y) * (u32)(px - a.x);
}

/**
 * draw a triangle given three (x, y) points and a fill.
 */
void draw_tri(Renderer* r, Vec2 a, Vec2 b, Vec2 c, Fill fill) {
    u16 minx = min3(a.x, b.x, c.x), maxx = max3(a.x, b.x, c.x);
    u16 miny = min3(a.y, b.y, c.y), maxy = max3(a.y, b.y, c.y);

    for (u16 py = miny; py <= maxy; py++) {
        for (u16 px = minx; px <= maxx; px++) {
            u32 w0 = edge(a, b, px, py);
            u32 w1 = edge(b, c, px, py);
            u32 w2 = edge(c, a, px, py);

            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) ||
                (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
                draw_pixel(r, px, py, sample_fill(fill, px, py));
            }
        }
    }
}

/**
 * draw a filled circle given a center, radius, and fill
 */
void draw_circle(Renderer* r, u16 cx, u16 cy, u16 radius, Fill fill) {
    u32 r2 = (u32)radius * (u32)radius;

    for (u32 py = cy - radius; py <= cy + radius; py++) {
        for (u32 px = cx - radius; px <= cx + radius; px++) {
            // underflow guard
            if (px < 0 || py < 0) continue;

            u32 dx = px - cx;
            u32 dy = py - cy;

            if (dx * dx + dy * dy <= r2) {
                draw_pixel(r, (u16)px, (u16)py, sample_fill(fill, (u16)px, (u16)py));
            }
        }
    }
}

void draw_char(Renderer* r, u16 x, u16 y, char c, u32 color, u8 scale) {
    if (c < 0x20 || c > 0x7E) c = '?';

    const u8* glyph = font8x8[(u8)c - 0x20];
    for (u32 row = 0; row < 8; row++) {
        for (u32 col = 0; col < 8; col++) {
            if (glyph[row] & (1 << col))
                draw_rectangle(r, x + col*scale, y + row*scale, scale, scale, fill_solid(color));
        }
    }
}

void draw_text(Renderer* r, u16 x, u16 y, const char* text, u32 color, u8 scale) {
    u16 cursor = x;
    while (*text) {
        draw_char(r, cursor, y, *text, color, scale);
        cursor += 8 * scale;
        text++;
    }
}

void draw_sprite(Renderer* r, Sprite* s, u16 x, u16 y) {
    // work through each row first
    for (u16 row = 0; row < s->height; row++) {
        u16 dy = y + row;
        if (dy >= r->height) break;

        // then column
        for (u16 col = 0; col < s->width; col++) {
            u16 dx = x + col;
            if (dx >= r->width) break;

            // grab source pixel, extract alpha (skip transparent)
            u32 src = s->data[(u32)row * s->width + col];
            u8  sa  = src & 0xFF;
            if (sa == 0) continue;

            // if fully opaque no lerp needed
            if (sa == 255)
                r->back->data[(u32)dy * r->width + dx] = src;
            
            else {
                // get color already on screen
                u32 dst = r->back->data[(u32)dy * r->width + dx];

                // lerp to that color (using 0-1 normalized alpha)
                float t = sa / 255.0f;
                r->back->data[(u32)dy * r->width + dx] = lerp_color(dst, src, t);
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
