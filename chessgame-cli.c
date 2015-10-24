/********************************************************************
 *                         chessgame-cli.c                          *
 *       small game making use of the chesslib.c chess library      *
 *                                                                  *
 *               (C)2015 <georgekoskerid@outlook.com>               *
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
	int round = 0, roundcount = 1, p_err = 0, loop_count = 1, argtmp;
	/*round: for each player's round, 1 for black, 2 for white
	*roundcount: total number of rounds
	*p_err: holds the position of the error_out array
	*loop_count: counter for LOOP
	*argtmp: temporary storage for command line arguments*/
	KingState white_king = safe, black_king = safe;
	FILE *logfile;

	do {
		argtmp = getopt(argc, argv, "c");
		if (argtmp == 'c')
			break;
	} while (argtmp != -1);
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
			if (white_king == check || white_king == safe_check) {
				if (!WKingMoves)
					printf("White King can't move.\n");
				else {
					if (white_king == check)
						printf("White King is in danger!\n");
#ifndef NDEBUG
					printf("Possible moves for white King: %s\n", WKingMoves);
#endif
				}
			}
			if (black_king == check || black_king == safe_check) {
				if (!BKingMoves)
					printf("Black King can't move.\n");
				else {
					if (black_king == check)
						printf("Black King is in danger!\n");
#ifndef NDEBUG
					printf("Possible moves for black King: %s\n", BKingMoves);
#endif
				}
			}
			if (round == BLACK) {
				if (AI_IS_ENABLED) {
					memcpy(playerInput, AImove(chess_board), 4);
#ifndef NDEBUG
					printf("%s ", playerInput);
#endif
				}
				else {
					printf("It\'s black\'s turn: ");
					playerInput = getPlayerInput();
				}
			} else {
				printf("It\'s white\'s turn: ");
				playerInput = getPlayerInput();
			}

			if (!playerInput) {	/*avoid segfaulting for strlen(NULL) later*/
				loop_count++;
				continue;
			}
			if (!strcmp(playerInput, "quit") || !strcmp(playerInput, "exit")
				|| !strcmp(playerInput, "QUIT") || !strcmp(playerInput, "EXIT")) {
				goto ENDGAME;
			}
			if (strlen(playerInput) > 4 || playerInput[1] == '\0') {	/*change error code for bad input*/
				loop_count++;
#if !defined (__MINGW32__) || !defined(_WIN32)
				if (!strncmp(playerInput, "pieces", 7) || !strncmp(playerInput, "PIECES", 7)) {
					chbflag = 'p';
					p_err = 0;
					goto LOOP;
				}
				if (!strncmp(playerInput, "letters", 8) || !strncmp(playerInput, "LETTERS", 8)) {
					chbflag = 'a';
					p_err = 0;
					goto LOOP;
				}
#endif
				if (!strncmp(playerInput, "version", 8) || !strncmp(playerInput, "VERSION", 8)) {
					p_err = 8;
					goto LOOP;
				}
				p_err = 2;
				goto LOOP;
			}
			loop_count++;
		} while (validInput(playerInput, &p_err) == false);
		playerInput[0] = (char)toupper(playerInput[0]);
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

		if (white_king == check  || black_king == check || 
			(playerInput[0] == 'K' && (white_king == safe_check || black_king == safe_check))) {
			if (!isCheckMoveValid(chess_board, playerInput, piece_to_move, round)) {
				p_err = 3;
				goto LOOP;
			}
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
