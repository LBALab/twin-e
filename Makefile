CC=gcc
SDL_CFLAGS = `sdl-config --cflags`
SDL_LIBS = `sdl-config --libs`
CFLAGS=-DMACOSX -Wall $(SDL_CFLAGS)
OBJS=src/actor.o \
	src/animations.o \
	src/collision.o \
	src/debug.o \
	src/debug.grid.o \
	src/debug.scene.o \
	src/extra.o \
	src/fcaseopen.o \
	src/filereader.o \
	src/flamovies.o \
	src/gamestate.o \
	src/grid.o \
	src/holomap.o \
	src/hqrdepack.o \
	src/interface.o \
	src/keyboard.o \
	src/lbaengine.o \
	src/main.o \
	src/menu.o \
	src/menuoptions.o \
	src/movements.o \
	src/music.o \
	src/redraw.o \
	src/renderer.o \
	src/resources.o \
	src/scene.o \
	src/screens.o \
	src/script.life.o \
	src/script.move.o \
	src/platform_sdl.o \
	src/sound.o \
	src/sys.o \
	src/xmidi.o \
	src/text.o

LIBS=$(SDL_LIBS) -lSDL_mixer -lSDL_ttf -lm
VERSION=v0.3.0
TARGET=bin/twin-e_$(VERSION)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm $(OBJS)
	@rm $(TARGET)
