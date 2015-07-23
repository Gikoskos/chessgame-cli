/* This program is free software. It comes without any warranty, to
	* the extent permitted by applicable law. You can redistribute it
	* and/or modify it under the terms of the Do What The Fuck You Want
	* To Public License, Version 2, as published by Sam Hocevar. See
	* http://www.wtfpl.net/ for more details. */
	
          
/********************************************************************/
/*                                                                  */
/* standard chess engine no AI or complicated rules implemented yet */
/*                                                                  */
/*                       by g.k.                                    */
/*                                                                  */
/********************************************************************/


#include "chesslib.h"


int main(int argc, char *argv[])
{
	ch_template chess_board[8][8];
	char *playerInput = NULL, piece_to_move[2], fn[s_l], attack_guard[5];
		/*playerInput: input from stdin
		 *piece_to_move: the final piece to move
		 *fn: file name string with s_l length
		 *attack_guard: double pawn attack guard and temporary storage for the return of findPiece*/
	int round = 0, roundcount = 1;
		/*round: for each player's round, 1 for black, 2 for white
		 *roundcount: total number of rounds*/
	bool gameover = false;
		/*flag to control the game loop, becomes true when the King has no moves*/
	FILE *logfile;

	clear_screen();
	printf("\n");
	printBanner("Welcome to my Chess game!");
	printf("\n");
	initChessboard(chess_board, 0, 'A');
	printBoard(chess_board);
	date_filename(fn, s_l);

	while (gameover == false) {
		if (roundcount%2 == 1)
			round = WHITE;
		else
			round = BLACK;

		if (argc > 1) {
			if (strcmp(argv[1], "help") ==  0) {
				printInstructions();
			} else 
				fprintf(stderr, "Command line argument not recognized\n");
		}

		printf("Type 'help' and ENTER to view the instructions any time.\n");

		LOOP:do {
			if (round == BLACK) 
				printf("It\'s black\'s turn: ");
			else 
				printf("It\'s white\'s turn: ");
			playerInput = getPlayerInput();
			if (!playerInput)
				continue;
			if (strlen(playerInput) > 4) {
				fprintf(stderr, "Bad input\n");
				continue;	/*weird things happen if I change this continue to a goto, on my system I get SIGSEV*/
			}
		} while (validInput(playerInput) == false);
		playerInput[1] = (char)toupper(playerInput[1]);
		
		if (!findPiece(chess_board, playerInput, round)) {
			fprintf(stderr, "Illegal move\n");
			goto LOOP;
		}
		strcpy(attack_guard,findPiece(chess_board, playerInput, round));
		if(memcmp(findPiece(chess_board, playerInput, round), "q", 1) == 0) {
			printBoard(chess_board);
			continue;
		} else if(strlen(attack_guard) < 3) {
			memcpy(piece_to_move, attack_guard, 2);
		} else {
			memcpy(piece_to_move, pawnConflict(attack_guard), 2);
		}
		
		if (movePiece(chess_board, playerInput, piece_to_move, round) == false) {
			fprintf(stderr, "Illegal move\n");
			goto LOOP;
		}
		if (!(logfile = fopen(fn, "a"))) {
			fprintf(stderr, "Log file could not be created\n");
		} else {
			write_to_log(round, logfile, playerInput, piece_to_move);
		}
		fclose(logfile);
		clear_screen();
		roundcount++;
		printf("\n\n\n\n\n");
		printBoard(chess_board);
	}
	playerInput = NULL;
	return 0;
}
