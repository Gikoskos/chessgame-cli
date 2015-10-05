/*********************************************************************
 *                            chesslib.c                             *
 *                standard chess game implementation                 *
 *                                                                   *
 *                 (C)2015 <cyberchiller@gmail.com>                  *
 *                                                                   *
 *********************************************************************/

#include "chesslib.h"


/*********
 *globals*
 *********/

static unsigned __attribute__((unused)) w_enpassant_round_left = 0;
static unsigned __attribute__((unused)) w_enpassant_round_right = 0;
static unsigned __attribute__((unused)) b_enpassant_round_left = 0;
static unsigned __attribute__((unused)) b_enpassant_round_right = 0;
static bool __attribute__((unused)) enpassant = false;

static CastlingBool check_castling = {true, true, true, true, true, true};
static unsigned rc = 0;

unsigned black_move_count;
unsigned white_move_count;

MoveNode *b_moves[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
MoveNode *w_moves[6] = {NULL, NULL, NULL, NULL, NULL, NULL};


/********************************************
 *prototypes for functions used only in here*
 ********************************************/

bool piecesOverlap(ch_template chb[][8], const int start_x, const int start_y,
		const int end_x, const int end_y, const char piece);
void removeThreatsToKing(ch_template chb[][8], int c_flag);
bool isKingOnTheBoard(ch_template chb[][8], int color);



void addNode(MoveNode **llt, const char *st, const char *en)
{
	if (!*llt) {
		*llt = malloc(sizeof(MoveNode));
		strncpy((*llt)->start, st, 4);
		strncpy((*llt)->end, en, 4);
		(*llt)->nxt = NULL;
		return;
	}
	MoveNode *curr = *llt, *new = malloc(sizeof(MoveNode));
	while (curr->nxt)
		curr = curr->nxt;
	strncpy(new->start, st, 4);
	strncpy(new->end, en, 4);
	new->nxt = NULL;
	curr->nxt = new;
}

#ifdef DEBUG
void printMoveList(MoveNode *llt, FILE *fd)
{
	if (!llt) {
		fprintf(stderr, "No moves.\n");
		return;
	}
	MoveNode *curr = llt;
	short unsigned i = 0;
	while (i++, curr) {
		fprintf(fd, "%s -> %s", curr->start, curr->end);
		curr = curr->nxt;
		if (!(i%5) && curr)
			printf("\n");
		else if (curr)
			printf("     ");
	}
	printf("\n");
}
#endif

void deleteMoveList(MoveNode **llt)
{
	MoveNode *curr = (*llt);
	while ((*llt)) {
		(*llt) = (*llt)->nxt;
		free(curr);
		curr = (*llt);
	}
}

bool isOnList(const char *start_move, const char *end_move, const char piece, const int color)
{
	unsigned short idx;
	MoveNode *curr;
	if (end_move && start_move) {
		switch (piece) {
			case PAWN:
				idx = 0;
				break;
			case KING:
				idx = 1;
				break;
			case QUEEN:
				idx = 2;
				break;
			case ROOK:
				idx = 3;
				break;
			case KNIGHT:
				idx = 4;
				break;
			case BISHOP:
				idx = 5;
				break;
			default:
				return false;
		}
		switch (color) {
			case BLACK:
				curr = b_moves[idx];
				break;
			case WHITE:
				curr = w_moves[idx];
				break;
			default:
				return false;
		}
		while (curr) {
			if (!strncmp(end_move, curr->end, 2) && !strncmp(start_move, curr->start, 2)) {
				return true;
			}
			curr = curr->nxt;
		}
	}
	return false;
}

__attribute__((destructor)) void deleteMoves()
{
	deleteBlackMoves();
	deleteWhiteMoves();
	for (int i = 0; i < 6; i++) {
		b_moves[i] = NULL;
		w_moves[i] = NULL;
	}
}

void _initChessboard(ch_template chb[][8], unsigned k, char col)	/*k is row, col is column*/
{ 
	if (k == 0 || k == 7) {
		if (col == 'A' || col == 'H')
			chb[k][col - 'A'].current = 'R';
		else if (col == 'B' || col == 'G')
			chb[k][col - 'A'].current = 'N';
		else if (col == 'C' || col == 'F')
			chb[k][col - 'A'].current = 'B';
		else if (col == 'D')
			chb[k][col - 'A'].current = 'Q';
		else
			chb[k][col - 'A'].current = 'K';
		if (k == 7)
			chb[k][col - 'A'].c = WHITE;	/*colorize the pieces*/
		else
			chb[k][col - 'A'].c = BLACK;
		chb[k][col - 'A'].occ = true;
		chb[k][col - 'A'].square[0] = col;
		chb[k][col - 'A'].square[1] = 7 - k + '1';
		col = col + 1;
	} else if (k == 1 || k == 6) {
		if(k == 6) 
			chb[k][col - 'A'].c = WHITE;
		else
			chb[k][col - 'A'].c = BLACK;
		chb[k][col - 'A'].current = 'P';
		chb[k][col - 'A'].occ = true;
		chb[k][col - 'A'].square[0] = col;
		chb[k][col - 'A'].square[1] = 7 - k + '1';
		col = col + 1;
	} else if (k >= 2 && k <= 5) {
		chb[k][col - 'A'].c = EMPTY;
		chb[k][col - 'A'].current = 'e';
		chb[k][col - 'A'].occ = false;
		chb[k][col - 'A'].square[0] = col;
		chb[k][col - 'A'].square[1] = 7 - k + '1';
		col = col + 1;
	}
	if (col == 'I') {
		++k;
		col = 'A';
	}
	if (k != 8)
		_initChessboard(chb, k, col);
}

int _getMoveList(ch_template chb[][8], int c_flag)
{
	int i, j, k, l, move_count = 0;
	char t_st[3], t_en[3];

	black_move_count = 0;
	white_move_count = 0;
	t_en[2] = t_st[2] = '\0';
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			t_st[0] = chb[i][j].square[0];
			t_st[1] = chb[i][j].square[1];
			if (chb[i][j].current == PAWN && chb[i][j].c == c_flag) {
				if (chb[i][j].c == BLACK) {
					if (i == 1 && !(chb[i+2][j].occ) && !(chb[i+1][j].occ)) {
						t_en[0] = chb[i+2][j].square[0];
						t_en[1] = chb[i+2][j].square[1];
						addNode(&b_moves[0], t_st, t_en);
						black_move_count++;
						move_count++;
					}
					/*if (i == 3) {
						if (w_enpassant_round_left) {
							//if (chb[i][j+1].current == 'P' && chb[i][j+1].c == WHITE) {
								t_en[0] = chb[i+1][j+1].square[0];
								t_en[1] = chb[i+1][j+1].square[1];
								addNode(&b_moves[0], t_st, t_en);
								black_move_count++;
								move_count++;
							//}
						} else if (w_enpassant_round_right) {
							//if (chb[i][j-1].current == 'P' && chb[i][j-1].c == WHITE) {
								t_en[0] = chb[i+1][j-1].square[0];
								t_en[1] = chb[i+1][j-1].square[1];
								addNode(&b_moves[0], t_st, t_en);
								black_move_count++;
								move_count++;
							//}
						}
					}*/
					t_en[0] = chb[i+1][j].square[0];
					if (chb[i+1][j].occ == false) {
						t_en[1] = chb[i+1][j].square[1];
						addNode(&b_moves[0], t_st, t_en);
						black_move_count++;
						move_count++;
					}
					if (chb[i+1][j+1].c == WHITE) {
						t_en[1] = chb[i+1][j+1].square[1];
						addNode(&b_moves[0], t_st, t_en);
						black_move_count++;
						move_count++;
					} 
					if (chb[i+1][j-1].c == WHITE) {
						t_en[1] = chb[i+1][j-1].square[1];
						addNode(&b_moves[0], t_st, t_en);
						black_move_count++;
						move_count++;
					}
				} else {
					if (i == 6 && !(chb[i-2][j].occ) && !(chb[i-1][j].occ)) {
						t_en[0] = chb[i-2][j].square[0];
						t_en[1] = chb[i-2][j].square[1];
						addNode(&w_moves[0], t_st, t_en);
						white_move_count++;
						move_count++;
					}
					/*if (i == 4) {
						if (b_enpassant_round_left) {
							//if (chb[i][j+1].current == 'P' && chb[i][j+1].c == BLACK) {
								t_en[0] = chb[i-1][j+1].square[0];
								t_en[1] = chb[i-1][j+1].square[1];
								addNode(&w_moves[0], t_st, t_en);
								white_move_count++;
								move_count++;
							//}
						} else if (b_enpassant_round_right) {
							//if (chb[i][j-1].current == 'P' && chb[i][j-1].c == BLACK) {
								t_en[0] = chb[i-1][j-1].square[0];
								t_en[1] = chb[i-1][j-1].square[1];
								addNode(&w_moves[0], t_st, t_en);
								white_move_count++;
								move_count++;
							//}
						}
					}*/
					t_en[0] = chb[i-1][j].square[0];
					if (chb[i-1][j].occ == false) {
						t_en[1] = chb[i-1][j].square[1];
						addNode(&w_moves[0], t_st, t_en);
						white_move_count++;
						move_count++;
					}
					if (chb[i-1][j+1].c == BLACK) {
						t_en[1] = chb[i-1][j+1].square[1];
						addNode(&w_moves[0], t_st, t_en);
						white_move_count++;
						move_count++;
					} 
					if (chb[i-1][j-1].c == BLACK) {
						t_en[1] = chb[i-1][j-1].square[1];
						addNode(&w_moves[0], t_st, t_en);
						white_move_count++;
						move_count++;
					}
				}
			}
			if ((chb[i][j].current == ROOK || chb[i][j].current == QUEEN) && chb[i][j].c == c_flag) {
				k = i;
				for (l = j+1; l <= 7; l++) {
					if (chb[k][l].c == chb[i][j].c)
						break;
					t_en[0] = chb[k][l].square[0];
					t_en[1] = chb[k][l].square[1];
					if (chb[i][j].c == BLACK) {
						if (chb[i][j].current == 'R') {
							addNode(&b_moves[3], t_st, t_en);
							black_move_count++;
							move_count++;
						} else {
							addNode(&b_moves[2], t_st, t_en);
							black_move_count++;
							move_count++;
						}
					} else {
						if (chb[i][j].current == 'R') {
							addNode(&w_moves[3], t_st, t_en);
							white_move_count++;
							move_count++;
						} else {
							addNode(&b_moves[2], t_st, t_en);
							white_move_count++;
							move_count++;
						}
					}
					if (chb[k][l].occ)
						break;
				}
				for (l = j-1; l >= 0; l--) {
					if (chb[k][l].c == chb[i][j].c)
						break;
					t_en[0] = chb[k][l].square[0];
					t_en[1] = chb[k][l].square[1];
					if (chb[i][j].c == BLACK) {
						if (chb[i][j].current == 'R') {
							addNode(&b_moves[3], t_st, t_en);
							black_move_count++;
							move_count++;
						} else {
							addNode(&b_moves[2], t_st, t_en);
							black_move_count++;
							move_count++;
						}
					} else {
						if (chb[i][j].current == 'R') {
							addNode(&w_moves[3], t_st, t_en);
							white_move_count++;
							move_count++;
						} else {
							addNode(&b_moves[2], t_st, t_en);
							white_move_count++;
							move_count++;
						}
					}
					if (chb[k][l].occ)
						break;
				}
				
				l = j;
				for (k = i+1; k <= 7; k++) {
					if (chb[k][l].c == chb[i][j].c)
						break;
					t_en[0] = chb[k][l].square[0];
					t_en[1] = chb[k][l].square[1];
					if (chb[i][j].c == BLACK) {
						if (chb[i][j].current == 'R') {
							addNode(&b_moves[3], t_st, t_en);
							black_move_count++;
							move_count++;
						} else {
							addNode(&b_moves[2], t_st, t_en);
							black_move_count++;
							move_count++;
						}
					} else {
						if (chb[i][j].current == 'R') {
							addNode(&w_moves[3], t_st, t_en);
							white_move_count++;
							move_count++;
						} else {
							addNode(&b_moves[2], t_st, t_en);
							white_move_count++;
							move_count++;
						}
					}
					if (chb[k][l].occ)
						break;
				}
				for (k = i-1; k >= 0; k--) {
					if (chb[k][l].c == chb[i][j].c)
						break;
					t_en[0] = chb[k][l].square[0];
					t_en[1] = chb[k][l].square[1];
					if (chb[i][j].c == BLACK) {
						if (chb[i][j].current == 'R') {
							addNode(&b_moves[3], t_st, t_en);
							black_move_count++;
							move_count++;
						} else {
							addNode(&b_moves[2], t_st, t_en);
							black_move_count++;
							move_count++;
						}
					} else {
						if (chb[i][j].current == 'R') {
							addNode(&w_moves[3], t_st, t_en);
							white_move_count++;
							move_count++;
						} else {
							addNode(&b_moves[2], t_st, t_en);
							white_move_count++;
							move_count++;
						}
					}
					if (chb[k][l].occ)
						break;
				}
			}
			if ((chb[i][j].current == BISHOP || chb[i][j].current == 'Q') && chb[i][j].c == c_flag) {
				k = i - 1; 
				l = j - 1;
				while ((k <= 7 && k >= 0) && (l >= 0 && l <= 7)) {
					if (chb[k][l].c != chb[i][j].c) {
						t_en[0] = chb[k][l].square[0];
						t_en[1] = chb[k][l].square[1];
						if (chb[i][j].c == BLACK) {
							if (chb[i][j].current == 'B') {
								addNode(&b_moves[5], t_st, t_en);
								black_move_count++;
								move_count++;
							} else {
								addNode(&b_moves[2], t_st, t_en);
								black_move_count++;
								move_count++;
							}
						} else {
							if (chb[i][j].current == 'B') {
								addNode(&w_moves[5], t_st, t_en);
								white_move_count++;
								move_count++;
							} else {
								addNode(&b_moves[2], t_st, t_en);
								white_move_count++;
								move_count++;
							}
						}
					}
					if (chb[k][l].occ)
						break;
					k--;
					l--;
				}
				k = i - 1;
				l = j + 1;
				while ((k >= 0 && k <= 7) && (l <= 7 && l >= 0)) {
					if (chb[k][l].c != chb[i][j].c) {
						t_en[0] = chb[k][l].square[0];
						t_en[1] = chb[k][l].square[1];
						if (chb[i][j].c == BLACK) {
							if (chb[i][j].current == 'B') {
								addNode(&b_moves[5], t_st, t_en);
								black_move_count++;
								move_count++;
							} else {
								addNode(&b_moves[2], t_st, t_en);
								black_move_count++;
								move_count++;
							}
						} else {
							if (chb[i][j].current == 'B') {
								addNode(&w_moves[5], t_st, t_en);
								white_move_count++;
								move_count++;
							} else {
								addNode(&b_moves[2], t_st, t_en);
								white_move_count++;
								move_count++;
							}
						}
					}
					if (chb[k][l].occ)
						break;
					k--;
					l++;
				} 
				k = i + 1;
				l = j - 1;
				while ((k <= 7 && k >= 0) && (l >= 0 && l <= 7)) {
					if (chb[k][l].c != chb[i][j].c) {
						t_en[0] = chb[k][l].square[0];
						t_en[1] = chb[k][l].square[1];
						if (chb[i][j].c == BLACK) {
							if (chb[i][j].current == 'B') {
								addNode(&b_moves[5], t_st, t_en);
								black_move_count++;
								move_count++;
							} else {
								addNode(&b_moves[2], t_st, t_en);
								black_move_count++;
								move_count++;
							}
						} else {
							if (chb[i][j].current == 'B') {
								addNode(&w_moves[5], t_st, t_en);
								white_move_count++;
								move_count++;
							} else {
								addNode(&b_moves[2], t_st, t_en);
								white_move_count++;
								move_count++;
							}
						}
					}
					if (chb[k][l].occ)
						break;
					k++;
					l--;
				}
				k = i + 1;
				l = j + 1;
				while ((k <= 7 && k >= 0) && (l <= 7 && l >= 0)) {
					if (chb[k][l].c != chb[i][j].c) {
						t_en[0] = chb[k][l].square[0];
						t_en[1] = chb[k][l].square[1];
						if (chb[i][j].c == BLACK) {
							if (chb[i][j].current == 'B') {
								addNode(&b_moves[5], t_st, t_en);
								black_move_count++;
								move_count++;
							} else {
								addNode(&b_moves[2], t_st, t_en);
								black_move_count++;
								move_count++;
							}
						} else {
							if (chb[i][j].current == 'B') {
								addNode(&w_moves[5], t_st, t_en);
								white_move_count++;
								move_count++;
							} else {
								addNode(&b_moves[2], t_st, t_en);
								white_move_count++;
								move_count++;
							}
						}
					}
					if (chb[k][l].occ)
						break;
					k++;
					l++;
				}
			}
			if (chb[i][j].current == KING && chb[i][j].c == c_flag) {
				for (k = i - 1; k < i + 2; k++){
					for (l = j - 1; l < j + 2; l++){
						if (k > 7 || k < 0 || l > 7 || l < 0)
							continue;
						if ((i == k && j == l) || chb[k][l].c == chb[i][j].c)
							continue;
						t_en[0] = chb[k][l].square[0];
						t_en[1] = chb[k][l].square[1];
						if (chb[i][j].c == BLACK) {
							addNode(&b_moves[1], t_st, t_en);
							black_move_count++;
							move_count++;
						} else {
							addNode(&w_moves[1], t_st, t_en);
							white_move_count++;
							move_count++;
						}
					}
				}
			}
			if (chb[i][j].current == KNIGHT && chb[i][j].c == c_flag) {
				int knightrow[] = {i-2,i-2,i-1,i-1,i+1,i+1,i+2,i+2};
				int knightcol[] = {j-1,j+1,j-2,j+2,j-2,j+2,j-1,j+1};
				int count = 0;
				for (; count < 8; count++) {
					if (knightrow[count] > -1 && knightrow[count] < 8
						&& knightcol[count] > -1 && knightcol[count] < 8) {
						if (chb[knightrow[count]][knightcol[count]].c != chb[i][j].c) {
							t_en[0] = chb[knightrow[count]][knightcol[count]].square[0];
							t_en[1] = chb[knightrow[count]][knightcol[count]].square[1];
							if (chb[i][j].c == BLACK) {
								addNode(&b_moves[4], t_st, t_en);
								black_move_count++;
								move_count++;
							} else {
								addNode(&w_moves[4], t_st, t_en);
								white_move_count++;
								move_count++;
							}
						}
					}
				}
			}
			if (check_castling.KBlack) {
				if (check_castling.BR_right && !piecesOverlap(chb, 7, ('H'-'A'), 7, 4, 'R')) {
				}
				if (check_castling.BR_left && !piecesOverlap(chb, 7, ('A'-'A'), 7, 4, 'R')) {
				}
			}
		}
	}
	removeThreatsToKing(chb, c_flag);
	return move_count;
}

bool makeMove(ch_template chb[][8], char *st_move, char *en_move, const int color)
{
	if (!en_move || !st_move)
		return false;

	unsigned short startx, starty, endx, endy;
	startx = st_move[0] - 'A';
	starty = '8' - st_move[1];
	endx = en_move[0] - 'A';
	endy = '8' - en_move[1];

	char piece = chb[starty][startx].current;
	if (piece == 'e')
		return false;

	if (!isOnList(st_move, en_move, piece, color))
		return false;

	if (piecesOverlap(chb, startx, starty, endx, endy, piece) || chb[endy][endx].c == color)
		return false;
	
	if (piece == ROOK) {
		if (startx == 0) {
			if (starty == 0)
				check_castling.BR_left = false;
			else if (starty == 7)
				check_castling.BR_right = false;
		} else if (startx == 7) {
			if (starty == 0)
				check_castling.WR_left = false;
			else if (starty == 7)
				check_castling.WR_right = false;
		}
	}

	if (piece == KING) {
		if (color == BLACK)
			check_castling.KBlack = false;
		else
			check_castling.KWhite = false;
	}

	if (color == WHITE) rc++;
	/*b_enpassant_round_left = 0;
	b_enpassant_round_right = 0;
	w_enpassant_round_left = 0;
	w_enpassant_round_right = 0;
	if (piece == PAWN) {
		if (color == BLACK) {
			if (starty == 1 && endy == 3) {
				if (chb[endy][endx+1].current == 'P' && chb[endy][endx+1].c == WHITE) {
					b_enpassant_round_right = rc;
					enpassant = true;
				}
				if (chb[endy][endx-1].current == 'P' && chb[endy][endx-1].c == WHITE) {
					b_enpassant_round_left = rc;
					enpassant = true;
				}
			}
		} else {
			if (starty == 6 && endy == 4) {
				if (chb[endy][endx+1].current == 'P' && chb[endy][endx+1].c == BLACK) {
					w_enpassant_round_right = rc;
					enpassant = true;
				}
				if (chb[endy][endx-1].current == 'P' && chb[endy][endx-1].c == BLACK) {
					w_enpassant_round_left = rc;
					enpassant = true;
				}
			}
		}
	}*/

	chb[endy][endx].occ = true;
	chb[endy][endx].current = chb[starty][startx].current;
	chb[endy][endx].c = color;
	chb[starty][startx].occ = false;
	chb[starty][startx].c = EMPTY;
	chb[starty][startx].current = 'e';
	return true;
}

void removeThreatsToKing(ch_template chb[][8], int c_flag)
{
	ch_template next_chb[8][8];
	
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			next_chb[i][j] = chb[i][j];
		}
	}
	if (c_flag == BLACK) {
		for (int i = 0; i < 6; i++) {
			MoveNode *curr = b_moves[i], *prv = NULL;
			while (curr) {
				makeMove(next_chb, curr->start, curr->end, BLACK);
				if (!isKingOnTheBoard(next_chb, WHITE)) {
					if (prv)
						prv->nxt = curr->nxt;
					prv = curr;
					curr = curr->nxt;
					free(curr);
					for (int y = 0; y < 8; y++) {
						for (int z = 0; z < 8; z++) {
							next_chb[y][z] = chb[y][z];
						}
					}
					continue;
				}
				for (int y = 0; y < 8; y++) {
					for (int z = 0; z < 8; z++) {
						next_chb[y][z] = chb[y][z];
					}
				}
				prv = curr;
				curr = curr->nxt;
			}
		}
	} else {
		for (int i = 0; i < 6; i++) {
			MoveNode *curr = w_moves[i], *prv = NULL;
			while (curr) {
				makeMove(next_chb, curr->start, curr->end, WHITE);
				if (!isKingOnTheBoard(next_chb, BLACK)) {
					if (prv)
						prv->nxt = curr->nxt;
					prv = curr;
					curr = curr->nxt;
					free(curr);
					for (int y = 0; y < 8; y++) {
						for (int z = 0; z < 8; z++) {
							next_chb[y][z] = chb[y][z];
						}
					}
					continue;
				}
				for (int y = 0; y < 8; y++) {
					for (int z = 0; z < 8; z++) {
						next_chb[y][z] = chb[y][z];
					}
				}
				prv = curr;
				curr = curr->nxt;
			}
		}
	}
}

bool isKingOnTheBoard(ch_template chb[][8], int color)
{
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (chb[i][j].current == KING && chb[i][j].c == color) {
				return true;
			}
		}
	}
	return false;
}

bool piecesOverlap(ch_template chb[][8], const int start_x, const int start_y,
		const int end_x, const int end_y, const char piece)
{
	int temp_y = start_y, temp_x = start_x;

	if (piece == ROOK || (piece == QUEEN && (start_y == end_y || start_x == end_x))) {
		if (!(start_y - end_y)) {
			if (start_x > end_x) {
				temp_x--;
				for (; temp_x > end_x; temp_x--) {
					if (chb[start_y][temp_x].occ)
						return true;
				}
			} else {
				temp_x++;
				for (; temp_x < end_x; temp_x++) {
					if (chb[start_y][temp_x].occ)
						return true;
				}
			}
		} else {
			if (start_y > end_y) {
				temp_y--;
				for (; temp_y > end_y; temp_y--) {
					if (chb[temp_y][start_x].occ)
						return true;
				}
			} else {
				temp_y++;
				for (; temp_y < end_y; temp_y++) {
					if (chb[temp_y][start_x].occ)
						return true;
				}
			}
		}
	}
	if (piece == BISHOP || piece == QUEEN) {
		temp_y = end_y;
		temp_x = end_x;
		if (end_y > start_y && end_x > start_x) {
			temp_y--;
			temp_x--;
			while (temp_y > start_y && temp_x > start_x) {
				if (chb[temp_y--][temp_x--].occ)
					return true;
			}
		} else if (end_y < start_y && end_x > start_x) {
			temp_y++;
			temp_x--;
			while (temp_y < start_y && temp_x > start_x) {
				if (chb[temp_y++][temp_x--].occ)
					return true;
			}
		} else if (end_y > start_y && end_x < start_x) {
			temp_y--;
			temp_x++;
			while (temp_y > start_y && temp_x < start_x) {
				if (chb[temp_y--][temp_x++].occ)
					return true;
			}
		} else {
			temp_y++;
			temp_x++;
			while (temp_y < start_y && temp_x < start_x) {
				if (chb[temp_y++][temp_x++].occ)
					return true;
			}
		}
	}
	return false;
}

#ifdef DEBUG
void date_filename(char *buf, int ln)
{
	time_t t_epc = time(NULL);
	struct tm t;

	t = *localtime(&t_epc);
	strftime(buf, ln, "%a %Y-%m-%d %H%M%S.txt", &t);
}

void write_to_log(int round, FILE* logf, char *plInput, char piece[2])
{

	if (!strncmp(piece, CSTL_LEFTROOK, 2)) {
		if (round == WHITE) {
			fprintf(logf, "Round  #%d:\tWhite moves Rook from A1 to D1 and King from E1 to C1\n", rc);
		} else {
			fprintf(logf, "           \tBlack moves Rook from A8 to D8 and King from E8 to C8\n");
			rc++;
		}
		return;
	} else if (!strncmp(piece, CSTL_RIGHTROOK, 2)) {
		if (round == WHITE)
			fprintf(logf, "Round  #%d:\tWhite moves Rook from H1 to F1 and King from E1 to G1\n", rc);
		else {
			fprintf(logf, "           \tBlack moves Rook from H8 to F8 and King from E8 to G8\n");
			rc++;
		}
		return;
	}
	if (round == WHITE) {
		fprintf(logf, "Round  #%d:\tWhite moves ", rc);
	} else {
		fprintf(logf, "           \tBlack moves ");
		rc++;
	}
	if (plInput[0] == 'P') {
		fprintf(logf, "Pawn ");
	} else if (plInput[0] == 'R') {
		fprintf(logf, "Rook ");
	} else if (plInput[0] == 'N') {
		fprintf(logf, "Knight ");
	} else if (plInput[0] == 'B') {
		fprintf(logf, "Bishop ");
	} else if (plInput[0] == 'Q') {
		fprintf(logf, "Queen ");
	} else if (plInput[0] == 'K') {
		fprintf(logf, "King ");
	}
	fprintf(logf, "from %c%c to %c%c\n", piece[0], piece[1], plInput[1], plInput[2]);
}
#endif