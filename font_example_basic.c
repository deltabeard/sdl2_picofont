/**
 * This file is released under the UNLICENSE license, the terms of which are
 * provided within the LICENSE file.
 */

#include <SDL2/SDL.h>
#include "sdl2_picofont.h"

#define CHK(x)	if(!(x)) {						\
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,		\
		             "%s:%d: %s",				\
		             __func__, __LINE__, SDL_GetError());	\
		exit(EXIT_FAILURE);					\
	}

#define SDL_SetRenderSDLColour(rend,color)				\
	SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, color.a)

const SDL_Colour white = { .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF };
const SDL_Colour black = { .r = 0x00, .g = 0x00, .b = 0x00, .a = 0xFF };
const SDL_Colour skyblue = { .r = 0x87, .g = 0xCE, .b = 0xEB, .a = 0xFF };

int main(int argc, char *argv[])
{
	SDL_Window *win;
	SDL_Renderer *rend;
	font_ctx *ctx;
	SDL_Event e;

	CHK(SDL_Init(SDL_INIT_VIDEO) == 0);
	CHK(SDL_CreateWindowAndRenderer(320, 240,
	                                SDL_WINDOW_RESIZABLE,
	                                &win, &rend) == 0);
	SDL_SetWindowTitle(win, "sdl2_picofont basic example");
	CHK((ctx = FontStartup(rend)) != NULL);

	while(1)
	{
		SDL_Rect dstscale = { .x = 50, .y = 50, .h = 4, .w = 3 };

		while(SDL_PollEvent(&e) != 0)
		{
			if(e.type == SDL_QUIT)
				goto out;
		}

		SDL_SetRenderSDLColour(rend, black);
		SDL_RenderClear(rend);

		SDL_SetRenderSDLColour(rend, white);
		CHK(FontPrintToRenderer(ctx, "Hello World!", NULL) == 0);

		SDL_SetRenderSDLColour(rend, skyblue);
		CHK(FontPrintToRenderer(ctx, "I'm blue!", &dstscale) == 0);

		dstscale.y = 5;
		dstscale.x = 200;
		dstscale.h = 3;
		dstscale.w = 2;
		CHK(FontPrintToRenderer(ctx, "still blue", &dstscale) == 0);

		dstscale.y = 100;
		dstscale.h = 1;
		SDL_SetRenderSDLColour(rend, white);
		CHK(FontPrintToRenderer(ctx, "white text", &dstscale) == 0);

		SDL_RenderPresent(rend);
		SDL_Delay(15);
	}

out:
	FontExit(ctx);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}
