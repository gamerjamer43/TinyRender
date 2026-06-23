#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "display.h"
#include "renderer.h"
#include "testing.h"

// setup
const char* TITLE = "simple software renderer";
const Vec2 RESOLUTION = { 800, 600 };

/**
 * draws a triangle with 3 vertices orbiting the center, 120 degrees apart
 */
static void draw_triangle_frame(TestContext* ctx) {
    float radius = 150.0f; // each vertex sits 150px from the center
    Vec2 p[3];

    for (int i = 0; i < 3; i++) {
        // full circle is 2pi radians, divided into 3 parts (rotation repeats when 1/3 of a turn is reached)
        float a = ctx->time + i * (2.0f * (float)M_PI / 3.0f);

        // parametric circles!!! i remembered something from calc!
        p[i].x = (u16)(ctx->center.x + cosf(a) * radius);
        p[i].y = (u16)(ctx->center.y + sinf(a) * radius);
    }

    Fill fill = fill_linear(p[0], p[1], COLOR_RED, pack_color(150, 20, 20, 255));
    draw_tri(ctx->r, p[0], p[1], p[2], fill);
}

/**
 * helper for the rainbow traingle test
 */
static u32 rainbow_color(float hue) {
    float phase = hue * 2.0f * (float)M_PI;
    return pack_color(
        (u8)((sinf(phase) + 1.0f) * 127.5f),
        (u8)((sinf(phase + 2.0f * (float)M_PI / 3.0f) + 1.0f) * 127.5f),
        (u8)((sinf(phase + 4.0f * (float)M_PI / 3.0f) + 1.0f) * 127.5f),
        255
    );
}

/**
 * draws a fixed triangle with a slowly phasing rainbow gradient
 */
static void draw_rainbow_triangle_frame(TestContext* ctx) {
    u16 top = (u16)(ctx->center.y - 170);
    u16 bottom = (u16)(ctx->center.y + 140);
    float half_width = 180.0f;
    float phase = ctx->time * 0.04f;

    for (u16 y = top; y <= bottom; y++) {
        float t = (float)(y - top) / (float)(bottom - top);
        u16 left = (u16)(ctx->center.x - half_width * t);
        u16 right = (u16)(ctx->center.x + half_width * t);

        for (u16 x = left; x <= right; x++) {
            float hue = ((float)(x - left) / (float)(right - left)) + phase;
            draw_pixel(ctx->r, x, y, rainbow_color(hue));
        }
    }
}

/**
 * draws a circle at the center that pulsates in radius over time
 */
static void draw_circle_frame(TestContext* ctx) {
    float base_radius = 60.0f;  // resting radius
    float pulse_amount = 20.0f; // grow/shrink amount

    // radius + sin(dt) * pulse gives a nice infinite grow/shrink
    u16 radius = (u16)(base_radius + sinf(ctx->time) * pulse_amount);
    Fill fill = fill_linear(
        (Vec2){ ctx->center.x, (u16)(ctx->center.y - radius) },
        (Vec2){ ctx->center.x, (u16)(ctx->center.y + radius) },
        COLOR_RED,
        COLOR_BLUE
    );
    draw_circle(ctx->r, (u16)ctx->center.x, (u16)ctx->center.y, radius, fill);
}

/**
 * draws hello world to the center of the screen, and cycles its color
 */
static void draw_text_frame(TestContext* ctx) {
    u8 scale = 3;
    int len = strlen(TITLE);

    // center offset
    u16 x = ctx->center.x - (len * 8 * scale) / 2;
    u16 y = ctx->center.y - (8 * scale) / 2;

    for (int i = 0; i < len; i++) {
        // shift over time
        float hue = ((float)i / len) + ctx->time * 0.04f;
        draw_char(ctx->r, x + i * 8 * scale, y, TITLE[i], rainbow_color(hue), scale);
    }
}

void triangle_test(void) {
    run_test(TITLE, RESOLUTION, draw_triangle_frame);
}

void circle_test(void) {
    run_test(TITLE, RESOLUTION, draw_circle_frame);
}

void rainbow_triangle_test(void) {
    run_test(TITLE, RESOLUTION, draw_rainbow_triangle_frame);
}

void rainbow_text_test(void) {
    run_test(TITLE, RESOLUTION, draw_text_frame);
}

int main(void) {
    // triangle_test();
    // circle_test();
    // rainbow_triangle_test();
    rainbow_text_test();
    return 1;
}
