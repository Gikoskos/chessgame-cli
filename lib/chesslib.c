/*********************************************************************
 *                            chesslib.c                             *
 *                standard chess game implementation                 *
 *                                                                   *
 *                 (C)2015 <cyberchiller@gmail.com>                  *
 *                                                                   *
 *********************************************************************/

#include <chesslib.h>
#include <chlib-cli.h>

/*********
 *globals*
 *********/

static unsigned w_enpassant_round_left = 0;
static unsigned w_enpassant_round_right = 0;
static unsigned b_enpassant_round_left = 0;
static unsigned b_enpassant_round_right = 0;
static bool enpassant = false;

static unsigned rc = 1;
static unsigned white_removed_moves;
static unsigned black_removed_moves;


CastlingBool check_castling = {true, true, true, true, true, true};

unsigned black_move_count, white_move_count;

KingState WhiteKing = safe, BlackKing = safe;

MoveNode *b_moves[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
MoveNode *w_moves[6] = {NULL, NULL, NULL, NULL, NULL, NULL};


/*********************************************
 *prototypes for functions used in chesslib.c*
 *********************************************/

void _initChessboard(ch_template chb[][8], unsigned k, char col);
bool _isOnList(const char *start_move, const char *end_move, const char piece, const int color);


/******************************************************************
 *prototypes for functions used in chesslib.c and chlib-computer.c*
 ******************************************************************/

int _fillMoveLists(ch_template chb[][8], MoveNode ***move_array, const int flag);
void _removeMove(MoveNode **llt, char *st_todel, char *en_todel);
void _removeThreatsToKing(ch_template chb[][8], int color);
void _addMove(MoveNode **llt, const char *st, const char *en);
void _copyBoard(ch_template to[][8], ch_template from[][8]);
bool _isKingOnTheBoard(ch_template chb[][8], int color);
bool _makeMove(ch_template chb[][8], char *st_move, char *en_move, const int color, const bool ListCheck);


void _addMove(MoveNode **llt, const char *st, const char *en)
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

void _removeMove(MoveNode **llt, char *st_todel, char *en_todel)
{
	MoveNode *curr = (*llt), *prv = NULL;
	while (curr) {
		if (!strncmp(st_todel, curr->start, 3) && !strncmp(en_todel, curr->end, 3)) {
			if (prv)
				prv->nxt = curr->nxt;
			else
				(*llt) = (*llt)->nxt;
			free(curr);
			return;
		}
		prv = curr;
		curr = curr->nxt;
	}
	fprintf(stderr, "ERROR: %s -> %s not found\n", st_todel, en_todel);
}

void deleteMoveList(MoveNode **llt)
{
	MoveNode *curr = (*llt);
	while ((*llt)) {
		(*llt) = (*llt)->nxt;
		free(curr);
		curr = (*llt);
	}
}

bool _isOnList(const char *start_move, const char *end_move, const char piece, const int color)
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

void deleteMoves()
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
			chb[k][col - 'A'].current = ROOK;
		else if (col == 'B' || col == 'G')
			chb[k][col - 'A'].current = KNIGHT;
		else if (col == 'C' || col == 'F')
			chb[k][col - 'A'].current = BISHOP;
		else if (col == 'D')
			chb[k][col - 'A'].current = QUEEN;
		else
			chb[k][col - 'A'].current = KING;
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
		chb[k][col - 'A'].current = PAWN;
		chb[k][col - 'A'].occ = true;
		chb[k][col - 'A'].square[0] = col;
		chb[k][col - 'A'].square[1] = 7 - k + '1';
		col = col + 1;
	} else if (k >= 2 && k <= 5) {
		chb[k][col - 'A'].c = EMPTY;
		chb[k][col - 'A'].current = NOPIECE;
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
		return _initChessboard(chb, k, col);
}

void initChessboard(ch_template chb[][8])
{
	return _initChessboard(chb, 0, 'A');
}

int getAllMoves(ch_template chb[][8], int c_flag)
{
	deleteMoves();
	int total_move_count = _fillMoveLists(chb, NULL, ALL);
	unsigned b_tmp = black_move_count, w_tmp = white_move_count;

	white_removed_moves = 0;
	black_removed_moves = 0;

	_removeThreatsToKing(chb, c_flag);

	b_tmp -= black_removed_moves;
	w_tmp -= white_removed_moves;
	black_move_count = b_tmp;
	white_move_count = w_tmp;
	if (!black_move_count)
		BlackKing = checkmate;
	if (!white_move_count)
		WhiteKing = checkmate;
	return total_move_count;
}

int _fillMoveLists(ch_template chb[][8], MoveNode ***move_array, int flag)
{
	int i, j, k, l, move_count = 0;
	char t_st[3], t_en[3];
	MoveNode **black_m, **white_m;

	if (flag == ALL) {
		black_m = b_moves;
		white_m = w_moves;
	} else {
		if (flag == BLACK) {
			black_m = *move_array;
		} else {
			white_m = *move_array;
		}
	}

	black_move_count = 0;
	white_move_count = 0;
	t_en[2] = t_st[2] = '\0';
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			t_st[0] = chb[i][j].square[0];
			t_st[1] = chb[i][j].square[1];
			if (chb[i][j].current == PAWN) {
				if (chb[i][j].c == BLACK && (flag == ALL || flag == BLACK)) {
					if (i == 1 && !(chb[i+2][j].occ) && !(chb[i+1][j].occ)) {
						t_en[0] = chb[i+2][j].square[0];
						t_en[1] = chb[i+2][j].square[1];
						_addMove(&black_m[0], t_st, t_en);
						black_move_count++;
						move_count++;
					}
					/*if (i == 3) {
						if (w_enpassant_round_left) {
							if (chb[i][j+1].current == 'P' && chb[i][j+1].c == WHITE) {
								t_en[0] = chb[i+1][j+1].square[0];
								t_en[1] = chb[i+1][j+1].square[1];
								_addMove(&black_m[0], t_st, t_en);
								black_move_count++;
								move_count++;
							}
						} else if (w_enpassant_round_right) {
							if (chb[i][j-1].current == 'P' && chb[i][j-1].c == WHITE) {
								t_en[0] = chb[i+1][j-1].square[0];
								t_en[1] = chb[i+1][j-1].square[1];
								_addMove(&black_m[0], t_st, t_en);
								black_move_count++;
								move_count++;
							}
						}
					}*/
					t_en[1] = chb[i+1][j].square[1];
					if (chb[i+1][j].occ == false) {
						t_en[0] = chb[i+1][j].square[0];
						_addMove(&black_m[0], t_st, t_en);
						black_move_count++;
						move_count++;
					}
					if (chb[i+1][j+1].c == WHITE) {
						t_en[0] = chb[i+1][j+1].square[0];
						_addMove(&black_m[0], t_st, t_en);
						black_move_count++;
						move_count++;
					} 
					if (chb[i+1][j-1].c == WHITE) {
						t_en[0] = chb[i+1][j-1].square[0];
						_addMove(&black_m[0], t_st, t_en);
						black_move_count++;
						move_count++;
					}
				} else if (chb[i][j].c == WHITE && (flag == ALL || flag == WHITE)) {
					if (i == 6 && !(chb[i-2][j].occ) && !(chb[i-1][j].occ)) {
						t_en[0] = chb[i-2][j].square[0];
						t_en[1] = chb[i-2][j].square[1];
						_addMove(&white_m[0], t_st, t_en);
						white_move_count++;
						move_count++;
					}
					/*if (i == 4) {
						if (b_enpassant_round_left) {
							if (chb[i][j+1].current == 'P' && chb[i][j+1].c == BLACK) {
								t_en[0] = chb[i-1][j+1].square[0];
								t_en[1] = chb[i-1][j+1].square[1];
								_addMove(&white_m[0], t_st, t_en);
								white_move_count++;
								move_count++;
							}
						} else if (b_enpassant_round_right) {
							if (chb[i][j-1].current == 'P' && chb[i][j-1].c == BLACK) {
								t_en[0] = chb[i-1][j-1].square[0];
								t_en[1] = chb[i-1][j-1].square[1];
								_addMove(&white_m[0], t_st, t_en);
								white_move_count++;
								move_count++;
							}
						}
					}*/
					t_en[1] = chb[i-1][j].square[1];
					if (chb[i-1][j].occ == false) {
						t_en[0] = chb[i-1][j].square[0];
						_addMove(&white_m[0], t_st, t_en);
						white_move_count++;
						move_count++;
					}
					if (chb[i-1][j+1].c == BLACK) {
						t_en[0] = chb[i-1][j+1].square[0];
						_addMove(&white_m[0], t_st, t_en);
						white_move_count++;
						move_count++;
					} 
					if (chb[i-1][j-1].c == BLACK) {
						t_en[0] = chb[i-1][j-1].square[0];
						_addMove(&white_m[0], t_st, t_en);
						white_move_count++;
						move_count++;
					}
				}
			}
			if (chb[i][j].current == ROOK || chb[i][j].current == QUEEN) {
				k = i;
				for (l = j+1; l <= 7; l++) {
					if (chb[k][l].c == chb[i][j].c)
						break;
					t_en[0] = chb[k][l].square[0];
					t_en[1] = chb[k][l].square[1];
					if (chb[i][j].c == BLACK && (flag == ALL || flag == BLACK)) {
						if (chb[i][j].current == ROOK) {
							_addMove(&black_m[3], t_st, t_en);
							black_move_count++;
							move_count++;
						} else {
							_addMove(&black_m[2], t_st, t_en);
							black_move_count++;
							move_count++;
						}
					} else if (chb[i][j].c == WHITE && (flag == ALL || flag == WHITE)) {
						if (chb[i][j].current == ROOK) {
							_addMove(&white_m[3], t_st, t_en);
							white_move_count++;
							move_count++;
						} else {
							_addMove(&white_m[2], t_st, t_en);
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
					if (chb[i][j].c == BLACK && (flag == ALL || flag == BLACK)) {
						if (chb[i][j].current == ROOK) {
							_addMove(&black_m[3], t_st, t_en);
							black_move_count++;
							move_count++;
						} else {
							_addMove(&black_m[2], t_st, t_en);
							black_move_count++;
							move_count++;
						}
					} else if (chb[i][j].c == WHITE && (flag == ALL || flag == WHITE)) {
						if (chb[i][j].current == ROOK) {
							_addMove(&white_m[3], t_st, t_en);
							white_move_count++;
							move_count++;
						} else {
							_addMove(&white_m[2], t_st, t_en);
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
					if (chb[i][j].c == BLACK && (flag == ALL || flag == BLACK)) {
						if (chb[i][j].current == ROOK) {
							_addMove(&black_m[3], t_st, t_en);
							black_move_count++;
							move_count++;
						} else {
							_addMove(&black_m[2], t_st, t_en);
							black_move_count++;
							move_count++;
						}
					} else if (chb[i][j].c == WHITE && (flag == ALL || flag == WHITE)) {
						if (chb[i][j].current == ROOK) {
							_addMove(&white_m[3], t_st, t_en);
							white_move_count++;
							move_count++;
						} else {
							_addMove(&white_m[2], t_st, t_en);
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
					if (chb[i][j].c == BLACK && (flag == ALL || flag == BLACK)) {
						if (chb[i][j].current == ROOK) {
							_addMove(&black_m[3], t_st, t_en);
							black_move_count++;
							move_count++;
						} else {
							_addMove(&black_m[2], t_st, t_en);
							black_move_count++;
							move_count++;
						}
					} else if (chb[i][j].c == WHITE && (flag == ALL || flag == WHITE)) {
						if (chb[i][j].current == ROOK) {
							_addMove(&white_m[3], t_st, t_en);
							white_move_count++;
							move_count++;
						} else {
							_addMove(&white_m[2], t_st, t_en);
							white_move_count++;
							move_count++;
						}
					}
					if (chb[k][l].occ)
						break;
				}
			}
			if (chb[i][j].current == BISHOP || chb[i][j].current == QUEEN) {
				k = i - 1; 
				l = j - 1;
				while ((k <= 7 && k >= 0) && (l >= 0 && l <= 7)) {
					if (chb[k][l].c != chb[i][j].c) {
						t_en[0] = chb[k][l].square[0];
						t_en[1] = chb[k][l].square[1];
						if (chb[i][j].c == BLACK && (flag == ALL || flag == BLACK)) {
							if (chb[i][j].current == BISHOP) {
								_addMove(&black_m[5], t_st, t_en);
								black_move_count++;
								move_count++;
							} else {
								_addMove(&black_m[2], t_st, t_en);
								black_move_count++;
								move_count++;
							}
						} else if (chb[i][j].c == WHITE && (flag == ALL || flag == WHITE)) {
							if (chb[i][j].current == BISHOP) {
								_addMove(&white_m[5], t_st, t_en);
								white_move_count++;
								move_count++;
							} else {
								_addMove(&white_m[2], t_st, t_en);
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
						if (chb[i][j].c == BLACK && (flag == ALL || flag == BLACK)) {
							if (chb[i][j].current == BISHOP) {
								_addMove(&black_m[5], t_st, t_en);
								black_move_count++;
								move_count++;
							} else {
								_addMove(&black_m[2], t_st, t_en);
								black_move_count++;
								move_count++;
							}
						} else if (chb[i][j].c == WHITE && (flag == ALL || flag == WHITE)) {
							if (chb[i][j].current == BISHOP) {
								_addMove(&white_m[5], t_st, t_en);
								white_move_count++;
								move_count++;
							} else {
								_addMove(&white_m[2], t_st, t_en);
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
						if (chb[i][j].c == BLACK && (flag == ALL || flag == BLACK)) {
							if (chb[i][j].current == BISHOP) {
								_addMove(&black_m[5], t_st, t_en);
								black_move_count++;
								move_count++;
							} else {
								_addMove(&black_m[2], t_st, t_en);
								black_move_count++;
								move_count++;
							}
						} else if (chb[i][j].c == WHITE && (flag == ALL || flag == WHITE)) {
							if (chb[i][j].current == BISHOP) {
								_addMove(&white_m[5], t_st, t_en);
								white_move_count++;
								move_count++;
							} else {
								_addMove(&white_m[2], t_st, t_en);
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
						if (chb[i][j].c == BLACK && (flag == ALL || flag == BLACK)) {
							if (chb[i][j].current == BISHOP) {
								_addMove(&black_m[5], t_st, t_en);
								black_move_count++;
								move_count++;
							} else {
								_addMove(&black_m[2], t_st, t_en);
								black_move_count++;
								move_count++;
							}
						} else if (chb[i][j].c == WHITE && (flag == ALL || flag == WHITE)) {
							if (chb[i][j].current == BISHOP) {
								_addMove(&white_m[5], t_st, t_en);
								white_move_count++;
								move_count++;
							} else {
								_addMove(&white_m[2], t_st, t_en);
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
			if (chb[i][j].current == KING) {
				for (k = i - 1; k < i + 2; k++){
					for (l = j - 1; l < j + 2; l++){
						if (k > 7 || k < 0 || l > 7 || l < 0)
							continue;
						if ((i == k && j == l) || chb[k][l].c == chb[i][j].c)
							continue;
						t_en[0] = chb[k][l].square[0];
						t_en[1] = chb[k][l].square[1];
						if (chb[i][j].c == BLACK && (flag == ALL || flag == BLACK)) {
							_addMove(&black_m[1], t_st, t_en);
							black_move_count++;
							move_count++;
						} else if (chb[i][j].c == WHITE && (flag == ALL || flag == WHITE)) {
							_addMove(&white_m[1], t_st, t_en);
							white_move_count++;
							move_count++;
						}
					}
				}
			}
			if (chb[i][j].current == KNIGHT) {
				int knightrow[] = {i-2,i-2,i-1,i-1,i+1,i+1,i+2,i+2};
				int knightcol[] = {j-1,j+1,j-2,j+2,j-2,j+2,j-1,j+1};
				int count = 0;
				for (; count < 8; count++) {
					if (knightrow[count] > -1 && knightrow[count] < 8
						&& knightcol[count] > -1 && knightcol[count] < 8) {
						if (chb[knightrow[count]][knightcol[count]].c != chb[i][j].c) {
							t_en[0] = chb[knightrow[count]][knightcol[count]].square[0];
							t_en[1] = chb[knightrow[count]][knightcol[count]].square[1];
							if (chb[i][j].c == BLACK && (flag == ALL || flag == BLACK)) {
								_addMove(&black_m[4], t_st, t_en);
								black_move_count++;
								move_count++;
							} else if (chb[i][j].c == WHITE && (flag == ALL || flag == WHITE)) {
								_addMove(&white_m[4], t_st, t_en);
								white_move_count++;
								move_count++;
							}
						}
					}
				}
			}
		}
	}
	if (flag == ALL || flag == BLACK) {
		if (check_castling.KBlack && BlackKing != check) {
			if (check_castling.BR_left) {
				if (!chb[0][1].occ && !chb[0][2].occ && !chb[0][3].occ) {
					_addMove(&black_m[1], "E8", "C8");
				}
			}
			if (check_castling.BR_right) {
				if (!chb[0][5].occ && !chb[0][6].occ) {
					_addMove(&black_m[1], "E8", "G8");
				}
			}
		}
	}
	if (flag == ALL || flag == WHITE) {
		if (check_castling.KWhite && WhiteKing != check) {
			if (check_castling.WR_left) {
				if (!chb[7][1].occ && !chb[7][2].occ && !chb[7][3].occ) {
					_addMove(&white_m[1], "E1", "C1");
				}
			}
			if (check_castling.WR_right) {
				if (!chb[7][5].occ && !chb[7][6].occ) {
					_addMove(&white_m[1], "E1", "C1");
				}
			}
		}
	}
	return move_count;
}

bool _makeMove(ch_template chb[][8], char *st_move, char *en_move, const int color, const bool ListCheck)
{
	if (!en_move || !st_move)
		return false;

	if (islower(st_move[0]) || islower(en_move[0]) 
		|| islower(st_move[2]) || islower(en_move[2])) {
		st_move[0] = (char)toupper(st_move[0]);
		en_move[0] = (char)toupper(en_move[0]);
		st_move[2] = (char)toupper(st_move[2]);
		en_move[2] = (char)toupper(en_move[0]);
	}

	unsigned short startx, starty, endx, endy;

	startx = st_move[0] - 'A';
	starty = '8' - st_move[1];
	endx = en_move[0] - 'A';
	endy = '8' - en_move[1];

	char piece = chb[starty][startx].current;
	if (piece == NOPIECE)
		return false;

	if (ListCheck) {
		if (!_isOnList(st_move, en_move, piece, color))
			return false;
	}

	b_enpassant_round_left = 0;
	b_enpassant_round_right = 0;
	w_enpassant_round_left = 0;
	w_enpassant_round_right = 0;
	enpassant = false;
	if (piece == PAWN) {
		if (color == BLACK) {
			if (starty == 1 && endy == 3) {
				if (chb[endy][endx+1].current == PAWN && chb[endy][endx+1].c == WHITE) {
					b_enpassant_round_right = rc;
					enpassant = true;
				}
				if (chb[endy][endx-1].current == PAWN && chb[endy][endx-1].c == WHITE) {
					b_enpassant_round_left = rc;
					enpassant = true;
				}
			}
		} else {
			if (starty == 6 && endy == 4) {
				if (chb[endy][endx+1].current == PAWN && chb[endy][endx+1].c == BLACK) {
					w_enpassant_round_right = rc;
					enpassant = true;
				}
				if (chb[endy][endx-1].current == PAWN && chb[endy][endx-1].c == BLACK) {
					w_enpassant_round_left = rc;
					enpassant = true;
				}
			}
		}
	}

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
		if (color == BLACK) {
			check_castling.KBlack = false;
			if (startx == 4 && endx == 2) {
				chb[0][3].occ = true;
				chb[0][3].current = ROOK;
				chb[0][3].c = BLACK;
				chb[0][0].occ = false;
				chb[0][0].c = EMPTY;
				chb[0][0].current = NOPIECE;
			} else if (startx == 4 && endx == 6) {
				chb[0][5].occ = true;
				chb[0][5].current = ROOK;
				chb[0][5].c = BLACK;
				chb[0][7].occ = false;
				chb[0][7].c = EMPTY;
				chb[0][7].current = NOPIECE;
			}
		} else {
			check_castling.KWhite = false;
			if (startx == 4 && endx == 2) {
				chb[7][3].occ = true;
				chb[7][3].current = ROOK;
				chb[7][3].c = WHITE;
				chb[7][0].occ = false;
				chb[7][0].c = EMPTY;
				chb[7][0].current = NOPIECE;
			} else if (startx == 4 && endx == 6) {
				chb[7][5].occ = true;
				chb[7][5].current = ROOK;
				chb[7][5].c = WHITE;
				chb[7][7].occ = false;
				chb[7][7].c = EMPTY;
				chb[7][7].current = NOPIECE;
			}
		}
	}

	chb[endy][endx].occ = true;
	chb[endy][endx].current = chb[starty][startx].current;
	chb[endy][endx].c = color;
	chb[starty][startx].occ = false;
	chb[starty][startx].c = EMPTY;
	chb[starty][startx].current = NOPIECE;

	return true;
}

bool makeMove(ch_template chb[][8], char *st_move, char *en_move, const int color)
{
	if ( _makeMove(chb, st_move, en_move, color, true))
		return true;
	else
		return false;
}

void _removeThreatsToKing(ch_template chb[][8], const int color)
{
	ch_template next_chb[8][8];
	int ccolor = (color == BLACK)?WHITE:BLACK;
	bool removed = false;
	CastlingBool tempCstl = check_castling;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			next_chb[i][j] = chb[i][j];
		}
	}

	if (color == BLACK) {
		BlackKing = safe;
	} else {
		WhiteKing = safe;
	}

	for (int i = 0; i < 6; i++) {
		MoveNode *curr = (color == WHITE)?w_moves[i]:b_moves[i];
		while (curr) {
			_makeMove(next_chb, curr->start, curr->end, color, false);
			ch_template temp_chb[8][8];
			_copyBoard(temp_chb, next_chb);
			MoveNode **temp_moves = malloc(6*sizeof(MoveNode));
			for (int k = 0; k < 6; k++) {
				temp_moves[k] = NULL;
			}
			_fillMoveLists(next_chb, &temp_moves, ccolor);
			MoveNode *curr_nextPlayer[6] = {temp_moves[0], temp_moves[1], temp_moves[2], 
				temp_moves[3], temp_moves[4], temp_moves[5]};
			for (int z = 0; z < 6; z++) {
				while (curr_nextPlayer[z]) {
					_makeMove(next_chb, curr_nextPlayer[z]->start, curr_nextPlayer[z]->end, ccolor, false);
					if (!_isKingOnTheBoard(next_chb, color)) {
						_removeMove((color == WHITE)?&w_moves[i]:&b_moves[i], curr->start, curr->end);
						removed = true;
						if (color == WHITE)
							white_removed_moves++;
						else
							black_removed_moves++;
						break;
					}
					_copyBoard(next_chb, temp_chb);
					curr_nextPlayer[z] = curr_nextPlayer[z]->nxt;
				}
				if (removed) {
					removed = false;
					break;
				}
			}
			for (int k = 0; k < 8; k++) {
				for (int l = 0; l < 8; l++) {
					next_chb[k][l] = chb[k][l];
				}
				if (k < 6)
					deleteMoveList(&temp_moves[k]);
			}
			free(temp_moves);
			curr = curr->nxt;
		}
	}
	check_castling = tempCstl;
}

void playMoves(ch_template chb[][8], int *round, unsigned short move_count, ...)
{
	va_list next_move;
	char temp[5], move_begin[3], move_end[3];

	move_begin[2] = '\0';
	move_end[2] = '\0';
	va_start(next_move, move_count);
	while (move_count--) {
		char temp_arg[5];
		strncpy(temp_arg, va_arg(next_move, const char *), 5);
		for (int i = 0; i < 5; i++)
			(i < 4)?(temp[i] = temp_arg[i]):(temp[i] = '\0');
		move_begin[0] = temp[0];
		move_begin[1] = temp[1];
		move_end[0] = temp[2];
		move_end[1] = temp[3];
		getAllMoves(chb, (*round));
		makeMove(chb, move_begin, move_end, (*round));
		*round = ((*round)== BLACK)?WHITE:BLACK;
	}
	va_end(next_move);
}

bool _isKingOnTheBoard(ch_template chb[][8], const int color)
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

void _copyBoard(ch_template to[][8], ch_template from[][8])
{
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			to[i][j] = from[i][j];
		}
	}
}
