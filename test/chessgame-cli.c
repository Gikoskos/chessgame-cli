/********************************************************************
 *                          chessgame-cli.c                         *
 *       small game making use of the chesslib.c chess library      *
 *                                                                  *
 *                 (C)2015 <cyberchiller@gmail.com>                 *
 *                                                                  *
 ********************************************************************/

#include <chesslib.h>
#include <chlib-cli.h>


int main(int argc, char *argv[])
{
	ch_template chess_board[8][8];
	char *playerInput = NULL, start_move[3], end_move[3];
	initChessboard(chess_board);
	int round = WHITE, t_moves;
	bool pr_moves = false;


	start_move[2] = '\0';
	end_move[2] = '\0';
	while (1) {
		clear_screen();
		deleteMoves();
		if (round == WHITE)
			t_moves = getWhiteMoves(chess_board);
		else
			t_moves = getBlackMoves(chess_board);
		printBoard(chess_board, 'l');
		if (!t_moves) {
			printf("OMG U LOST ;\(\n");
			deleteMoves();
			break;
		}
		if (pr_moves) {
			printf("\t\t\t***BLACK MOVES***\n");
			printWhiteMoves();
			printf("\n\t\t\t***WHITE MOVES***\n");
			printBlackMoves();
			pr_moves = false;
		}
		printf("\nWHITE_MOVE_COUNT = %u\t BLACK_MOVE_COUNT = %u\n", white_move_count, black_move_count);
		if (round == WHITE)
			printf("White plays: ");
		else
			printf("Black plays: ");
		playerInput = getPlayerInput();
		if (strlen(playerInput) == 4) {
			if (!strncmp(playerInput, "quit", 4) || !strncmp(playerInput, "exit", 4)) {
				free(playerInput);
				playerInput = NULL;
				break;
			}
		} else if (strlen(playerInput) == 1) {
			pr_moves = true;
			free(playerInput);
			playerInput = NULL;
			continue;
		} else {
			free(playerInput);
			playerInput = NULL;
			continue;
		}
		start_move[0] = (char)toupper(playerInput[0]);
		start_move[1] = (char)toupper(playerInput[1]);
		end_move[0] = (char)toupper(playerInput[2]);
		end_move[1] = (char)toupper(playerInput[3]);

		if (makeMove(chess_board, start_move, end_move, round))
			round = (round == BLACK)?WHITE:BLACK;
		free(playerInput);
		playerInput = NULL;
	}
	
	return 0;
}
