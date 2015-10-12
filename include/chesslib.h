/********************************************************************
 *                            chesslib.h                            *
 *                  API for the chesslib.c library                  *
 *                                                                  *
 *                 (C)2015 <cyberchiller@gmail.com>                 *
 *                                                                  *
 ********************************************************************/

#ifndef CHESS_LIB_H
#define CHESS_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#if defined(__MINGW32__) || defined(_WIN32)
# ifdef BUILD_CHESSLIB_DLL
#  define CHESSLIB_DLL __declspec(dllexport)
# else
#  define CHESSLIB_DLL __declspec(dllimport)
# endif
# include <windows.h>
#endif

#define ALL 0x1eae          /*7854*/
#define WHITE 0x1eaf        /*7855*/
#define BLACK 0x1eb0        /*7856*/
#define EMPTY 0x1eb1        /*7857*/
#define s_l 26	/*length of the filename string*/
#define CSTL_LEFTROOK "l"
#define CSTL_RIGHTROOK "r"
#define PAWN 'P'
#define KING 'K'
#define QUEEN 'Q'
#define BISHOP 'B'
#define KNIGHT 'N'
#define ROOK 'R'

/*versioning*/
#define CHESSLIB_MAJOR 0
#define CHESSLIB_MINOR 2
#define CHESSLIB_PATCH 0
#define SSTR(x) STR(x)
#define STR(x) #x
#define CHESSLIB_VERSION_STRING SSTR(CHESSLIB_MAJOR)"."SSTR(CHESSLIB_MINOR)"."SSTR(CHESSLIB_PATCH)


#define initChessboard(x) _initChessboard(x, 0, 'A')
#define printWhiteMoves() printMoves(w_moves)
#define printBlackMoves() printMoves(b_moves)



#define printMoves(x)                                                  \
{                                                                      \
    int move_list_piece_index = 0;                                     \
    char *piece_name[6] = {"Pawns:\n", "King:\n", "Queen:\n",          \
        "Rooks:\n", "Knights:\n", "Bishops:\n"};                       \
    while (move_list_piece_index < 6) {                                \
        fprintf(stdout, "%s", piece_name[move_list_piece_index]);      \
        printMoveList(x[move_list_piece_index++], stdout);             \
    }                                                                  \
}

#define deleteBlackMoves()                                             \
{                                                                      \
    int move_list_piece_index = 0;                                     \
    while (move_list_piece_index < 6)                                  \
        deleteMoveList(&b_moves[move_list_piece_index++]);             \
}

#define deleteWhiteMoves()                                             \
{                                                                      \
    int move_list_piece_index = 0;                                     \
    while (move_list_piece_index < 6)                                  \
        deleteMoveList(&w_moves[move_list_piece_index++]);             \
}


/*standard chessboard template*/
typedef struct ch_template {
	char current ;	/*current piece letter (P, N, Q, K, R), e for empty square*/
	char square[2];	/*current square on the board eg.A1,H4*/
	bool occ;	/*flag to check if square is occupied*/
	int c;	/*piece color, see the BLACK, WHITE and EMPTY macros*/
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

typedef enum KingState {
	check,
	checkmate,
	stalemate,
	safe,	/*King is safe (not threatened in his 3x3 vicinity*/
} KingState;

/*node template for the move list*/
typedef struct MoveNode {
	char start[3];	/*string for the square the piece is on*/
	char end[3];	/*string for the square the piece can move to*/
	struct MoveNode *nxt;
} MoveNode;

/*global list array that store the possible moves for each piece on each
 *round, for both players; each index of the array refers to each piece like so:
 *0 is Pawn (P), 1 is King (K), 2 is Queen (Q), 3 is Rook (R), 4 is Knight (N), 5 is Bishop (B)*/
extern MoveNode *b_moves[6];
extern MoveNode *w_moves[6];

/*counters for all the moves a player can do
 *they are initialized after every getMoveList() call*/
extern unsigned black_move_count;
extern unsigned white_move_count;

extern KingState WhiteKing, BlackKing;

/******************************************
 *function prototypes for the main library*
 ******************************************/

/*fills a ch_template[8][8] chess board with chess pieces, recursively
 *call initChessboard() if you just need to fill a standard chess board; don't 
 *use this function
 *k: the row the function starts filling pieces from
 *col: same as k except that it's the column not the row*/
void _initChessboard (ch_template chb[][8], unsigned k, char col);

int getAllMoves(ch_template chb[][8], int c_flag);

void printMoveList(MoveNode *llt, FILE *fd);

void deleteMoveList(MoveNode **llt);

__attribute__((destructor)) void deleteMoves();

int findOnMoveList(MoveNode *llt, char *tofind);

bool makeMove(ch_template chb[][8], char *st_move, char *en_move, const int color, const bool ListCheck);

void write_to_log(int round, FILE* logf, char *plInput, char piece[2]);

void date_filename(char *buf, int ln);

#endif
