/*******************************************************************
 *                         blackburnemate.c                        *
 *            blackburne-shilling mate to test chesslib            *
 *                                                                 *
 *                (C)2015 <cyberchiller@gmail.com>                 *
 *                                                                 *
 *******************************************************************/

#include <chesslib.h>
#include <chlib-cli.h>


int main(void)
{
	ch_template chess_board[8][8];
	int round = WHITE;
	clock_t start, stop;
	double total_cpu_time;

	start = clock();

	initChessboard(chess_board);
	playMoves(chess_board, &round, 14, "e2e4", "e7e5", "g1f3", "b8c6", "f1c4", "c6d4", "f3e5", 
		"d8g5", "e5f7", "g5g2", "h1f1", "g2e4", "c4e2", "d4f3");
	getAllMoves(chess_board, round);

	stop = clock();
	total_cpu_time = ((double)(stop - start))/CLOCKS_PER_SEC;
	printf("CPU TIME ELAPSED:%lf\n", total_cpu_time);
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
 
