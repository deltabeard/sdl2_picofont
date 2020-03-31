/**
 * This file is released under the UNLICENSE license, the terms of which are
 * provided within the LICENSE file.
 */

#include <SDL2/SDL.h>

#include "sdl2_picofont.h"

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
	SDL_Renderer *rend;
	SDL_Surface *surf;
};

font_ctx *FontStartup(SDL_Renderer *renderer)
{
	const SDL_Colour colours[2] =
	{
		{ .r = 0x00, .g = 0x00, .b = 0x00, .a = 0x00 }, // BG
		{ .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0x00 }  // FG
	};
	SDL_Surface *bmp_surf;
	font_ctx *ctx = malloc(sizeof(font_ctx));
	Uint32 tex_format;
	Uint8 *pixels = malloc(FONT_BITMAP_SIZE);;

	if(ctx == NULL || pixels == NULL)
	{
		SDL_SetError("Unable to allocate memory.");
		goto err;
	}

	memcpy(pixels, bitmap_font, FONT_BITMAP_SIZE);
	bmp_surf = SDL_CreateRGBSurfaceFrom(pixels,
	                                    FONT_BITMAP_WIDTH,
	                                    FONT_BITMAP_HEIGHT,
	                                    1, FONT_PITCH, 0, 0, 0, 0);
	if(bmp_surf == NULL)
		goto err;

	if(SDL_SetPaletteColors(bmp_surf->format->palette, colours, 0, 2) < 0)
		goto err;

	SDL_UnlockSurface(bmp_surf);
	SDL_SetColorKey(bmp_surf, SDL_TRUE, 0);

	ctx->tex = SDL_CreateTextureFromSurface(renderer, bmp_surf);
	if(ctx->tex == NULL)
		goto err;

	SDL_QueryTexture(ctx->tex, &tex_format, NULL, NULL, NULL);

	/**
	 * Converting to native format used by textures as there is a bug in
	 * SDL2 whereby 1bpp palette surfaces can not be blitted to ARGB8888
	 * surfaces properly.
	 */
	ctx->surf = SDL_ConvertSurfaceFormat(bmp_surf, tex_format, 0);
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
	free(pixels);
	return ctx;

err:
	free(ctx);
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

SDL_Surface *FontRenderToSurface(font_ctx *const ctx, const char *text,
                                 int *w, int *h)
{
	SDL_Rect font_rect, screen_rect;
	int w_max;
	SDL_Surface *render;
	size_t len = strlen(text);
	Uint32 tex_format;

	SDL_assert(ctx != NULL);
	SDL_assert(text != NULL);

	font_rect.w = FONT_CHAR_WIDTH;
	font_rect.h = FONT_CHAR_HEIGHT;

	screen_rect.w = FONT_CHAR_WIDTH;
	screen_rect.h = FONT_CHAR_HEIGHT;
	screen_rect.x = 0;
	screen_rect.y = 0;

	w_max = len * FONT_CHAR_WIDTH;

	if(SDL_QueryTexture(ctx->tex, &tex_format, NULL, NULL, NULL) < 0)
		tex_format = SDL_PIXELFORMAT_ARGB8888;

	/* FIXME: get depth instead of setting it to 32 here. */
	render = SDL_CreateRGBSurfaceWithFormat(0, w_max, FONT_CHAR_HEIGHT, 32,
	                                        tex_format);
	if(render == NULL)
		return render;

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
	return tex;
}

void FontExit(font_ctx *ctx)
{
	SDL_FreeSurface(ctx->surf);
	SDL_DestroyTexture(ctx->tex);
	free(ctx);
}
