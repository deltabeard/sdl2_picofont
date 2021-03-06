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


int main(int argc, char *argv[])
{
	SDL_Window *win;
	SDL_Renderer *rend;
	font_ctx *ctx;
	SDL_Texture *tex;
	struct
	{
		unsigned char run : 1;
		unsigned char dir_x : 1;
		unsigned char dir_y : 1;
		unsigned char colour_sel : 3;
	} b =
	{
		.run = 1, .dir_x = 1, .dir_y = 1, .colour_sel = 0
	};
	int win_w = 320;
	int win_h = 240;

	CHK(SDL_Init(SDL_INIT_VIDEO) == 0);
	SDL_SetHint("SDL_HINT_RENDER_VSYNC", "1");

	CHK((win = SDL_CreateWindow("sdl2_picofont fancy example",
	                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	                            win_w, win_h, SDL_WINDOW_RESIZABLE)) != NULL);
	CHK((rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED |
	                               SDL_RENDERER_TARGETTEXTURE)) != NULL);

	CHK((ctx = FontStartup(rend)) != NULL);

	SDL_RenderSetLogicalSize(rend, win_w, win_h);

	int tex_w, tex_h;
	const char *str = "I'm a texture!";

	{
		FontDrawSize(strlen(str), &tex_w, &tex_h);
		tex = SDL_CreateTexture(rend, SDL_PIXELFORMAT_ARGB8888,
		                        SDL_TEXTUREACCESS_TARGET, tex_w, tex_h);
		CHK(tex != NULL);
	}

	{
		SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

		CHK(SDL_SetRenderTarget(rend, tex) == 0);
		SDL_SetRenderSDLColour(rend, white);
		CHK(FontPrintToRenderer(ctx, str, NULL) == 0);
		SDL_SetRenderSDLColour(rend, black);
		CHK(SDL_SetRenderTarget(rend, NULL) == 0);
	}

	/* Make output texture twice the size. */
	tex_w *= 2;
	tex_h *= 2;
	SDL_Rect dest_rect = { .x = 1, .y = 0, .w = tex_w, .h = tex_h};

	/* Limit when movement and colour changes occur. */
	Uint32 last_move_ms = SDL_GetTicks();
	const Uint32 move_delay_ms = 15;

	char allchars[('~' - ' ') + 1];
	char *c = allchars;
	for(char i = ' '; i <= '~'; i++)
	{
		*c = i;
		c++;
	}
	*c = '\0';

	while(b.run)
	{
		const Uint8 step_x = 1;
		const Uint8 step_y = 1;
		SDL_Event e;
		const SDL_Colour colour[8] =
		{
			{ .r = 0xFF, .g = 0x00, .b = 0x00 },
			{ .r = 0xFF, .g = 0xA5, .b = 0x00 },
			{ .r = 0xFF, .g = 0xFF, .b = 0x00 },
			{ .r = 0x00, .g = 0x80, .b = 0x00 },
			{ .r = 0xFF, .g = 0x00, .b = 0xFF },
			{ .r = 0x4B, .g = 0x00, .b = 0x82 },
			{ .r = 0xEE, .g = 0x82, .b = 0xEE },
			{ .r = 0xFF, .g = 0xFF, .b = 0xFF }
		};

		while(SDL_PollEvent(&e) != 0)
		{
			if(e.type == SDL_WINDOWEVENT &&
			                e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				win_w = e.window.data1;
				win_h = e.window.data2;
				SDL_RenderSetLogicalSize(rend, win_w, win_h);
				continue;
			}

			if(e.type == SDL_QUIT)
				b.run = 0;
		}

		SDL_SetRenderSDLColour(rend, black);
		SDL_RenderClear(rend);

		SDL_SetRenderSDLColour(rend, white);
		CHK(FontPrintToRenderer(ctx, allchars, NULL) == 0);

		Uint32 delay = move_delay_ms - (SDL_GetTicks() - last_move_ms);
		if(delay <= move_delay_ms)
			SDL_Delay(delay);

		last_move_ms = SDL_GetTicks();

		dest_rect.x += b.dir_x ? step_x : -(step_x);
		dest_rect.y += b.dir_y ? step_y : -(step_y);

		if(dest_rect.x >= (win_w - tex_w) || dest_rect.x <= 0)
		{
			dest_rect.x = b.dir_x ? (win_w - tex_w) : 0;
			b.dir_x = !b.dir_x;
		}

		if(dest_rect.y >= (win_h - tex_h) || dest_rect.y <= 0)
		{
			dest_rect.y = b.dir_y ? (win_h - tex_h) : 0;
			b.dir_y = !b.dir_y;
		}

		if(dest_rect.y % 30 == 0)
			b.colour_sel++;

		SDL_SetTextureColorMod(tex,
		                       colour[b.colour_sel].r,
		                       colour[b.colour_sel].g,
		                       colour[b.colour_sel].b);
		SDL_RenderCopy(rend, tex, NULL, &dest_rect);
		SDL_RenderPresent(rend);
	}

	FontExit(ctx);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}

