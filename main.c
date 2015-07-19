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
	char playerInput[4], piece_to_move[2], clinearg[5], fn[s_l];
	int round = 0, roundcount = 1; //round: for each player's round, 1 for black, 2 for white, roundcount: total number of rounds
	bool gameover = false; //flag to control the game loop, becomes true when the King has no moves
	FILE *logfile;

	clear_screen();
	printf("\n\n\tWelcome to my Chess game!\n\n");
	initChessboard(chess_board, 0, 'A');
	printBoard(chess_board);
	date_filename(fn, s_l);

	while (gameover == false){
		if(roundcount%2 == 1)
			round = WHITE;
		else
			round = BLACK;

		if(argc > 1){
			if(strlen(argv[1]) == 4){
				strcpy(clinearg, argv[1]);
				if(strcmp(clinearg, "help") ==  0){
					printInstructions();
				}
				else 
				printf("Command line argument not recognized");
			}
			else 
				printf("Command line argument not recognized");
		}

		printf("Type 'hlp' and ENTER to view the instructions any time.\n");

		LOOP:do {
			if(round == BLACK) 
				printf("It\'s black\'s turn: ");
			else 
				printf("It\'s white\'s turn: ");
			fgets(playerInput, sizeof(playerInput), stdin);
			if(strcmp(playerInput, "hlp") == 0){
				printInstructions();
				clear_buffer();
			}
		} while(validInput(playerInput) == false);
		playerInput[1] = (char)toupper(playerInput[1]);

		if(findPiece(chess_board, playerInput, round) == NULL) {
			printf("Illegal move\n");
			clear_buffer();
			goto LOOP;
		}

		if(memcmp(findPiece(chess_board, playerInput, round), "q", 1) == 0){
			printBoard(chess_board);
			continue;
		}
		else if(findPiece(chess_board, playerInput, round) != NULL){
			memcpy(piece_to_move, findPiece(chess_board, playerInput, round), 2);
		}
		
		if(movePiece(chess_board, playerInput, piece_to_move, round) == false){
			printf("Illegal move\n");
			clear_buffer();
			goto LOOP;
		}
		if(!(logfile = fopen(fn, "a"))){
			printf("Log file could not be created\n");
		}
		else{
			write_to_log(round, logfile, playerInput, piece_to_move);
		}
		fclose(logfile);
		clear_screen();
		clear_buffer();
		roundcount++;
		printf("\n\n\n\n");
		printBoard(chess_board);
	}
	return 0;
}