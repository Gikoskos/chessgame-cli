/********************************************************************
 *                            chesslib.h                            *
 *                  API for the chesslib.c library                  *
 *                                                                  *
 *               (C)2015 <georgekoskerid@outlook.com>               *
 *                                                                  *
 ********************************************************************/

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
#elif defined(__FreeBSD__) \
|| defined(__linux__) \
|| defined(__APPLE__) \
|| defined(__gnu_linux__)
# define KRED  "\x1B[31m"
# define KYEL  "\x1B[33m"
# define RESET "\033[0m"
# include <termcap.h>
# include <alloca.h>
#else
# error Non-compatible OS or compiler
#endif

#define initChessboard(x) _initChessboard(x, 0, 'A')

#define MOS 17	/*this one controls the chess board size, don't mess with it*/
#define WHITE 2
#define BLACK 1
#define EMPTY 0
#define BANNER_SPEED 100 - R_SPEED
#define R_SPEED 80	/*speed of the animated banner, bigger value bigger speed; maximum value is 99*/
#define s_l 26	/*length of the filename string*/
#define ALL_CASTL_TRUE {true, true, true, true, true, true}
#define CSTL_LEFTROOK "l"
#define CSTL_RIGHTROOK "r"
#define AI_IS_ENABLED argtmp=='c'?1:0

/*versioning*/
#define CHESSGAMECLI_MAJOR 0
#define CHESSGAMECLI_MINOR 2
#define CHESSGAMECLI_PATCH 0

#define SSTR(x) STR(x)
#define STR(x) #x

#define CHESSGAMECLI_VERSION_STRING SSTR(CHESSGAMECLI_MAJOR)"."SSTR(CHESSGAMECLI_MINOR)"."SSTR(CHESSGAMECLI_PATCH)

/*standard chessboard template*/
typedef struct ch_template {
	char current ;	/*current piece letter, e for empty square*/
	char square[2];	/*current square on the board eg.A1,H4*/
	bool occ;	/*flag to check if square is occupied*/
	int c;	/*piece color, 0 if there is no piece, 1 for black, 2 for white*/
} ch_template;

/*struct of bools to check whether castling is possible for each piece*/
typedef struct CastlingBool {
	bool WR_left;	/*white rook at A1*/
	bool WR_right;	/*white rook at H1*/
	bool BR_left;	/*black rook at A8*/
	bool BR_right;	/*black rook at H8*/
	bool KBlack;	/*black king*/
	bool KWhite;	/*white king*/
} CastlingBool;

/*enumerator for the possible states a King can be in*/
typedef enum KingState {
	check,
	checkmate,
	safe,	/*King is safe (not threatened in his 3x3 vicinity*/
	safe_check	/*King is not in check but also not allowed to move to certain squares*/
} KingState;

/*possible moves that each King can do after a check situation
 *for example WKingMoves = "A8 H4 B3"*/
extern char *WKingMoves;
extern char *BKingMoves;

/*if a player chose to do castling and castling is possible
 *for this round, the value of this boolean becomes true*/
extern bool cstl_is_enabled;
extern CastlingBool check_castling;


/******************************************
 *function prototypes for the main library*
 ******************************************/

/*fills a ch_template[8][8] chess board with chess pieces, recursively
 *call initChessboard() if you just need to fill a standard chess board; don't 
 *use this function
 *k: the row the function starts filling pieces from
 *col: same as k except that it's the column not the row*/
void _initChessboard(ch_template chb[][8], unsigned k, char col);

/*function to print the board at any given point in the game
 *p: character that determines the chess piece type that will be printed; 'a' is for
 *capital letters and 'p' is for the Unicode chess pieces (doesn't work on Windows)*/
void printBoard (ch_template chb[][8], const char p);

/*traverses the chessboard, finds and returns the piece that is capable
 *to perform the move entered by the player; if more than one piece can move
 *to the square entered by the player it finds and returns them both
 *input: string with the piece and square to be moved to, e.g. PH3 NA5 KC1
 *color: the piece color of the current player*/
char *findPiece (ch_template chb[][8], const char *input, int color);

/*move the piece if no other piece is in the way; also checks for pawn promotion
 *returns false if the move isn't legal
 *plInput: string with the piece and square to be moved to, e.g. PH3 NA5 KC1
 *piece: array of two characters for the column and row of the square the 
 *piece is on, e.g. H3 C7*/
bool movePiece (ch_template chb[][8], char *plInput, char piece[2], int color);

/*checks if a move is valid based on whether the piece overlaps other pieces or not
 *it takes the coordinates of two pieces (the first always has to be Q, R or B),
 *as arguments, and calculates if there are any other pieces inbetween them
 *sx, sy: the row and column of the first piece
 *ex, ey: the row and column of the second piece
 *piece: the first piece's letter*/
bool piecesOverlap (ch_template chb[][8], const int sx, const int sy, const int ex, const int ey, const char piece);

/*in case a King's domain is threatened in any way (check or safe_check) this function
 *checks if the next move entered removes the King from a threatening situation;
 *arguments are the same as movePiece*/
bool isCheckMoveValid(ch_template chb[][8], char *plInput, char piece[2], int color);

/*check for validity of player input; returns false for bad input and true otherwise
 *input: input buffer as entered by the player; check if the string is NULL before 
 *using this function
 *errPtr: reference to the error code integer in the main function*/
bool validInput (const char *input, int *errPtr);

/*create a string with the current date to be used as the log file's filename*/
void date_filename (char*, int);

/*write each player's moves to a log file
 *round: BLACK or WHITE
 *logf: file descriptor of the log file
 *plInput: input entered by the player; contains the piece and the square the piece moved into
 *piece: the initial square the piece was into*/
void write_to_log (int round, FILE* logf, char *plInput, char piece[2]);

/*basic error printing function; writes output to stderr
 *see implementation for possible error codes*/
void printError (int);

/*safely copies the input buffer to a string and returns that string*/
char *getPlayerInput (void);

/*handles move conflict: whether two pieces of the same kind and color 
 *are able to move in the same square entered by the player, at the same round
 *piece_pos: string with 4 characters that holds the location of the two squares e.g. H6A4
 *p: the piece to move*/
char *pieceConflict (const char *piece_pos, const char p);

/*finds and saves the state of each King for the current round; see 
 *the KingState enum for all the states a King can have*/
void findKState (ch_template chb[][8], KingState *WK, KingState *BK);

/*takes care of all the moves that happen during castling; only call it if cstl_is_enabled is true
 *plInput: the castling piece that was returned by findPiece
 *color: the piece color of the player*/
void setCastling (ch_template chb[][8], char *plInput, int color);

/*functions I created to adjust my own chess game*/
inline void clear_screen(void);
inline void clear_buffer(void);
inline void printBanner(const char*);
inline void printInstructions(void);
char *AImove(ch_template chb[][8]);
