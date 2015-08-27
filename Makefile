CC := gcc
CFLAGS := -g -Wall -std=gnu99 -fgnu89-inline
LINKER := -lncurses
ENABLEDLL := -DBUILD_CHESSLIB_DLL
ENABLEDEBUG := -DDEBUG
DLL := chesslib.dll
ELF := chessgame
INC_W_LEVEL := -Wextra -pedantic
BLDFOLDER := build
TESTFOLDER := tests
CHESSLIB := chesslib.c
AIC := chlib-computer.c
prefix := /usr/local

# Build Linux cli ELF #
chessgame-cli: chessgame-cli.c chlib-computer.c chesslib.c chesslib.h
	if [ ! -e $(BLDFOLDER) ]; then mkdir $(BLDFOLDER); fi \
	&& $(CC) $(CFLAGS) $< $(CHESSLIB) $(AIC) -o $@ $(LINKER); mv $@ $(BLDFOLDER)


# Same thing just with more warnings #
chessgameWall: chessgame-cli.c chlib-computer.c chesslib.c chesslib.h
	if [ ! -e $(BLDFOLDER) ]; then mkdir $(BLDFOLDER); fi \
	&& $(CC) $(CFLAGS) $(INC_W_LEVEL) $< $(CHESSLIB) $(AIC) -o $@ $(LINKER); mv $@ $(BLDFOLDER)

# Build static library archive of ChessLib for Linux to be linked to your program at compile time #
chesslib: chesslib.c
	$(CC) -c $(CFLAGS) $<; \
	ar -cq chesslib.a chesslib.o; \
	rm chesslib.o

# Build Linux ELF with debugging flag #
debug: chessgame-cli.c chlib-computer.c chesslib.c chesslib.h
	if [ ! -e $(BLDFOLDER) ]; then mkdir $(BLDFOLDER); fi \
	&& $(CC) $(CFLAGS) $(ENABLEDEBUG) $< $(CHESSLIB) $(AIC) -o $@ $(LINKER); mv $@ $(BLDFOLDER)

# Build DLL #
dll: dllobject dllcompile
dllobject: chesslib.c
	$(CC) -c $(ENABLEDLL) $(CFLAGS) $<
	
dllcompile:
	$(CC) -shared -o $(DLL) chesslib.o -W1,--out-implib,chesslib.a

# Build Windows ELF with the above DLL #
exe: chessgame-cli.c chesslib.dll
	$(CC) -o chessgame.exe $<

# Run the Linux executables #
run:
	exec ./$(BLDFOLDER)/$(ELF)

runW:
	exec ./$(BLDFOLDER)/$(ELF)Wall

.PHONY: install clean  cleantxt
clean:
	rm -rf build tests \
	rm *.o

cleantxt:
	rm -rf build/*.txt \
	rm *.txt

install: chessgame-cli
	install -m 0755 $(BLDFOLDER)/$< $(prefix)/bin
