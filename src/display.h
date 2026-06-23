#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "renderer.h"

/**
 * sdl display output, containing refs to window, an sdl renderer, and a texture
 */
typedef struct {
    SDL_Window*   window;
    SDL_Renderer* sdl_renderer;
    SDL_Texture*  texture;
    TTF_Font*     font;

    // fps overlay caching
    SDL_Texture*  fps_tex;
    int           fps_tex_w, fps_tex_h;
    int           fps_last;
} Display;

// display api
Display display_create(Renderer* r, const char* title);
void display_present(Display* d, Renderer* r, int fps);
void display_destroy(Display* d);

// sdl helper
int poll_should_quit();

#endif
