#ifndef _SDL_UTILS_H
#define _SDL_UTILS_H

#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

typedef struct
{
    TTF_Font *font;
    SDL_Texture *texture;
    SDL_Rect background;
    unsigned int wrap_length;
    bool has_background;
    SDL_Color *color;
} utils_label_t;

// Wrap length restricts the text's width to a certain number and wraps long sentences
// into their own, separate lines
void utils_label_create(utils_label_t *label, TTF_Font *font, unsigned int wrap_length, SDL_Color *background_color, bool has_background);

void utils_label_set_content(utils_label_t *label, SDL_Renderer *renderer, const char *content);
void utils_label_render(utils_label_t *label, SDL_Renderer *renderer);
void utils_label_destroy(utils_label_t *label);

#endif
