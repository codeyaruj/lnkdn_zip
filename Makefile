# Makefile for ZIP Puzzle Game
# Immutable Board Architecture

CC = cc
CFLAGS = -std=c99 -Wall -Wextra -O2
TARGET = zip

SOURCES = main.c engine.c generator.c solver.c ui_terminal.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
