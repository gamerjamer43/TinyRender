#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "display.h"
#include "renderer.h"

// shared state every test gets access to each frame
typedef struct {
    Renderer* r;
    Vec2 center;
    float time; // accumulates every frame, drives all animation
} TestContext;

typedef void (*DrawFn)(TestContext* ctx);

/**
 * runs the boilerplate every test shares: window setup, main loop,
 * fps counter, teardown. each test just hands in a draw_fn that paints
 * one frame given the current time.
 */
static void run_test(const char* title, DrawFn draw_fn) {
    test_renderer();

    Renderer* r = make_renderer(800, 600);
    Display d = display_create(r, title);

    TestContext ctx = {
        .r = r,
        .center = { r->width / 2, r->height / 2 },
        .time = 0.0f
    };

    Uint32 fps_timer = SDL_GetTicks();
    int frame_count = 0;
    int fps = 0;

    while (!poll_should_quit()) {
        clear_renderer(r, COLOR_BLACK);
        draw_fn(&ctx);
        flip_renderer(r);

        // fps counter
        frame_count++;
        Uint32 now = SDL_GetTicks();
        if (now - fps_timer >= 1000) {
            char title_buf[16];
            fps = frame_count;
            snprintf(title_buf, sizeof(title_buf), "%d fps", fps);
            frame_count = 0;
            fps_timer = now;
        }

        display_present(&d, r, fps);

        // advance 0.02 "radians" of time per tick
        ctx.time += 0.02f;
    }

    display_destroy(&d);
    destroy_renderer(r);
}

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

    Fill fill = fill_linear(p[0], p[1], COLOR_RED, COLOR_BLUE);
    draw_tri(ctx->r, p[0], p[1], p[2], fill);
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

void triangle_test(void) {
    run_test("simple software renderer", draw_triangle_frame);
}

void circle_test(void) {
    run_test("simple software renderer", draw_circle_frame);
}

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    triangle_test();
    circle_test();
    return 1;
}
