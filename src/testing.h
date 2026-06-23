#ifndef TESTING_H
#define TESTING_H

#include "renderer.h"
#include "display.h"

// shared state every test gets access to each frame
typedef struct {
    Renderer* r;
    Vec2 center;
    f32 time;
} TestContext;

typedef void (*DrawFn)(TestContext* ctx);

/**
 * runs the boilerplate every test shares: window setup, main loop,
 * fps counter, teardown. each test just hands in a draw_fn that paints
 * one frame given the current time.
 */
static void run_test(const char* title, Vec2 resolution, DrawFn draw_fn) {
    test_renderer();

    Renderer* r = make_renderer(resolution.x, resolution.y);
    Display d = display_create(r, title);
    // r->fps = 165; // uncomment to cap fps

    TestContext ctx = {
        .r = r,
        .center = { r->width / 2, r->height / 2 },
        .time = 0.0f
    };

    u64 perf_freq = SDL_GetPerformanceFrequency();
    u32 fps_timer = SDL_GetTicks();
    u32 frame_count = 0;
    u32 fps = 0;

    while (!poll_should_quit()) {
        u64 frame_start = SDL_GetPerformanceCounter();

        clear_renderer(r, COLOR_BLACK);
        draw_fn(&ctx);
        flip_renderer(r);

        frame_count++;
        u32 now = SDL_GetTicks();
        if (now - fps_timer >= 1000) {
            fps = frame_count;
            frame_count = 0;
            fps_timer = now;
        }

        display_present(&d, r, fps);

        if (r->fps > 0) {
            u64 target = perf_freq / r->fps;
            u64 elapsed = SDL_GetPerformanceCounter() - frame_start;
            if (elapsed < target) {
                u32 sleep_ms = (u32)((target - elapsed) * 1000 / perf_freq);
                if (sleep_ms > 2)
                    SDL_Delay(sleep_ms - 2);
                while (SDL_GetPerformanceCounter() - frame_start < target) {}
            }
        }

        ctx.time += 0.02f;
    }

    display_destroy(&d);
    destroy_renderer(r);
}

#endif