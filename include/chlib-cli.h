/**********************************************************************
 *                            chlib-cli.h                             *
 *      function interface specific to a command-line chess game      *
 *                                                                    *
 *                  (C)2015 <cyberchiller@gmail.com>                  *
 *                                                                    *
 **********************************************************************/

#ifndef CHLIB_CLI_H
#define CHLIB_CLI_H

#include <chesslib.h>

#define MOS 17	/*this one controls the chess board size, don't mess with it*/

/*safely copies the input buffer to a string and returns that string*/
char *getPlayerInput (void);

/*function to print the board at any given point in the game
 *p: character that determines the chess piece type that will be printed; 'a' is for
 *capital letters and 'p' is for the Unicode chess pieces (doesn't work on Windows)*/
void printBoard (ch_template chb[][8], const char p);

/*check for validity of player input; returns false for bad input and true otherwise
 *input: input buffer as entered by the player; check if the string is NULL before 
 *using this function
 *errPtr: reference to the error code integer in the main function*/
bool validInput (const char *input, int *errPtr);

/*self-explanatory functions*/
inline void clear_screen(void);
inline void clear_buffer(void);
inline void printBanner(const char*);

#endif
