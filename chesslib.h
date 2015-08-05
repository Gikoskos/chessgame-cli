/********************************************************************/
/*                            chesslib.h                            */
/*              declarations for the chesslib.c engine              */
/*                                                                  */
/*                    by cyberchiller@gmail.com                     */
/*                                                                  */
/********************************************************************/

#if !defined(__GNUC__) && !defined(__SUNPRO_C)
# pragma once
#endif

#ifndef CHESSLIB_H
# define CHESSLIB_H
# include <stdio.h>
# include <stdbool.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>
# include <time.h>
# include <unistd.h>

# if defined(__MINGW32__) || defined(_WIN32)
#  ifndef UNICODE
#   define UNICODE
#  endif
#  ifndef _UNICODE
#   define _UNICODE
#  endif
#  include <windows.h>
# elif defined(__unix__) || defined(__gnu_linux__)
#  define KRED  "\x1B[31m"
#  define KYEL  "\x1B[33m"
#  define RESET "\033[0m"
#  include <termcap.h>
#  include <alloca.h>
# else
#  error Non-compatible OS or compiler found
# endif

# define MOS 17	//this one controls the chess board size, don't mess with it
# define WHITE 2
# define BLACK 1
# define EMPTY 0
# define BANNER_SPEED 100 - R_SPEED
# define R_SPEED 75	//speed of the animated banner, bigger value bigger speed; maximum value is 99
# define s_l 26	//length of the filename string

typedef struct ch_template {
	char current ;	//current piece letter, e for empty square
	char square[2];	//current square on the board eg.A1,H4
	bool occ;	//flag to check if square is occupied
	int c;	//piece color, 0 if there is no piece, 1 for black, 2 for white
}ch_template;

typedef enum KingState {
	checkB,	/*check for Black King*/
	checkmateB, /*checkmate for Black King*/
	checkW,	/*check for White King*/
	checkmateW,	/*checkmate for White King*/
	safe	/*King is safe*/
}KingState;


/*prototypes*/
void initChessboard(ch_template[][8], unsigned, char);	//add pieces on the chess board recursively 
void printBoard(ch_template[][8]);	//function to print the board at any given point in the game
char *findPiece(ch_template[][8], const char*, int);	//find the piece that is capable to perform the move entered by the player
bool movePiece(ch_template[][8], char*, char[2], int);	//move the piece if no other piece is in the way
bool piecesOverlap(ch_template[][8], const int, const int, const int , const int, const char);
bool validInput(const char*, int*);	//check for validity of input
void date_filename(char*, int);	//create a string with the current date to be used as the log date_filename
void write_to_log(int, FILE*, char*, char[]);	//write each player's moves to a log file
void printError(int);
char *getPlayerInput(void);	//copy the input buffer to a string
char *pieceConflict(const char*, const char);
inline void clear_screen(void);
inline void clear_buffer(void);
inline void printInstructions(void);
inline void printBanner(const char*);
bool check(ch_template[][8]);
KingState findKState(ch_template[][8]);

#endif
