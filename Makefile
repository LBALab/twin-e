all: prequengine
CC=gcc
SDL_CFLAGS = `sdl-config --cflags`
SDL_LIBS = `sdl-config --libs`
CFLAGS=-ggdb -DUNIX -DGAMEMOD -Wall $(SDL_CFLAGS)
OBJS=animations.o debug.o debug.scene.o debug.grid.o renderer.o redraw.o lbaengine.o actor.o scene.o grid.o dialogues.o resources.o movies.o sdlengine.o hqrdepack.o filereader.o images.o sound.o  music.o menuoptions.o menu.o interface.o main.o

LIBS=$(SDL_LIBS) -lSDLmain -lSDL_mixer -lSDL_ttf

prequengine: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm $(OBJS)
