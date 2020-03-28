#include <SDL2/SDL.h>
#include "sdl2_picofont.h"

#define CHK(x)	if(!(x)) {						\
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,		\
		             "%s:%d: %s",				\
		             __func__, __LINE__, SDL_GetError());	\
		exit(EXIT_FAILURE);					\
	}

int main(void)
{
	SDL_Window *win;
	SDL_Renderer *rend;
	font_ctx *ctx;
	SDL_Texture *tex;


	CHK(SDL_Init(SDL_INIT_VIDEO) == 0);
	CHK(SDL_CreateWindowAndRenderer(320, 240,
	                                SDL_WINDOW_RESIZABLE,
	                                &win, &rend) == 0);
	SDL_SetWindowTitle(win, "sdl2_picofont basic example");
	CHK((ctx = FontStartup(rend)) != NULL);

	while(1)
	{
		SDL_Event e;
		const SDL_Colour white = { .r = 0xFF, .g = 0xFF, .b = 0xFF };
		const SDL_Colour skyblue = { .r = 0x87, .g = 0xCE, .b = 0xEB };

		while(SDL_PollEvent(&e) != 0)
		{
			if(e.type == SDL_QUIT)
				goto out;
		}

		SDL_RenderClear(rend);
		CHK(FontPrintToRenderer(ctx, "Hello World!",
		                        0, 0, 2, 2, white) == 0);

		CHK(FontPrintToRenderer(ctx, "Stretch!",
		                        100, 80, 2, 6, skyblue) == 0);

		CHK(FontPrintToRenderer(ctx, "Stretch!",
		                        100, 80, 2, 6, skyblue) == 0);

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
