/********************************************************************
 *                          scholarsmate.c                          *
 *                 scholar's mate to test chesslib                  *
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
	char *scholars_mate[] = {"e2e4", "e7e5", "f1c4", "g8f6", "d1h5", "b8c6", "h5f7"};
	clock_t start, stop;
	double total_cpu_time;
	

	start_move[2] = '\0';
	end_move[2] = '\0';

	start = clock();
	initChessboard(chess_board);
	while (1) {
		deleteMoves();
		getAllMoves(chess_board, round);
		if (BlackKing == checkmate || WhiteKing == checkmate)
			break;
		start_move[0] = scholars_mate[total_rounds-1][0];
		start_move[1] = scholars_mate[total_rounds-1][1];
		end_move[0] = scholars_mate[total_rounds-1][2];
		end_move[1] = scholars_mate[total_rounds-1][3];

		if (makeMove(chess_board, start_move, end_move, round)) {
			round = (round == BLACK)?WHITE:BLACK;
			total_rounds++;
		}
	}
	stop = clock();
	total_cpu_time = ((double)(stop - start))/CLOCKS_PER_SEC;
	printf("CPU TIME ELAPSED:%lf\n\n", total_cpu_time);
	printBoard(chess_board, 'l');
	printf("\t\t\t***WHITE MOVES***\n");
	printWhiteMoves();
	printf("\n\t\t\t***BLACK MOVES***\n");
	printBlackMoves();
	if (!black_move_count)
		printf("White wins!\n");
	if (!white_move_count)
		printf("Black wins!\n");
	return 0;
}
 
 
