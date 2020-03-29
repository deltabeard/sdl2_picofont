#include <SDL2/SDL.h>
#include <time.h>
#include "sdl2_picofont.h"

#define CHK(x)	if(!(x)) {					\
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,	\
			"%s:%d: %s", __func__, __LINE__, SDL_GetError()); \
		exit(EXIT_FAILURE);				\
		}

#define WIN_WIDTH	320
#define WIN_HEIGHT	240

int main(void)
{
	SDL_Window *win;
	SDL_Renderer *rend;
	font_ctx *ctx;
	const SDL_Colour blue = { .r = 0x00, .g = 0x00, .b = 0xFF, .a = 0x00 };
	SDL_Texture *tex;

	CHK(SDL_Init(SDL_INIT_VIDEO) == 0);
	CHK(SDL_CreateWindowAndRenderer(WIN_WIDTH, WIN_HEIGHT,
					SDL_WINDOW_RESIZABLE, &win, &rend) == 0);
	CHK((ctx = FontStartup(rend)) != NULL);

	SDL_RenderSetLogicalSize(rend, WIN_WIDTH, WIN_HEIGHT);
	SDL_RenderSetIntegerScale(rend, 1);

	int tex_w, tex_h;
	CHK((tex = FontRenderToTexture(ctx, "I'm a texture!", &tex_w, &tex_h)) != NULL);

	tex_w *= 2;
	tex_h *= 2;
	SDL_Event e;
	unsigned run = 1;
	SDL_Rect dest_rect = { .x = 0, .y = 0, .w = tex_w, .h = tex_h};
	SDL_Colour colour[7] =
	{
		{ .r = 0xFF, .g = 0x00, .b = 0x00 },
		{ .r = 0xFF, .g = 0xA5, .b = 0x00 },
		{ .r = 0xFF, .g = 0xFF, .b = 0x00 },
		{ .r = 0x00, .g = 0x80, .b = 0x00 },
		{ .r = 0xFF, .g = 0x00, .b = 0xFF },
		{ .r = 0x4B, .g = 0x00, .b = 0x82 },
		{ .r = 0xEE, .g = 0x82, .b = 0xEE }
	};
	Uint8 r = 0, g = 0x0F, b = 0xF0;
	Uint8 dir_x = 1, dir_y = 1;
	Uint8 colour_sel = 0;
	srand(time(NULL));
	Uint8 step_x = (rand() % 4) + 1;
	Uint8 step_y = (rand() % 4) + 1;

	while(run)
	{
		SDL_RenderClear(rend);

		CHK(FontPrintToRenderer(ctx, "Hello World!",
					100, 0, 2, 5, blue) == 0);

		while(SDL_PollEvent(&e) != 0)
		{
			if(e.type == SDL_QUIT)
				run = 0;
		}

		if(dest_rect.y % 30 == 0)
		{
			colour_sel++;
			colour_sel &= 0b111;
		}

		dest_rect.x += dir_x ? step_x : -(step_x);
		dest_rect.y += dir_y ? step_y : -(step_y);

		if(dest_rect.x >= (WIN_WIDTH - tex_w) || dest_rect.x <= 0)
		{
			step_x = (rand() % 4) + 1;
			dest_rect.x = dir_x ? (WIN_WIDTH - tex_w) : 0;
			dir_x = !dir_x;
		}

		if(dest_rect.y >= (WIN_HEIGHT - tex_h) || dest_rect.y <= 0)
		{
			step_y = (rand() % 4) + 1;
			dest_rect.y = dir_y ? (WIN_HEIGHT - tex_h) : 0;
			dir_y = !dir_y;
		}

		SDL_SetTextureColorMod(tex,
		                       colour[colour_sel].r,
		                       colour[colour_sel].g,
		                       colour[colour_sel].b);
		SDL_RenderCopy(rend, tex, NULL, &dest_rect);
		SDL_RenderPresent(rend);
		SDL_Delay(15);
	}

	FontExit(ctx);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}

