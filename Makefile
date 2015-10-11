CC := gcc
CFLAGS := -g3 -Wall -std=gnu11 -fgnu89-inline
LINKER := -lncurses
ENABLEDLL := -DBUILD_CHESSLIB_DLL
DEBUG := -DDEBUG
DLL := chesslib.dll
ELF := chessgame-cli
INC_W_LEVEL := -Wextra -pedantic
BLDFOLDER := build/
CHESSLIB := src/chesslib.c
AIC := src/chlib-computer.c
CLILIB := src/chlib-cli.c
CLISRC := test/chessgame-cli.c
INCLUDE_FOLDER := include/
INC := -I$(INCLUDE_FOLDER)
prefix := /usr/local/

# Build Linux cli ELF #
chessgame-cli: $(CLISRC) $(AIC) $(CHESSLIB) $(CLILIB)
	if [ ! -e $(BLDFOLDER) ]; then mkdir $(BLDFOLDER); fi \
	&& $(CC) $(CFLAGS) $(INC) $^ -o $@ $(LINKER); mv $@ $(BLDFOLDER)


# Same thing just with more warnings #
chessgame-cliWall: $(CLISRC) $(CHESSLIB) $(AIC) $(CLILIB)
	if [ ! -e $(BLDFOLDER) ]; then mkdir $(BLDFOLDER); fi \
	&& $(CC) $(CFLAGS) $(INC) $(INC_W_LEVEL) $^ -o $@ $(LINKER); mv $@ $(BLDFOLDER)

# Build static library archive of ChessLib for Linux to be linked to your program at compile time #
chesslib: $(CHESSLIB)
	$(CC) -c $(CFLAGS) $(INC) $(NDEBUG) $<; \
	ar csq libchesslib.a chesslib.o; \
	rm chesslib.o

# Build Linux ELF with debugging flags enabled #
debug: $(CLISRC) $(CHESSLIB) $(AIC) $(CLILIB)
	if [ ! -e $(BLDFOLDER) ]; then mkdir $(BLDFOLDER); fi \
	&& $(CC) $(CFLAGS) $(INC) $(DEBUG) $^ -o $@ $(LINKER); mv $@ $(BLDFOLDER)

# Build DLL #
dll: dllobject dllcompile
dllobject: $(CHESSLIB) $(AIC)
	$(CC) -c $(ENABLEDLL) $(INC) $(CFLAGS) $^
	
dllcompile:
	$(CC) -shared -o $(DLL) chesslib.o chlib-computer.o -Wl,--out-implib,chesslib.a

# Build Windows ELF with the above DLL #
exe: $(CLISRC) $(DLL)
	$(CC) -o chessgame-cli.exe $^

# Run the Linux executables #
run:
	@exec ./$(BLDFOLDER)/$(ELF)

runW:
	@exec ./$(BLDFOLDER)/$(ELF)Wall

.PHONY: install clean cleantxt
cleanall: clean cleantxt

clean:
	rm -rf build tests \
	rm *.o \
	rm *.out \
	rm libchesslib.a

cleantxt:
	rm -rf build/*.txt \
	rm *.txt

install: chessgame-cli
	install -m 0755 $(BLDFOLDER)/$< $(prefix)/bin
