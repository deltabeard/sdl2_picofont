/**
 * This file is released under the UNLICENSE license, the terms of which are
 * provided within the LICENSE file.
 */

#include <SDL2/SDL.h>

#include "sdl2_picofont.h"

/**
 * You may want to define the fallback format as SDL_PIXELFORMAT_RGB565 for low
 * power hardware. Although this will format will only be used if
 * SDL_QueryTexture() fails for some reason (which is unlikely).
 */
#ifndef PICOFONT_FALLBACK_FORMAT
#define PICOFONT_FALLBACK_FORMAT SDL_PIXELFORMAT_ARGB8888
#endif

#if defined(PICOFONT_5x8)
#include "5x8.h"
#elif defined(PICOFONT_9x15)
#include "9x15.h"
#else
#include "9x15.h"
#warning "No font specified. Using 9x15 font by default."
#endif

struct font_ctx_s
{
	SDL_Texture *tex;
	/* FIXME: Don't use renderer. */
	SDL_Renderer *rend;
	/* FIXME: Don't use surface. */
	SDL_Surface *surf;
	Uint32 format;
};

font_ctx *FontStartup(SDL_Renderer *renderer)
{
	const SDL_Colour colours[2] =
	{
		{ .r = 0x00, .g = 0x00, .b = 0x00, .a = 0x00 }, // BG
		{ .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0x00 }  // FG
	};
	SDL_Surface *bmp_surf;
	font_ctx *ctx = SDL_malloc(sizeof(font_ctx));
	Uint8 *pixels = SDL_malloc(FONT_BITMAP_SIZE);;

	if(ctx == NULL || pixels == NULL)
	{
		SDL_SetError("Unable to allocate memory.");
		goto err;
	}

	SDL_memcpy(pixels, bitmap_font, FONT_BITMAP_SIZE);
	bmp_surf = SDL_CreateRGBSurfaceFrom(pixels,
	                                    FONT_BITMAP_WIDTH,
	                                    FONT_BITMAP_HEIGHT,
	                                    1, FONT_PITCH, 0, 0, 0, 0);
	if(bmp_surf == NULL)
		goto err;

	if(SDL_SetPaletteColors(bmp_surf->format->palette, colours, 0, 2) < 0)
	{
		SDL_FreeSurface(bmp_surf);
		goto err;
	}

	if(SDL_SetColorKey(bmp_surf, SDL_TRUE, 0) < 0)
	{
		SDL_FreeSurface(bmp_surf);
		goto err;
	}

	ctx->tex = SDL_CreateTextureFromSurface(renderer, bmp_surf);
	if(ctx->tex == NULL)
	{
		SDL_FreeSurface(bmp_surf);
		goto err;
	}

	if(SDL_QueryTexture(ctx->tex, &ctx->format, NULL, NULL, NULL) < 0)
		ctx->format = PICOFONT_FALLBACK_FORMAT;

	/* Converting to native format used by textures. */
	ctx->surf = SDL_ConvertSurfaceFormat(bmp_surf, ctx->format, 0);
	if(ctx->surf == NULL)
	{
		SDL_DestroyTexture(ctx->tex);
		SDL_FreeSurface(bmp_surf);
		goto err;
	}

	SDL_SetColorKey(ctx->surf, SDL_TRUE, 0x000000);
	SDL_UnlockSurface(ctx->surf);
	ctx->rend = renderer;

	SDL_FreeSurface(bmp_surf);

out:
	SDL_free(pixels);
	return ctx;

err:
	SDL_free(ctx);
	ctx = NULL;
	goto out;
}

int FontPrintToRenderer(font_ctx *const ctx, const char *text, int x, int y,
                        const Uint8 width_scale, const Uint8 height_scale,
                        const SDL_Colour colour)
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

	SDL_SetTextureColorMod(ctx->tex, colour.r, colour.g, colour.b);

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

/* TODO: Create renderer from surface then pass to FontPrintToRenderer(). */
SDL_Surface *FontRenderToSurface(font_ctx *const ctx, const char *text,
                                 int *w, int *h)
{
	SDL_Rect font_rect, screen_rect;
	int w_max;
	SDL_Surface *render;
	size_t len = SDL_strlen(text);

	SDL_assert(ctx != NULL);
	SDL_assert(text != NULL);

	font_rect.w = FONT_CHAR_WIDTH;
	font_rect.h = FONT_CHAR_HEIGHT;

	screen_rect.w = FONT_CHAR_WIDTH;
	screen_rect.h = FONT_CHAR_HEIGHT;
	screen_rect.x = 0;
	screen_rect.y = 0;

	w_max = len * FONT_CHAR_WIDTH;
	/* Error if string is too long. */
	if(w_max < 0)
	{
		SDL_SetError("Input string was too long.");
		return NULL;
	}

	render = SDL_CreateRGBSurfaceWithFormat(0, w_max, FONT_CHAR_HEIGHT,
						SDL_BITSPERPIXEL(ctx->format),
	                                        ctx->format);
	if(render == NULL)
		return NULL;

	SDL_SetColorKey(render, SDL_TRUE, 0x000000);

	for(; *text; text++)
	{
		Uint8 pos;
		int ret;

		if(*text >= ' ' && *text <= '~')
			pos = *text - ' ';
		else
			pos = '?' - ' ';

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

	SDL_assert(*w == w_max);

	return render;
}

SDL_Texture *FontRenderToTexture(font_ctx *const ctx, const char *text,
                                 int *w, int *h)
{
	SDL_Surface *surf;
	SDL_Texture *tex;

	surf = FontRenderToSurface(ctx, text, w, h);
	if(surf == NULL)
		return NULL;

	tex = SDL_CreateTextureFromSurface(ctx->rend, surf);
	SDL_FreeSurface(surf);
	return tex;
}

void FontExit(font_ctx *ctx)
{
	SDL_FreeSurface(ctx->surf);
	SDL_DestroyTexture(ctx->tex);
	SDL_free(ctx);
}
