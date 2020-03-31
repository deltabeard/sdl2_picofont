OPT ?= -Os -s
CFLAGS += $(shell sdl2-config --cflags) $(OPT) -D SDL_ASSERT_LEVEL=2
LDLIBS += $(shell sdl2-config --libs)

all: font_example_fancy font_example_basic
font_example_basic:  font_example_basic.c sdl2_picofont.o
font_example_fancy:  font_example_fancy.c sdl2_picofont.o
sdl2_picofont.o: sdl2_picofont.c sdl2_picofont.h

clean:
	$(RM) font_example_basic font_example_fancy sdl2_picofont.o
