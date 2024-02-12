CFLAGS := -std=c17 -Wall -Werror -Wextra
SRC_DIR := src
BUILD_DIR := build
TARGET := chip8
SDL2 := `sdl2-config --cflags --libs`

SRCS := $(wildcard $(SRC_DIR)/*.c) \
		$(SRC_DIR)/chip8.c
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	gcc $^ -o $@ $(CFLAGS) $(SDL2)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	gcc -c $< -o $@ $(CFLAGS) $(SDL2)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean