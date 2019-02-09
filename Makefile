default: build

CC = gcc
FLAGS = -ljansson -lcurl

SRC = src/nhl_scores.c

OBJ_DIR = bin
OBJ_NAME = nhl

INSTALL_DIR = /usr/local/bin

build:
	mkdir -p $(OBJ_DIR)
	$(CC) -o $(OBJ_DIR)/$(OBJ_NAME) $(SRC) $(FLAGS)

run:
	if [ ! -f $(OBJ_DIR)/$(OBJ_NAME) ]; then make build; fi;
	$(OBJ_DIR)/$(OBJ_NAME)

clean:
	rm -rf bin

install:
	$(CC) -o $(INSTALL_DIR)/$(OBJ_NAME) $(SRC) $(FLAGS)

uninstall:
	rm -f $(INSTALL_DIR)/$(OBJ_NAME)
