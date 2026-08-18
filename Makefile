CC = clang
CFLAGS = -std=c99 -Wall -Wextra -O2
BREW_PREFIX = /opt/homebrew
INCLUDES = -I$(BREW_PREFIX)/include
LIBRARY_PATHS = -L$(BREW_PREFIX)/lib
LDFLAGS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
LIBS = -lraylib -lm

SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/isometric-rpg

SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/player.c $(SRC_DIR)/tilemap.c
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBRARY_PATHS) $(LDFLAGS) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*.o $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run