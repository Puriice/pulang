SRC_DIR=src
BUILD_DIR=build
CFLAGS=-Wall -Werror -Wextra
CC=gcc

.PHONY: main

main: all
	$(CC) $(CFLAGS) $(SRC_DIR)/main.c -o $(BUILD_DIR)/pp

all: clear
	mkdir -p $(BUILD_DIR)

clear:
	rm -rf $(BUILD_DIR)