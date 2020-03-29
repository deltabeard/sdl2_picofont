OPT ?= -Os -s
CFLAGS += $(shell sdl2-config --cflags) $(OPT) -D SDL_ASSERT_LEVEL=2
LDLIBS += $(shell sdl2-config --libs)

all: font_example
font_example: font_example.c sdl2_picofont.c

clean:
	$(RM) font_example
