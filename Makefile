CC = gcc
SOURCES = src/main.c
OBJECTS = obj/main.o
TARGET = bin/cyjam
CFLAGS = -Wall -I./include
LDFLAGS = -L./lib
LDLIBS = -lm -lisland -lfov -lncurses

all: CFLAGS += -O3
all: $(TARGET)

debug: CFLAGS += -g
debug: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(CLFAGS) $(LDFLAGS) $(LDLIBS)

obj/main.o: src/main.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(RM) $(TARGET) $(OBJECTS)

