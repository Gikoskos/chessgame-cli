/********************************************************************
 *                              tree.c                              *
 *               various tests for the tree functions               *
 *                                                                  *
 *                 (C)2015 <cyberchiller@gmail.com>                 *
 *                                                                  *
 ********************************************************************/

#include <chesslib.h>
#include <chlib-cli.h> 


int main(void)
{
	ch_template chess_board[8][8];
	char start_move[3], end_move[3];
	int round = WHITE, total_rounds = 1;
	char *fools_mate[] = {"f2f3", "e7e5", "g2g4"};

	start_move[2] = '\0';
	end_move[2] = '\0';

	initChessboard(chess_board);
	while (total_rounds < 4) {
		deleteMoves();
		getAllMoves(chess_board, round);
		start_move[0] = fools_mate[total_rounds-1][0];
		start_move[1] = fools_mate[total_rounds-1][1];
		end_move[0] = fools_mate[total_rounds-1][2];
		end_move[1] = fools_mate[total_rounds-1][3];
		if (makeMove(chess_board, start_move, end_move, round)) {
			round = (round == BLACK)?WHITE:BLACK;
			total_rounds++;
		}
	}
	printBoard(chess_board, 'l');
}
