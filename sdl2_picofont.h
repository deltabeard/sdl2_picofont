/**
 * This file is released under the UNLICENSE license, the terms of which are
 * provided within the LICENSE file.
 */

#pragma once

#include <SDL2/SDL.h>

/**
 * Font to be selected at compile time. Options are:
 * PICOFONT_9x15 (default)
 * PICOFONT_6x5
 */
#define PICOFONT_9x15	0
#define PICOFONT_6x5	1

#define PICOFONT_SELECTION PICOFONT_9x15
//#define PICOFONT_SELECTION PICOFONT_5x6

/**
 * Context required to store the generated texture with the given renderer.
 */
typedef struct font_ctx_s font_ctx;

/**
 * Initialises font context with given renderer. Must be called first.
 * The given renderer must remain valid until after FontExit() is called.
 *
 * \return 0 on success, else error. Use SDL_GetError().
 */
font_ctx *FontStartup(SDL_Renderer *renderer);

/**
 * Prints a string to the SDL2 renderer.
 *
 * \param ctx	Font library context.
 * \param text	Text to print.
 * \param x	X co-ordinate to start printing text.
 * \param y	Y co-ordinate to start printing text.
 * \param width_scale	Width scale of font. Must be larger than 0.
 * \param height_scale	Height scale of font. Must be larger than 0.
 * \param colour Set text colour.
 * \return	0 on success, else error. Use SDL_GetError().
 */
int FontPrintToRenderer(font_ctx *const ctx, const char *text, int x, int y,
			const Uint8 width_scale, const Uint8 height_scale,
			const SDL_Colour colour);

/**
 * Renders the given string to a surface.
 *
 * \param ctx	Font library context.
 * \param text	Text to print.
 * \param w	If not NULL, the width of the created surface is stored in this
 * 		pointer.
 * \param h	If not NULL, the height of the created surface is stored in this
 * 		pointer.
 * \return	Rendered surface or NULL on error. Use SDL_GetError().
 */
SDL_Surface *FontRenderToSurface(font_ctx *const ctx, const char *text,
				 int *w, int *h);

/**
 * Renders the given string to a texture.
 *
 * \param ctx	Font library context.
 * \param text	Text to print.
 * \param w	If not NULL, the width of the created texture is stored in this
 * 		pointer.
 * \param h	If not NULL, the height of the created texture is stored in this
 * 		pointer.
 *  \return	Rendered texture or NULL on error. Use SDL_GetError().
 */
SDL_Texture *FontRenderToTexture(font_ctx *const ctx, const char *text,
				 int *w, int *h);

/**
 * Deletes font context.
 */
void FontExit(font_ctx *ctx);
