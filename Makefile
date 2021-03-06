CC = gcc
OBJECTS = obj/main.o obj/game_state.o obj/in_game.o obj/main_menu.o obj/map.o obj/intro.o obj/game_over.o obj/fov.o
TARGET = bin/cyjam
CFLAGS = -Wall -I./include -std=c99
LDLIBS = -lncursesw -lm

all: CFLAGS += -O3
all: $(TARGET)

debug: CFLAGS += -g
debug: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(CLFAGS) $(LDLIBS)

obj/main.o: src/main.c include/constants.h include/game_state.h
	$(CC) -c -o $@ $< $(CFLAGS)

obj/game_state.o: src/game_state.c include/game_state.h include/in_game.h
	$(CC) -c -o $@ $< $(CFLAGS)

obj/in_game.o: src/in_game.c include/in_game.h include/game_state.h include/map.h
	$(CC) -c -o $@ $< $(CFLAGS)

obj/main_menu.o: src/main_menu.c include/main_menu.h include/game_state.h
	$(CC) -c -o $@ $< $(CFLAGS)

obj/intro.o: src/intro.c include/intro.h include/intro_img.h include/game_state.h
	$(CC) -c -o $@ $< $(CFLAGS)

obj/game_over.o: src/game_over.c include/game_over.h include/game_state.h
	$(CC) -c -o $@ $< $(CFLAGS)

obj/map.o: src/map.c include/map.h
	$(CC) -c -o $@ $< $(CFLAGS)

obj/fov.o: src/fov.c include/fov.h
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(RM) $(TARGET) $(OBJECTS)
