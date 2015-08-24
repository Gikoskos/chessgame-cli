/*  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

/********************************************************************
 *                             main.c                               *
 *       small game making use of the chesslib.c chess library      *
 *                                                                  *
 *                 (C)2015 <cyberchiller@gmail.com>                 *
 *                                                                  *
 ********************************************************************/

#include "chesslib.h"

int main(int argc, char *argv[])
{
	ch_template chess_board[8][8];
	char *playerInput = NULL, piece_to_move[2], fn[s_l], attack_guard[5], temp_cpiece, chbflag = 'a';
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
	KingState white_king = safe, black_king = safe;
	FILE *logfile;

	initChessboard(chess_board);
	date_filename(fn, s_l);
	clear_screen();

	while (1) {
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

		printf("Type 'help' and ENTER to view the instructions any time.\n");

		LOOP:
		do {
			if (loop_count > 1) {
				clear_screen();
				printf("\n\n\n\n\n\n");
				printBoard(chess_board, chbflag);
				if (!strcmp(playerInput, "help")) {
					printInstructions();
					p_err = 0;
				}
			} else 
				printBoard(chess_board, chbflag);
			printError(p_err);
			if (white_king == checkmate) {
				printf("Black player wins! Thanks for playing!\n\t\t\n");
				goto ENDGAME;
			} else if (black_king == checkmate) {
				printf("White player wins! Thanks for playing!\n\t\t\n");
				goto ENDGAME;
			}
			if (white_king == check) {
				if (!WKingMoves)
					printf("White King can't move.\n");
				else
					printf("Possible moves for white King: %s\n", WKingMoves);
			}
			if (black_king == check) {
				if (!BKingMoves)
					printf("Black King can't move.\n");
				else
					printf("Possible moves for black King: %s\n", BKingMoves);
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
#if !defined (__MINGW32__) || !defined(_WIN32)
				if (strncmp(playerInput, "pieces", 6) == 0) {
					chbflag = 'p';
					loop_count++;
					continue;
				} else if (strncmp(playerInput, "letters", 7) == 0) {
					chbflag = 'a';
					loop_count++;
					continue;
				}
#endif
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
		if (cstl_is_enabled) {
			setCastling(chess_board, playerInput, round);
			goto LOG;
		}
		strncpy(attack_guard,findPiece(chess_board, playerInput, round), 4);
		if (strlen(attack_guard) < 3) {
			memcpy(piece_to_move, attack_guard, 2);
		} else if (piecesOverlap(chess_board, (attack_guard[1]-'1'), (attack_guard[0]-'A'),
				(playerInput[2]-'1'), (playerInput[1]-'A'), playerInput[0]) == true && playerInput[0] != 'N') {
			piece_to_move[0] = attack_guard[2];
			piece_to_move[1] = attack_guard[3];
		} else if (piecesOverlap(chess_board, (attack_guard[3]-'1'), (attack_guard[2]-'A'),
				(playerInput[2]-'1'), (playerInput[1]-'A'), playerInput[0]) == true && playerInput[0] != 'N') {
			memcpy(piece_to_move, attack_guard, 2);
		} else {
			temp_cpiece = playerInput[0];
			memcpy(piece_to_move, pieceConflict(attack_guard, temp_cpiece), 2);
		}

		if (movePiece(chess_board, playerInput, piece_to_move, round) == false) {
			p_err = 3;
			goto LOOP;
		}
		LOG:
		if (!(logfile = fopen(fn, "a"))) {
			printError(3);
		} else {
			if (cstl_is_enabled) {
				cstl_is_enabled = false;
				if (playerInput[1] == 'C' || playerInput[1] == 'D')
					write_to_log(round, logfile, playerInput, CSTL_LEFTROOK);
				else
					write_to_log(round, logfile, playerInput, CSTL_RIGHTROOK);
			} else {
				write_to_log(round, logfile, playerInput, piece_to_move);
			}
		}
		fclose(logfile);
		free(playerInput);
		roundcount++;
		p_err = 0;
		loop_count = 2;
		findKState(chess_board, &white_king, &black_king);
	}
	ENDGAME:
	if (white_king == checkmate || black_king == checkmate) {
#if !defined(__MINGW32__) || !defined(_WIN32)
		sleep(4);
#else
		Sleep(4000);
#endif
	}
	playerInput = NULL;
	return 0;
}
