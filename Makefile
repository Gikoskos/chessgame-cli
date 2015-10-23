CC := gcc
CFLAGS := -g -Wall -std=gnu99 -fgnu89-inline
LINKER :=
ENABLEDLL := -DBUILD_CHESSLIB_DLL
NDEBUG := -DNDEBUG
DLL := chesslib.dll
ELF := chessgame-cli
INC_W_LEVEL := -Wextra -pedantic
BLDFOLDER := build
TESTFOLDER := tests
CHESSLIB := chesslib.c
AIC := chlib-computer.c
prefix := /usr/local

# Build Linux cli ELF #
chessgame-cli: chessgame-cli.c $(AIC) $(CHESSLIB)
	if [ ! -e $(BLDFOLDER) ]; then mkdir $(BLDFOLDER); fi \
	&& $(CC) $(CFLAGS) $(NDEBUG) $< $(CHESSLIB) $(AIC) -o $@ $(LINKER); mv $@ $(BLDFOLDER)


# Same thing just with more warnings #
chessgame-cliWall: chessgame-cli.c $(CHESSLIB) $(AIC)
	if [ ! -e $(BLDFOLDER) ]; then mkdir $(BLDFOLDER); fi \
	&& $(CC) $(CFLAGS) $(NDEBUG) $(INC_W_LEVEL) $< $(CHESSLIB) $(AIC) -o $@ $(LINKER); mv $@ $(BLDFOLDER)

# Build static library archive of ChessLib for Linux to be linked to your program at compile time #
chesslib: $(CHESSLIB)
	$(CC) -c $(CFLAGS) $(NDEBUG) $<; \
	ar csq libchesslib.a chesslib.o; \
	rm chesslib.o

# Build Linux ELF with debugging flags enabled #
debug: chessgame-cli.c $(CHESSLIB) $(AIC)
	if [ ! -e $(BLDFOLDER) ]; then mkdir $(BLDFOLDER); fi \
	&& $(CC) $(CFLAGS) $< $(CHESSLIB) $(AIC) -o $@ $(LINKER); mv $@ $(BLDFOLDER)

# Build Windows ELF
exe: chessgame-cli.c $(CHESSLIB) $(AIC)
	$(CC) $(NDEBUG) -o chessgame-cli.exe $^

# Run the Linux executables #
run:
	exec ./$(BLDFOLDER)/$(ELF)

runW:
	exec ./$(BLDFOLDER)/$(ELF)Wall

.PHONY: install clean  cleantxt
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
