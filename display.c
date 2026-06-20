#include "display.h"
#include "renderer.h"

Display display_create(Renderer* r, const char* title) {
    SDL_Init(SDL_INIT_VIDEO);
    Display d;

    // create a centered window of proper width and height
    d.window = SDL_CreateWindow(title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        r->width, r->height, 0);

    // create a renderer with whatever driver you can
    // hardware acceleration and vsync enabled
    d.sdl_renderer = SDL_CreateRenderer(d.window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    d.texture = SDL_CreateTexture(d.sdl_renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
        r->width, r->height);

    return d;
}

// copy data from renderer to sdl
void display_present(Display* d, Renderer* r) {
    SDL_UpdateTexture(d->texture, NULL, r->front->data, r->width * sizeof(u32));
    SDL_RenderCopy(d->sdl_renderer, d->texture, NULL, NULL);
    SDL_RenderPresent(d->sdl_renderer);
}

// free helper for display
void display_destroy(Display* d) {
    SDL_DestroyTexture(d->texture);
    SDL_DestroyRenderer(d->sdl_renderer);
    SDL_DestroyWindow(d->window);
    SDL_Quit();
}

// sdl helper for quit polling
int poll_should_quit() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) return 1;
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) return 1;
    }
    return 0;
}