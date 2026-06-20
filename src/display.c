#include "display.h"
#include "renderer.h"
#include <stdio.h>

static TTF_Font* display_open_font(void) {
    const char* paths[] = {
        "DejaVuSans.ttf",
        "C:\\Windows\\Fonts\\segoeui.ttf",
        "C:\\Windows\\Fonts\\arial.ttf",
    };

    for (int i = 0; i < (int)(sizeof(paths) / sizeof(paths[0])); i++) {
        TTF_Font* font = TTF_OpenFont(paths[i], 20);
        if (font) return font;
    }

    fprintf(stderr, "warning: could not load FPS font: %s\n", TTF_GetError());
    return NULL;
}

Display display_create(Renderer* r, const char* title) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    }

    if (TTF_Init() != 0) {
        fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
    }

    Display d = {0};

    // create a centered window of proper width and height
    d.window = SDL_CreateWindow(title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        r->width, r->height, 0);

    // create a renderer with whatever driver you can
    // hardware acceleration and vsync enabled
    d.sdl_renderer = SDL_CreateRenderer(d.window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // texture defaults to RGBA8888 with a stream based access
    d.texture = SDL_CreateTexture(d.sdl_renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
        r->width, r->height);

    d.font = display_open_font();

    return d;
}

// draw an fps counter on the screen
static void display_draw_fps(Display* d, Renderer* r, int fps) {
    // convert int to string
    char buf[16];
    snprintf(buf, sizeof(buf), "%d fps", fps);

    // create a white text texture
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surf = TTF_RenderText_Blended(d->font, buf, white);
    SDL_Texture* text_tex = SDL_CreateTextureFromSurface(d->sdl_renderer, surf);

    // copy texture to renderer
    SDL_Rect dst = { r->width - surf->w - 10, 10, surf->w, surf->h };
    SDL_RenderCopy(d->sdl_renderer, text_tex, NULL, &dst);

    // destroy when done
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(text_tex);
}

// copy data from renderer to sdl
void display_present(Display* d, Renderer* r, int fps) {
    SDL_UpdateTexture(d->texture, NULL, r->front->data, r->width * sizeof(u32));
    SDL_RenderCopy(d->sdl_renderer, d->texture, NULL, NULL);

    display_draw_fps(d, r, fps);

    SDL_RenderPresent(d->sdl_renderer);
}

// free helper for display
void display_destroy(Display* d) {
    if (d->font) TTF_CloseFont(d->font);
    
    SDL_DestroyTexture(d->texture);
    SDL_DestroyRenderer(d->sdl_renderer);
    SDL_DestroyWindow(d->window);

    TTF_Quit();
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
