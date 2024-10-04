CC=gcc
SDL_CFLAGS = `sdl-config --cflags`
SDL_LIBS = `sdl-config --libs`
CFLAGS=-DUNIX -Wall $(SDL_CFLAGS)
OBJS=actor.o \
	animations.o \
	collision.o \
	debug.o \
	debug.grid.o \
	debug.scene.o \
	extra.o \
	fcaseopen.o \
	filereader.o \
	flamovies.o \
	gamestate.o \
	grid.o \
	holomap.o \
	hqrdepack.o \
	interface.o \
	keyboard.o \
	lbaengine.o \
	main.o \
	menu.o \
	menuoptions.o \
	movements.o \
	music.o \
	redraw.o \
	renderer.o \
	resources.o \
	scene.o \
	screens.o \
	script.life.o \
	script.move.o \
	sdlengine.o \
	sound.o \
	sys.o \
	xmidi.o \
	text.o

LIBS=$(SDL_LIBS) -lSDL_mixer -lm
VERSION=v0.3.0
TARGET=twin-e_$(VERSION)
INSTALLDIR=../bin

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm $(OBJS)
	@rm $(TARGET)
	@rm *.bak
	@rm stderr.txt
	@rm stdout.txt

install: $(TARGET)
	cp $(TARGET) $(INSTALLDIR)
