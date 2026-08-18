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
	cppcheck --enable=all \
		-D__APPLE__ \
		-D__arm64__ \
		-D__LP64__ \
		-I src \
		-I src/rendering \
		-I src/core \
		-I src/game \
		-I src/ui \
		-I /opt/homebrew/include \
		-I /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include \
		--suppress=missingIncludeSystem \
		--suppress=unusedFunction:/Library/Developer/* \
		--suppress=constParameterPointer \
		--suppress=constVariablePointer \
		--check-level=exhaustive \
		--max-configs=1 \
		--error-exitcode=1 \
		src/

.PHONY: all clean run format format-check lint