#include <SDL2/SDL.h>
#include "sdl2_picofont.h"

/* Colours in ARGB8888 format. */
#define COLOUR_WHITE		0xFFFFFFFF
#define COLOUR_GREEN	 	0xFF00FF00
#define COLOUR_TRANSPARENT 	0x00000000

#define CHK(x)	if(x) {					\
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,	\
			"%s:%d: %s", __func__, __LINE__, SDL_GetError()); \
		exit(EXIT_FAILURE);				\
		}

int main(int argc, char *argv[])
{
	SDL_Window *win;
	SDL_Renderer *rend;
	SDL_Texture *font;

	(void) argc;
	(void) argv;

	CHK(SDL_Init(SDL_INIT_VIDEO) != 0);
	CHK(SDL_CreateWindowAndRenderer(320, 240, 0,
	                            &win, &rend) != 0);

	/* Use integer scale. */
	SDL_RenderSetLogicalSize(rend, 320, 240);
	SDL_RenderSetIntegerScale(rend, 1);

	font_ctx *ctx = FontStartup(rend);
	CHK(ctx == NULL);

	int ret = FontPrintToRenderer(ctx, "Hello World!", 0, 0, 2, 5);
	CHK(ret < 0);

	SDL_Rect dst = { .x = 50, .y = 100 };
	SDL_Surface *surf = FontRenderToSurface(ctx, "0123456789", &dst.w, &dst.h);
	CHK(surf == NULL);

	SDL_Texture *surf_tex = SDL_CreateTextureFromSurface(rend, surf);
	CHK(surf_tex == NULL);

	SDL_SetTextureColorMod(surf_tex, 0x00, 0xFF, 0x00);
	//SDL_RenderCopyEx(rend, surf_tex, NULL, &dst, 30.0, NULL, SDL_FLIP_NONE);


	SDL_RenderCopy(rend, surf_tex, NULL, NULL);

#if 0
	FontPrint(&white, "Hello World!", 0, 0, 2, 2);
	FontPrint(&white, "You can print me anywhere.", 10, 90, 2, 3);
	FontPrint(&green, "little green", 0, 160, 1, 1);
	FontPrint(&green, "BIG GREEN", 80, 170, 4, 12);
#endif
	SDL_RenderPresent(rend);

	SDL_Delay(10000);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}

