#include "sdl_utils.h"

void utils_label_create(utils_label_t *label, TTF_Font *font, unsigned int wrap_length, SDL_Color *color, bool has_background)
{
    label->font = font;
    label->has_background = has_background;
    label->color = color;
    label->wrap_length = wrap_length;
    label->texture = NULL;
}

void utils_label_set_content(utils_label_t *label, SDL_Renderer *renderer, const char *content)
{
    // Avoid the memory leak if the texture is already populated
    if (label->texture)
	SDL_DestroyTexture(label->texture);
    
    SDL_Surface *surface = TTF_RenderUTF8_Solid_Wrapped(label->font, content, *label->color, label->wrap_length);
    label->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    // Fetching the texture's dimensions and saving them in the labels's background rect
    SDL_QueryTexture(label->texture, NULL, NULL, &label->background.w, &label->background.h);
}

void utils_label_render(utils_label_t *label, SDL_Renderer *renderer)
{
    if (label->has_background)
    {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &label->background);
    }
    
    SDL_RenderCopy(renderer, label->texture, NULL, &label->background);
}

void utils_label_destroy(utils_label_t *label)
{
    SDL_DestroyTexture(label->texture);
}
