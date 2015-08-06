/* This program is free software. It comes without any warranty, to
	* the extent permitted by applicable law. You can redistribute it
	* and/or modify it under the terms of the Do What The Fuck You Want
	* To Public License, Version 2, as published by Sam Hocevar. See
	* http://www.wtfpl.net/ for more details. */
	
          
/********************************************************************/
/*                             main.c                               */
/*       small game making use of the chesslib.c chess engine       */
/*                                                                  */
/*                    by cyberchiller@gmail.com                     */
/*                                                                  */
/********************************************************************/

#include "chesslib.h"

int main(int argc, char *argv[])
{
	ch_template chess_board[8][8];
	char *playerInput = NULL, piece_to_move[2], fn[s_l], attack_guard[5], temp_cpiece;
		/*playerInput: input from stdin
		 *piece_to_move: the final piece to move
		 *fn: file name string with s_l length
		 *attack_guard: double pawn attack guard and temporary storage for the return of findPiece
		 *temp_cpiece: value to store the current piece name in use with the pieceConflict function*/
	int round = 0, roundcount = 1, p_err = 0, loop_count = 1;
		/*round: for each player's round, 1 for black, 2 for white
		 *roundcount: total number of rounds
		 *p_err: holds the position of the error_out array
		 *loop_count: counter for LOOP*/
	bool gameover = false;
		/*flag to control the game loop, becomes true when the King has no moves*/
	KingState tempK = safe;
	FILE *logfile;

	initChessboard(chess_board, 0, 'A');
	date_filename(fn, s_l);
	clear_screen();

	while (gameover == false) {
		if (roundcount == 1) {
			printf("\n");
			printBanner("Welcome to my Chess game!");
			printf("\n");
			round = WHITE;
		} else {
			if (roundcount%2 == 1) 
				round = WHITE;
			else 
				round = BLACK;
			printf("\n\n\n\n\n");
		}
		if (argc > 1) {
			if (strcmp(argv[1], "help") ==  0) {
				printInstructions();
			} else 
				p_err = 1;
		}

		printf("Type 'help' and ENTER to view the instructions any time.\n");

		LOOP:do {
			if (loop_count > 1) {
				clear_screen();
				printf("\n\n\n\n\n\n");
				printBoard(chess_board);
				if (!strcmp(playerInput, "help")) {
					printInstructions();
					p_err = 0;
				}
			} else 
				printBoard(chess_board);
			printError(p_err);
			if (tempK == checkW) {
				printf("White King is in danger!\n");
			} else if (tempK == checkB) {
				printf("Black King is in danger!\n");
			}
			if (round == BLACK) 
				printf("It\'s black\'s turn: ");
			else 
				printf("It\'s white\'s turn: ");
			playerInput = getPlayerInput();
			
			if (!playerInput) {	/*avoid segfaulting for strlen(NULL) later*/
				loop_count++;
				continue;
			}
			if (!strcmp(playerInput, "quit") || !strcmp(playerInput, "exit")) {
				goto ENDGAME;
			}
			if (strlen(playerInput) > 4 || playerInput[0] == '\n') {	/*change error code for bad input*/
				p_err = 2;
				loop_count++;
				continue;
			}
			loop_count++;
		} while (validInput(playerInput, &p_err) == false);
		playerInput[1] = (char)toupper(playerInput[1]);
		
		if (!findPiece(chess_board, playerInput, round)) {
			p_err = 3;
			goto LOOP;
		}
		strcpy(attack_guard,findPiece(chess_board, playerInput, round));
		if (strlen(attack_guard) < 3) {
			memcpy(piece_to_move, attack_guard, 2);
		} else {
			temp_cpiece = playerInput[0];
			memcpy(piece_to_move, pieceConflict(attack_guard, temp_cpiece), 2);
		}
		
		if (movePiece(chess_board, playerInput, piece_to_move, round) == false) {
			p_err = 3;
			goto LOOP;
		}
		if (!(logfile = fopen(fn, "a"))) {
			printError(3);
		} else {
			write_to_log(round, logfile, playerInput, piece_to_move);
		}
		fclose(logfile);
		free(playerInput);
		roundcount++;
		p_err = 0;
		loop_count = 2;
		//gameover = check(chess_board);
		tempK = findKState(chess_board);
	}
	ENDGAME:
	playerInput = NULL;
	return 0;
}
