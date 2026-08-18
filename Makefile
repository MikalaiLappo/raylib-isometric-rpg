CC = clang
CFLAGS = -std=c99 -Wall -Wextra -O2
BREW_PREFIX = /opt/homebrew
INCLUDES = -I$(BREW_PREFIX)/include -Isrc -Isrc/rendering -Isrc/core -Isrc/game -Isrc/ui
LIBRARY_PATHS = -L$(BREW_PREFIX)/lib
LDFLAGS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
LIBS = -lraylib -lm

SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/isometric-rpg

SOURCES = $(wildcard $(SRC_DIR)/*.c) \
          $(wildcard $(SRC_DIR)/rendering/*.c) \
          $(wildcard $(SRC_DIR)/core/*.c) \
          $(wildcard $(SRC_DIR)/game/*.c) \
          $(wildcard $(SRC_DIR)/ui/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBRARY_PATHS) $(LDFLAGS) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

format:
	clang-format -i $(shell find src -name '*.c' -o -name '*.h')

format-check:
	clang-format --dry-run --Werror $(shell find src -name '*.c' -o -name '*.h')

lint:
	cppcheck --enable=all --suppress=missingIncludeSystem --error-exitcode=1 src/

.PHONY: all clean run format format-check lint