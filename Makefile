CC := gcc
CFLAGS := -g -Wall -std=gnu99 -fgnu89-inline
LINKER := -lncurses
ENABLEDLL := -DBUILD_CHESSLIB_DLL
ENABLEAI := -DAI_IS_ENABLED
ENABLEDEBUG := -DDEBUG
DLL := chesslib.dll
ELF := chessgame
INC_W_LEVEL := -Wextra -pedantic
BLDFOLDER := build
TESTFOLDER := tests
CHESSLIB := chesslib.c
AI := computer.c
prefix := /usr/local

# Build Linux cli ELF with no AI #
chessgame-cli: chessgame-cli.c chesslib.c chesslib.h
	if [ ! -e $(BLDFOLDER) ]; then mkdir $(BLDFOLDER); fi \
	&& $(CC) $(CFLAGS) $< $(CHESSLIB) -o $@ $(LINKER); mv $@ $(BLDFOLDER)


# Same thing just with more warnings #
chessgameWall: chessgame-cli.c chesslib.c chesslib.h
	if [ ! -e $(BLDFOLDER) ]; then mkdir $(BLDFOLDER); fi \
	&& $(CC) $(CFLAGS) $(INC_W_LEVEL) $< $(CHESSLIB) -o $@ $(LINKER); mv $@ $(BLDFOLDER)

# Build static library archive of ChessLib for Linux to be linked to your program at compile time #
chesslib: chesslib.c
	$(CC) -c $(CFLAGS) $<; \
	ar -cq chesslib.a chesslib.o; \
	rm chesslib.o

# Build Linux ELF with AI tests #
AItests: chessgame-cli.c chesslib.c computer.c chesslib.h
	if [ ! -e $(TESTFOLDER) ]; then mkdir $(TESTFOLDER); fi \
	&& $(CC) $(CFLAGS) $(ENABLEAI) $< $(CHESSLIB) $(AI) -o $@ $(LINKER); mv $@ $(TESTFOLDER)

# Build Linux ELF with no AI with debugging flag #
debug: chessgame-cli.c chesslib.c chesslib.h
	if [ ! -e $(BLDFOLDER) ]; then mkdir $(BLDFOLDER); fi \
	&& $(CC) $(CFLAGS) $(ENABLEDEBUG) $< $(CHESSLIB) -o $@ $(LINKER); mv $@ $(BLDFOLDER)

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
	install -m 0755 $(BLDFOLDER)/chessgame $(prefix)/bin
