SRC_DIR=src
HEADER_DIR=include
BUILD_DIR=build
PLAYGROUND_DIR=playground
CFLAGS=-isysroot $(shell xcrun --show-sdk-path) -Wall -Wextra -I$(HEADER_DIR) 
CC=gcc-14

.PHONY: main 

main: clear all structures lexical
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/object/pp.o -c $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/pp $(BUILD_DIR)/object/*.o

structures: 
	$(CC) $(CFLAGS) -c $(SRC_DIR)/structures/*.c
	mv *.o $(BUILD_DIR)/object/

lexical: 
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/object/lexical.o -c $(SRC_DIR)/lexical.c

playground: all
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(PLAYGROUND_DIR)/playground.o $(PLAYGROUND_DIR)/hashtable.playground.c $(SRC_DIR)/structures/hashtable.c

all: 
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/object
	mkdir -p $(BUILD_DIR)/$(PLAYGROUND_DIR)

clear:
	rm -rf $(BUILD_DIR)