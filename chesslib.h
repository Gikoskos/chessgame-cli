#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <termcap.h>
#include <time.h>
#include <alloca.h>
#include <unistd.h>

#define KRED  "\x1B[31m"
#define KYEL  "\x1B[33m"
#define RESET "\033[0m"
#define WHITE 2
#define BLACK 1
#define EMPTY 0
#define BANNER_SPEED 100 - R_SPEED
#define R_SPEED 10	//speed of the animated banner, bigger value bigger speed; maximum value is 99
#define s_l 26	//length of the filename string

struct ch_template {
	char current ;	//current piece letter, e for empty square
	char square[2];	//current square on the board eg.A1,H4
	bool occ;	//flag to check if square is occupied
	int c;	//piece color, 0 if there is no piece, 1 for black, 2 for white
};

typedef struct ch_template ch_template;

/*prototypes*/
void initChessboard(ch_template[][8], unsigned, char);	//add pieces on the chess board recursively 
void printBoard(ch_template[][8]);	//function to print the board at any given point in the game
char *findPiece(ch_template[][8], const char*, int);	//find the piece that is capable to perform the move entered by the player
bool movePiece(ch_template[][8], char*, char[2], int);	//move the piece if no other piece is in the way
bool piecesOverlap(ch_template[][8], int, int, int ,int);
bool validInput(const char*);	//check for validity of input
void date_filename(char*, int);	//create a string with the current date to be used as the log date_filename
void write_to_log(int, FILE*, char*, char[]);	//write each player's moves to a log file
char *getPlayerInput(void);	//copy the input buffer to a string
char *pawnConflict(const char*);
inline void clear_screen(void);
inline void clear_buffer(void);
inline void printInstructions(void);
inline void printBanner(const char*);
