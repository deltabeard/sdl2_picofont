/**
 * This file is released under the UNLICENSE license, the terms of which are
 * provided within the LICENSE file.
 */

#ifndef _FONT_H
#define _FONT_H

#include <SDL2/SDL.h>

#define FONT_CHAR_WIDTH		6
#define FONT_CHAR_HEIGHT	6

/**
 * Context required to store the generated texture with the given renderer.
 */
typedef struct font_ctx_s font_ctx;

/**
 * Initialises font context with given renderer. Must be called first.
 * \return 0 on success, negative on error (call SDL_GetError() for more
 *		information).
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
 */
void FontPrint(SDL_Renderer *rend, SDL_Texture *font_tex,
	       const char *text, const int x, const int y,
               const Uint8 width_scale, const Uint8 height_scale);

int FontPrintToRenderer(font_ctx *const ctx, const char *text, int x, int y,
	const Uint8 width_scale, const Uint8 height_scale);

SDL_Surface *FontRenderToSurface(font_ctx *const ctx, const char *text,
				 int *w, int *h);

/**
 * Deletes font context.
 */
void FontExit(font_ctx *ctx);


/**
 * The following is an example of using this SDL2 font library.
 * Error checking is omitted for brevity.
 * Compile with:
 * `gcc font.c example.c $(sdl2-config --cflags) $(sdl2-config --libs)`
 */
#if 0
#include <SDL2/SDL.h>
#include "font.h"

/* Colours in ARGB8888 format. */
#define COLOUR_WHITE		0xFFFFFFFF
#define COLOUR_GREEN	 	0xFF00FF00
#define COLOUR_TRANSPARENT 	0x00000000

int main(int argc, char *argv[])
{
	SDL_Window *win;
	SDL_Renderer *rend;
	font_ctx white;
	font_ctx green;

	(void) argc;
	(void) argv;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(320, 240, 0, &win, &rend);
	FontStartup(&white, rend, COLOUR_WHITE, COLOUR_TRANSPARENT);
	FontStartup(&green, rend, COLOUR_GREEN, COLOUR_TRANSPARENT);
	FontPrint(&white, "Hello World!", 0, 0, 2, 2);
	FontPrint(&white, "You can print me anywhere.", 10, 90, 2, 3);
	FontPrint(&green, "little green", 0, 160, 1, 1);
	FontPrint(&green, "BIG GREEN", 80, 170, 4, 12);
	SDL_RenderPresent(rend);
	SDL_Delay(3000);

	FontExit(&white);
	FontExit(&green);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}
#endif

#endif
