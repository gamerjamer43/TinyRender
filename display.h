#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>

/**
 * sdl display output, containing refs to window, an sdl renderer (ig it makes no sense to write my own on top of this)
 * and a texture. texture defaults to RGBA8888 with a stream based access
 */
typedef struct {
    SDL_Window*   window;
    SDL_Renderer* sdl_renderer;
    SDL_Texture*  texture;
} Display;

// display api
static Display display_create(Renderer* r, const char* title);
static void display_present(Display* d, Renderer* r);
static void display_destroy(Display* d);

// sdl helper
static int poll_should_quit();

#endif