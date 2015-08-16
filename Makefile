CC = gcc
CFLAGS = -g -Wall -std=gnu99 -fgnu89-inline
LINKER = -lncurses
ENABLEDLL = -DBUILD_CHESSLIB_DLL
DLL = chesslib.dll
ELF = chessgame
INC_W_LEVEL = -Wextra -pedantic
BLDFOLDER = build
CHESSLIB = chesslib.c
prefix = /usr/local

chessgame: main.c chesslib.c chesslib.h
	if [ ! -e $(BLDFOLDER) ]; then mkdir $(BLDFOLDER); fi \
	&& $(CC) $(CFLAGS) $< $(CHESSLIB) -o $@ $(LINKER); mv $@ $(BLDFOLDER)

chessgameWall: main.c chesslib.c chesslib.h
	if [ ! -e $(BLDFOLDER) ]; then mkdir $(BLDFOLDER); fi \
	&& $(CC) $(CFLAGS) $(INC_W_LEVEL) $< $(CHESSLIB) -o $@ $(LINKER); mv $@ $(BLDFOLDER)

chesslib: chesslib.c
	$(CC) -c $(CFLAGS) $<; \
	ar -cq chesslib.a chesslib.o; \
	rm chesslib.o
	
dll: dllobject dllcompile
dllobject: chesslib.c
	$(CC) -c $(ENABLEDLL) $(CFLAGS) $<
	
dllcompile:
	$(CC) -shared -o $(DLL) chesslib.o -W1,--out-implib,chesslib.a

exe: main.c chesslib.dll
	$(CC) -o chessgame.exe $<

run:
	exec ./$(BLDFOLDER)/$(ELF)

runW:
	exec ./$(BLDFOLDER)/$(ELF)Wall

.PHONY: install clean  cleantxt
clean:
	rm -rf build \
	rm -rf *.o

cleantxt:
	rm -rf build/*.txt \
	rm -rf *.txt

install: chessgame $(BLDFOLDER)/chessgame
	install -m 0755 $< $(prefix)/bin
