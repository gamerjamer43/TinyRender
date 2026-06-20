#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "display.h"
#include "renderer.h"

int main(void) {
    // test just to confirm
    test_renderer();

    Renderer* r = make_renderer(800, 600);
    Display d = display_create(r, "simple software renderer");

    float angle = 0.0f;    // current angle
    float radius = 150.0f; // each vertex sits 150px from the center

    // center of the screen
    Vec2 center = { r->width / 2, r->height / 2 };

    // for fps counter
    Uint32 fps_timer = SDL_GetTicks();
    int frame_count = 0;
    int fps = 0;

    while (!poll_should_quit()) {
        clear_renderer(r, COLOR_BLACK);

        // 3 vertices for the triangle
        Vec2 p[3];

        for (int i = 0; i < 3; i++) {
            // full circle is 2pi radians, divided into 3 parts (rotation repeats when 1/3 of a turn is reached)
            // all 3 vertices stay 120 degrees apart
            float a = angle + i * (2.0f * (float)M_PI / 3.0f);

            // parametric circles!!! i remembered something from calc!
            p[i].x = (u16)(center.x + cosf(a) * radius);
            p[i].y = (u16)(center.y + sinf(a) * radius);
        }

        draw_tri(r, p[0], p[1], p[2], COLOR_RED);
        flip_renderer(r);

        // fps counter
        frame_count++;
        Uint32 now = SDL_GetTicks();
        if (now - fps_timer >= 1000) {
            char title[16];
            fps = frame_count;
            snprintf(title, sizeof(title), "%d fps", fps);
            SDL_SetWindowTitle(d.window, title);
            frame_count = 0;
            fps_timer = now;
        }

        display_present(&d, r, fps);
        
        // rotate 0.02 radians per tick
        angle += 0.02f;
    }

    display_destroy(&d);
    destroy_renderer(r);
    return 0;
}
