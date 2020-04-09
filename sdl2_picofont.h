/**
 * This file is released under the UNLICENSE license, the terms of which are
 * provided within the LICENSE file.
 */

#pragma once

#include <SDL2/SDL.h>

/**
 * Font to be selected at compile time. Options are:
 * PICOFONT_9x15 (default)
 * PICOFONT_5x8
 */

/**
 * Context required to store the generated texture with the given renderer.
 */
typedef struct font_ctx_s font_ctx;

/**
 * Initialises font context with given renderer. Must be called first.
 * The given renderer must remain valid until after FontExit() is called.
 *
 * \param renderer	Renderer of the window.
 * \return 0 on success, else error. Use SDL_GetError().
 */
font_ctx *FontStartup(SDL_Renderer *renderer);

/**
 * Prints a string to the SDL2 renderer.
 * Use SDL_SetRenderDrawColor() to change the text colour.
 * Use SDL_SetRenderTarget() to render to a texture instead.
 *
 * \param ctx	Font library context.
 * \param text	Text to print.
 * \param dstscale	Location and scale of text. (0,0) x1 if NULL.
 * \return	0 on success, else error. Use SDL_GetError().
 */
int FontPrintToRenderer(font_ctx *const ctx, const char *text,
			const SDL_Rect *dstscale);

/**
 * Expected size of texture/renderer required given the string length, assuming
 * a scale of 1.
 *
 * \param text_len	Length of text.
 * \param w		Pointer to store expected width.
 * \param h		Pointer to store expected height.
 */
void FontDrawSize(const size_t text_len, int *w, int *h);

/**
 * Deletes font context.
 */
void FontExit(font_ctx *ctx);
