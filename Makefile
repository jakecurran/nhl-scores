default: install

CC = gcc
FLAGS = -ljansson -lcurl

INSTALL_DIR = /usr/local/bin

install:
	$(CC) -o $(INSTALL_DIR)/nhl nhl_scores.c $(FLAGS)

uninstall:
	rm -f $(INSTALL_DIR)/nhl

OBJ_DIR = bin

test:
	mkdir -p $(OBJ_DIR)
	$(CC) -o $(OBJ_DIR)/nhl nhl_scores.c $(FLAGS)
