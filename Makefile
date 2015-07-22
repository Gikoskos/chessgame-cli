CC = gcc
CFLAGS = -g -Wall -std=c99 -fgnu89-inline
LINKER = -lncurses
ELF = chessgame
INC_W_LEVEL = -Wextra -pedantic
BLDFOLDER = ./build
CHESSLIB = chesslib.c

chessgame: main.c chesslib.c chesslib.h
	if [ ! -e $(BLDFOLDER) ]; then mkdir $(BLDFOLDER); fi \
	&& $(CC) $(CFLAGS) $< $(CHESSLIB) -o $@ $(LINKER); mv $@ $(BLDFOLDER)

run: chessgame
	exec ./build/chessgame

.PHONY: clean
clean:
	rm -rf build
