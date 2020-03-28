/**
 * This file is released under the UNLICENSE license, the terms of which are
 * provided within the LICENSE file.
 */

#include <SDL2/SDL.h>

#include "sdl2_picofont.h"

#define FONT_BITMAP_WIDTH	48
#define FONT_BITMAP_HEIGHT 	72
#define FONT_COLUMNS		8
#define FONT_PITCH		6

struct font_ctx_s {
    SDL_Texture *tex;
    SDL_Renderer *rend;
    SDL_Surface *surf;
    Uint8 pixels[FONT_BITMAP_HEIGHT][FONT_PITCH];
};

/**
 * 6 x 6 bitmap font for characters ' ' to '~', where each 64-bit value is a
 * line to be on the generated 48x72 font sheet in FontStartup().
 */
static const Uint8 bitmap_font[FONT_BITMAP_HEIGHT][FONT_PITCH] =
{
    { 0x00, 0x42, 0x8A, 0x3D, 0x91, 0x04 },
    { 0x00, 0x42, 0x9F, 0x51, 0xA2, 0x88 },
    { 0x00, 0x40, 0x0A, 0x38, 0x41, 0x00 },
    { 0x00, 0x00, 0x1F, 0x14, 0xB2, 0x80 },
    { 0x00, 0x40, 0x0A, 0x79, 0x31, 0x40 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x08, 0x45, 0x40, 0x00, 0x00, 0x01 },
    { 0x10, 0x23, 0x84, 0x00, 0x00, 0x02 },
    { 0x10, 0x27, 0xCE, 0x00, 0xE0, 0x04 },
    { 0x10, 0x23, 0x84, 0x10, 0x00, 0x08 },
    { 0x08, 0x45, 0x40, 0x20, 0x01, 0x10 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x38, 0x41, 0x8C, 0x18, 0xE1, 0x0F },
    { 0x4C, 0xC2, 0x42, 0x28, 0x82, 0x01 },
    { 0x54, 0x40, 0x84, 0x3C, 0xC7, 0x02 },
    { 0x64, 0x41, 0x02, 0x08, 0x24, 0x84 },
    { 0x38, 0xE3, 0xCC, 0x08, 0xC3, 0x04 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x30, 0xC0, 0x00, 0x08, 0x02, 0x0C },
    { 0x49, 0x21, 0x04, 0x10, 0xE1, 0x02 },
    { 0x30, 0xE0, 0x00, 0x20, 0x00, 0x84 },
    { 0x48, 0x21, 0x04, 0x10, 0xE1, 0x00 },
    { 0x30, 0x20, 0x08, 0x08, 0x02, 0x04 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x38, 0xE7, 0x8F, 0x79, 0xF7, 0xCF },
    { 0x05, 0x14, 0x50, 0x45, 0x04, 0x10 },
    { 0x35, 0xF7, 0x90, 0x45, 0xE7, 0x93 },
    { 0x55, 0x14, 0x50, 0x45, 0x04, 0x11 },
    { 0x39, 0x17, 0x8F, 0x79, 0xF4, 0x0F },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x44, 0xE1, 0xD2, 0x41, 0x14, 0x4E },
    { 0x44, 0x40, 0x94, 0x41, 0xB6, 0x51 },
    { 0x7C, 0x40, 0x98, 0x41, 0x55, 0x51 },
    { 0x44, 0x44, 0x94, 0x41, 0x14, 0xD1 },
    { 0x44, 0xE3, 0x12, 0x79, 0x14, 0x4E },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x78, 0xE7, 0x8F, 0x7D, 0x14, 0x51 },
    { 0x45, 0x14, 0x50, 0x11, 0x14, 0x51 },
    { 0x79, 0x17, 0x8E, 0x11, 0x14, 0x55 },
    { 0x41, 0x25, 0x01, 0x11, 0x12, 0x95 },
    { 0x40, 0xD4, 0x9E, 0x10, 0xE1, 0x0A },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x45, 0x13, 0x8E, 0x40, 0xE1, 0x00 },
    { 0x28, 0xA0, 0x88, 0x20, 0x22, 0x80 },
    { 0x10, 0x41, 0x08, 0x10, 0x20, 0x00 },
    { 0x28, 0x42, 0x08, 0x08, 0x20, 0x00 },
    { 0x44, 0x43, 0x8E, 0x04, 0xE0, 0x1F },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x10, 0x04, 0x00, 0x08, 0x00, 0x86 },
    { 0x08, 0xE7, 0x06, 0x08, 0x41, 0x0A },
    { 0x01, 0x24, 0x88, 0x38, 0xA3, 0x86 },
    { 0x01, 0x24, 0x88, 0x48, 0xC1, 0x02 },
    { 0x00, 0xD7, 0x06, 0x38, 0x61, 0x0C },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x20, 0x40, 0x88, 0x10, 0x00, 0x00 },
    { 0x20, 0x00, 0x08, 0x11, 0xA3, 0x04 },
    { 0x38, 0x40, 0x8A, 0x11, 0x52, 0x8A },
    { 0x24, 0x42, 0x8C, 0x11, 0x12, 0x8A },
    { 0x24, 0x41, 0x0A, 0x11, 0x12, 0x84 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x30, 0x62, 0x86, 0x38, 0x92, 0x91 },
    { 0x28, 0xA3, 0x04, 0x10, 0x92, 0x95 },
    { 0x30, 0x62, 0x02, 0x10, 0x92, 0x95 },
    { 0x20, 0x22, 0x06, 0x08, 0x61, 0x0A },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x02, 0x10, 0x80, 0x00 },
    { 0x28, 0xA1, 0x84, 0x10, 0x42, 0x80 },
    { 0x10, 0xA0, 0x8C, 0x00, 0x65, 0x00 },
    { 0x10, 0x41, 0x04, 0x10, 0x40, 0x00 },
    { 0x28, 0x81, 0x82, 0x10, 0x80, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

font_ctx *FontStartup(SDL_Renderer *renderer)
{
    const SDL_Colour colours[2] =
    {
        { .r = 0x00, .g = 0x00, .b = 0x00, .a = 0xFF }, // BG
        { .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF }  // FG
    };
    font_ctx* ctx = malloc(sizeof(font_ctx));

    if(ctx == NULL)
    {
        SDL_SetError("Unable to allocate memory.");
        goto out;
    }

    memcpy(ctx->pixels, bitmap_font, sizeof(bitmap_font));

    ctx->surf = SDL_CreateRGBSurfaceFrom(ctx->pixels,
                                         48, 72, 1, 6, 0, 0, 0, 0);

    if(ctx->surf == NULL)
        goto err;

    if(SDL_SetPaletteColors(ctx->surf->format->palette, colours, 0, 2) < 0)
        goto err;

    if(SDL_LockSurface(ctx->surf) < 0)
        goto err;

    SDL_UnlockSurface(ctx->surf);
    //SDL_SetSurfaceRLE(ctx->surf, 1);

    ctx->tex = SDL_CreateTextureFromSurface(renderer, ctx->surf);
    if(ctx->tex == NULL)
        goto err;

    ctx->rend = renderer;

out:
    return ctx;

err:
    free(ctx);
    ctx = NULL;
    goto out;
}

int FontPrintToRenderer(font_ctx *const ctx, const char *text, int x, int y,
                        const Uint8 width_scale, const Uint8 height_scale)
{
    SDL_Rect font_rect, screen_rect;
    SDL_assert(ctx != NULL);
    SDL_assert(text != NULL);

    font_rect.w = FONT_CHAR_WIDTH;
    font_rect.h = FONT_CHAR_HEIGHT;

    screen_rect.w = FONT_CHAR_WIDTH * width_scale;
    screen_rect.h = FONT_CHAR_HEIGHT * height_scale;
    screen_rect.x = x;
    screen_rect.y = y;

    SDL_SetTextureColorMod(ctx->tex, 0xFF, 0x00, 0x00);

    for(; *text; text++)
    {
        Uint8 pos = *text - ' ';
        int ret;

        font_rect.x = (pos % FONT_COLUMNS) * FONT_CHAR_WIDTH;
        font_rect.y = (pos / FONT_COLUMNS) * FONT_CHAR_HEIGHT;

        ret = SDL_RenderCopy(ctx->rend, ctx->tex,
                             &font_rect, &screen_rect);
        if(ret < 0)
            return ret;

        screen_rect.x += screen_rect.w;
    }

    return 0;
}

SDL_Surface *FontRenderToSurface(font_ctx *const ctx, const char *text,
                                 int *w, int *h)
{
    SDL_Rect font_rect, screen_rect;
    int w_max;
    SDL_Surface *render;
    size_t len = strlen(text);

    SDL_assert(ctx != NULL);
    SDL_assert(text != NULL);

    font_rect.w = FONT_CHAR_WIDTH;
    font_rect.h = FONT_CHAR_HEIGHT;

    screen_rect.w = FONT_CHAR_WIDTH;
    screen_rect.h = FONT_CHAR_HEIGHT;
    screen_rect.x = 0;
    screen_rect.y = 0;

    w_max = len * FONT_CHAR_WIDTH;

    render = SDL_CreateRGBSurfaceWithFormat(0, w_max, FONT_CHAR_HEIGHT, 32,
                                            SDL_PIXELFORMAT_ARGB8888);

    if(render == NULL)
        return render;

    //SDL_SetSurfaceRLE(render, 1);

    for(; *text; text++)
    {
        Uint8 pos = *text - ' ';
        int ret;

        SDL_assert(*text >= ' ');
        SDL_assert(*text <= '~');

        font_rect.x = (pos % FONT_COLUMNS) * FONT_CHAR_WIDTH;
        font_rect.y = (pos / FONT_COLUMNS) * FONT_CHAR_HEIGHT;

        ret = SDL_BlitSurface(ctx->surf, &font_rect,
                              render, &screen_rect);
        if(ret < 0)
        {
            SDL_FreeSurface(render);
            return NULL;
        }

        screen_rect.x += FONT_CHAR_WIDTH;
    }

    if(w != NULL)
        *w = screen_rect.x;

    if(h != NULL)
        *h = FONT_CHAR_HEIGHT;

    return render;
}

void FontExit(font_ctx *ctx)
{
    SDL_FreeSurface(ctx->surf);
    free(ctx->pixels);
}



