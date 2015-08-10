/********************************************************************/
/*                            chesslib.h                            */
/*              declarations for the chesslib.c engine              */
/*                                                                  */
/*                    by <cyberchiller@gmail.com>                   */
/*              see COPYING for copyright information               */
/********************************************************************/

#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#if defined(__MINGW32__) || defined(_WIN32)
# ifndef UNICODE
#  define UNICODE
# endif
# ifndef _UNICODE
#  define _UNICODE
# endif
# include <windows.h>
#elif defined(__unix__) || defined(__gnu_linux__)
# define KRED  "\x1B[31m"
# define KYEL  "\x1B[33m"
# define RESET "\033[0m"
# include <termcap.h>
# include <alloca.h>
#else
# error Non-compatible OS or compiler
#endif

#define MOS 17	/*this one controls the chess board size, don't mess with it*/
#define WHITE 2
#define BLACK 1
#define EMPTY 0
#define BANNER_SPEED 100 - R_SPEED
#define R_SPEED 75	/*speed of the animated banner, bigger value bigger speed; maximum value is 99*/
#define s_l 26	/*length of the filename string*/

typedef struct ch_template {
	char current ;	/*current piece letter, e for empty square*/
	char square[2];	/*current square on the board eg.A1,H4*/
	bool occ;	/*flag to check if square is occupied*/
	int c;	/*piece color, 0 if there is no piece, 1 for black, 2 for white*/
} ch_template;

typedef enum KingState {
	check,
	checkmate,
	safe,	/*King is safe (not threatened in his 3x3 vicinity*/
	safe_check	/*King is not allowed to move to certain squares*/
} KingState;


/*prototypes*/
/*#1 add pieces on the chess board recursively*/
void initChessboard(ch_template[][8], unsigned, char);

/*function to print the board at any given point in the game*/
void printBoard(ch_template[][8]);

/*find the piece that is capable to perform the move entered by the player*/
char *findPiece(ch_template[][8], const char*, int);

/*move the piece if no other piece is in the way; also checks for pawn promotion*/
bool movePiece(ch_template[][8], char*, char[2], int);

/*checks if a move is valid based on whether the piece overlaps other pieces or not*/
bool piecesOverlap(ch_template[][8], const int, const int, const int , const int, const char);

/*check for validity of input*/
bool validInput(const char*, int*);

/*create a string with the current date to be used as the log date_filename*/
void date_filename(char*, int);

/*write each player's moves to a log file*/
void write_to_log(int, FILE*, char*, char[]);

/*basic error printing function*/
void printError(int);

/*copies the input buffer to a string and return that string*/
char *getPlayerInput(void);

/*prevents move conflict: whether two pieces of the same kind and color are able to move in the same square at the same round*/
char *pieceConflict(const char*, const char);

/*finds and saves the state of each King for the current round; see the KingState enum for all the states a King can have*/
void findKState(ch_template[][8], KingState*, KingState*);

/*functions I created to adjust my own chess game*/
inline void clear_screen(void);
inline void clear_buffer(void);
inline void printBanner(const char*);
inline void printInstructions(void);
