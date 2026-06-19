#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include "renderer.h"

// simple check to make sure buffers work fine
void sanity_check(void) {
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
    printf("sanity check passed\n");

    destroy_renderer(r);
}

// another simple test to make sure im not losing it over vcpkg
int main(int argc, char* argv[]) {
    // call the quick sanity check
    sanity_check();

    // now the sdl test
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "SDL2 Test",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        0
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int running = 1;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT) running = 0;

        SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}