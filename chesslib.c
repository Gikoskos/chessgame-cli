/********************************************************************/
/*                           chesslib.c                             */
/* standard chess engine no AI or complicated rules implemented yet */
/*                                                                  */
/*                    by <cyberchiller@gmail.com>                   */
/*              see COPYING for copyright information               */
/********************************************************************/

#include "chesslib.h"

/*prototypes for functions used only in here*/
bool king_is_threatened(int, int, int, int, char);
void k_domain_ctrl(int, int, int, int, char);
KingState check_mate(char);


static short WKingLife[][3] = {{0, 0, 0},	//life energy of white King
				{0, 0, 0},	//when all 0s are 1s the game is over
				{0, 0, 0}};

static short BKingLife[][3] = {{0, 0, 0},	//life energy of black King
				{0, 0, 0},	//when all 0s are 1s the game is over
				{0, 0, 0}};


typedef struct KingDomain {
	int x;
	int y;
} KingDomain;


extern void clear_buffer(void)
{
	char clbuf;
	while ((clbuf=getchar()) != '\n');
}

void initChessboard(ch_template chb[][8], unsigned k, char col)	/*k is row, col is column*/
{ 
	if (k == 0 || k == 7) {
		if (col == 'A' || col == 'H')
			chb[k][col - 65].current = 'R';
		else if (col == 'B' || col == 'G')
			chb[k][col - 65].current = 'N';
		else if (col == 'C' || col == 'F')
			chb[k][col - 65].current = 'B';
		else if (col == 'D')
			chb[k][col - 65].current = 'Q';
		else
			chb[k][col - 65].current = 'K';
		if (k == 0)
			chb[k][col - 65].c = WHITE;	/*colorize the pieces*/
		else
			chb[k][col - 65].c = BLACK;
		chb[k][col - 65].occ = true;
		chb[k][col - 65].square[0] = col;
		chb[k][col - 65].square[1] = k + '1';
		col = col + 1;
	} else if (k == 1 || k == 6) {
		if(k == 1) 
			chb[k][col - 65].c = WHITE;
		else 
			chb[k][col - 65].c = BLACK;
		chb[k][col - 65].current = 'P';
		chb[k][col - 65].occ = true;
		chb[k][col - 65].square[0] = col;
		chb[k][col - 65].square[1] = k + '1';
		col = col + 1;
	} else if (k >= 2 && k <= 5) {
		chb[k][col - 65].c = EMPTY;
		chb[k][col - 65].current = 'e';
		chb[k][col - 65].occ = false;
		chb[k][col - 65].square[0] = col;
		chb[k][col - 65].square[1] = k + '1';
		col = col + 1;
	}
	if (col == 'I') {
		++k;
		col = 'A';
	}
	if (k != 8)
		initChessboard(chb, k, col);
}

void printBoard(ch_template chb[][8])
{
#ifdef _WIN32
	HANDLE cmdhandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO cmdinfo;
	WORD sv_att;
	int i, j;

	GetConsoleScreenBufferInfo(cmdhandle, &cmdinfo);
	sv_att = cmdinfo.wAttributes;

	printf("    a   b   c   d   e   f   g   h  \n");
	for (i = 0; i < 8; i++) {
		printf("%d | ", i + 1);
		for (j = 0; j < 8; j++) {
			if (chb[i][j].occ == false) {
				printf("  | ");
			} else {
				if (chb[i][j].c == BLACK)
					SetConsoleTextAttribute(cmdhandle, FOREGROUND_RED); 
				else
					SetConsoleTextAttribute(cmdhandle, FOREGROUND_GREEN);
				printf("%c", chb[i][j].current);
				SetConsoleTextAttribute(cmdhandle, sv_att);
				printf(" | ");
			}
		}
		printf("%d", i + 1);
		printf("\n");
	}
	printf("    a   b   c   d   e   f   g   h  \n");
	printf("\n\n");
#else
	int i, j, max, y = 0, x = 0;

	printf("    a   b   c   d   e   f   g   h  \n");
	for (i = 0; i < (max = (MOS%2)?MOS:(MOS-1)); i++) {
		if(i%2 == true) printf("%d ", y + 1);
		else printf("  ");
		for (j = 0; j < max; j++) {
			if (!i) { 
				if(!j) printf("\u250F");
				else if(j == (max-1)) printf("\u2513");
				else if(j%2) printf("\u2501\u2501\u2501");
				else if(!(j%2)) printf("\u2533");
			} else if (i == max-1) {
				if(!j)printf("\u2517");
				else if(j == (max-1)) printf("\u251B");
				else if(j%2) printf("\u2501\u2501\u2501");
				else if(!(j%2)) printf("\u253B");
			} else if (i%2) {
				if(!(j%2)) printf("\u2503");
				else {
					/*usleep(3000);
					fflush(stdout);*/
					if (chb[y][x].occ == false)
						printf("   ");
					else {
						if (chb[y][x].c == BLACK)
							printf(" %s%c%s ", KRED, chb[y][x].current, RESET);
						else
							printf(" %s%c%s ", KYEL, chb[y][x].current, RESET);
					}
					x++;
				}
			} else if (!(i%2)) {
				if(!j) printf("\u2523");
				else if(j == (max-1)) printf("\u252B");
				else if(j%2) printf("\u2501\u2501\u2501");
				else if(!(j%2)) printf("\u254B");
			}
		}
		x = 0;
		if(i%2 == true) {
			printf(" %d", y + 1);
			y++;
		} else
			printf("  ");
		printf("\n");
	}
	printf("    a   b   c   d   e   f   g   h  \n");
	printf("\n");
#endif
}

bool validInput(const char *input, int *errPtr)
{
	if (!strcmp(input, "help") || strlen(input) > 3) {
		return false;
	} else if (input[0] != 'R' && input[0] != 'N' && input[0] != 'B' && input[0] != 'Q' && input[0] != 'K' && input[0] != 'P') {
		*errPtr = 5;
		return false;
	} else if (input[1] < 'a' || input[1] > 'h') {
		*errPtr = 6;
		return false;
	} else if (input[2] < '1' || input[2] > '8') {
		*errPtr = 7;
		return false;
	}
	return true;
}

void printError(int i)
{
	char *error_out[] = { "\n",
	"Command line argument not recognized.\n", "Bad input.\n",
	"Illegal move.\n", "Log file could not be created.\n"};

	if (i < 5)
		fprintf(stderr, "%s", error_out[i]);
	else {
		if (i == 5)
			fprintf(stderr, "%s\n%s%s\n\t%s\n\t%s\n\t%s\n",
			"Not a valid Chess piece entered. ",
			"Please use only R for Rook, ", "N for Knight,", 
			"B for Bishop,", "Q for Queen,", "K for King and P for Pawn");
		else if (i == 6)
			fprintf(stderr, "%s\n",
			"Not a valid column letter entered. Please use only a b c d e f g or h.");
		else if (i == 7)
			fprintf(stderr, "%s\n",
			"Not a valid row entered. Please use only numbers from 1 to 8.");
		else
			fprintf(stderr, "%s%d.\n",
			"Unrecognized error number ", i);
	}
}

char *findPiece(ch_template chb[][8], const char *input, int color)
{
	int i, j, k, l, count, conflict = 3;	/*conflict: int to check for two pieces attacking the same piece*/
	char *retvalue = calloc(3, sizeof(*retvalue));
	char *temp = calloc(3, sizeof(*temp));

	if (!retvalue)
		exit(0);
	retvalue[2] = '\0';
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if (chb[i][j].current == input[0] && chb[i][j].c == color) {
				retvalue[0] = chb[i][j].square[0];
				retvalue[1] = chb[i][j].square[1];
				if (input[0] == 'P'){
					l = input[2] - '1';
					k = input[1] - 65;
					if (color == BLACK) { 
						if ((i == 6) && (l == i - 2) && (j == k) && (chb[l][k].occ == false) && (chb[i-1][j].occ == false)) 
							return retvalue;	/*check for pawn's double first move*/
						if ((i-1) == 0 && chb[i-1][j].square[0] == input[1] && chb[i-1][j].square[1] == input[2] && chb[i-1][j].occ == false) {
							return retvalue;
						}
						if ((i-1) == 0 && chb[i-1][j+1].square[0] == input[1] && chb[i-1][j+1].square[1] == input[2] && chb[i-1][j+1].occ == true && chb[i-1][j+1].c != color) {
							return retvalue;
						}
						if ((i-1) == 0 && chb[i-1][j-1].square[0] == input[1] && chb[i-1][j-1].square[1] == input[2] && chb[i-1][j-1].occ == true && chb[i-1][j-1].c != color) {
							return retvalue;
						}
						if (chb[i-1][j].occ == false) {
							if (chb[i-1][j].square[0] == input[1] && chb[i-1][j].square[1] == input[2]) {
								return retvalue;
							}
						}
						if (chb[i-1][j+1].occ == true) {
							if (chb[i-1][j+1].square[0] == input[1] && chb[i-1][j+1].square[1] == input[2]) {
								if (temp[0] != retvalue[0] && temp[1] == retvalue[1]) {
									conflict = true;
									goto EXIT_LOOP;
								}
								conflict = false;
								memcpy(temp,retvalue,3);
							}
						} 
						if (chb[i-1][j-1].occ == true) {
							if (chb[i-1][j-1].square[0] == input[1] && chb[i-1][j-1].square[1] == input[2]) {
								if (temp[0] != retvalue[0] && temp[1] == retvalue[1]) {
									conflict = true;
									goto EXIT_LOOP;
								}
								conflict = false;
								memcpy(temp,retvalue,3);
							}
						}
					} else {
						if ((i == 1) && (l == i + 2) && (j == k) && (chb[l][k].occ == false) && (chb[i+1][j].occ == false))
							return retvalue;	/*check for pawn's double first move*/
						if ((i+1) == 0 && chb[i+1][j].square[0] == input[1] && chb[i+1][j].square[1] == input[2] && chb[i+1][j].occ == false) {
							return retvalue;
						}
						if ((i+1) == 0 && chb[i+1][j+1].square[0] == input[1] && chb[i+1][j+1].square[1] == input[2] && chb[i+1][j+1].occ == true && chb[i+1][j+1].c != color) {
							return retvalue;
						}
						if ((i+1) == 0 && chb[i+1][j-1].square[0] == input[1] && chb[i+1][j-1].square[1] == input[2] && chb[i+1][j-1].occ == true && chb[i-1][j-1].c != color) {
							return retvalue;
						}
						if (chb[i+1][j].occ == false){
							if (chb[i+1][j].square[0] == input[1] && chb[i+1][j].square[1] == input[2]) {
								return retvalue;
							}
						}
						if (chb[i+1][j+1].occ == true){
							if (chb[i+1][j+1].square[0] == input[1] && chb[i+1][j+1].square[1] == input[2]) {
								if (temp[0] != retvalue[0] && temp[1] == retvalue[1]) {
									conflict = true;
									goto EXIT_LOOP;
								}
								conflict = false;
								memcpy(temp,retvalue,3);
							}
						}
						if (chb[i+1][j-1].occ == true){
							if (chb[i+1][j-1].square[0] == input[1] && chb[i+1][j-1].square[1] == input[2]) {
								if (temp[0] != retvalue[0] && temp[1] == retvalue[1]) {
									conflict = true;
									goto EXIT_LOOP;
								}
								conflict = false;
								memcpy(temp,retvalue,3);
							}
						}
					}
					continue;
				} else if (input[0] == 'K') {
					for (k = i - 1; k < i + 2; k++){
						for (l = j - 1; l < j + 2; l++){
							if (chb[k][l].square[0] == input[1] && chb[k][l].square[1] == input[2]) {
								return retvalue;
							}
						}  
					}
				} else if (input[0] == 'N') {
					int knightrow[] = {i-2,i-2,i-1,i-1,i+1,i+1,i+2,i+2};
					int knightcol[] = {j-1,j+1,j-2,j+2,j-2,j+2,j-1,j+1};
					l = input[2] - '1';
					k = input[1] - 65;
					for (count = 0; count < 8; count++) {
						if (knightrow[count] == l && knightcol[count] == k) {
							if (conflict == false) {
								conflict = true;
								goto EXIT_LOOP;
							}
							conflict = false;
							memcpy(temp,retvalue,3);
						}
					}
				} else if (input[0] == 'R' || input[0] == 'Q') {
					k = i;
					for (l=0; l < 8; l++) {
						if (l == j)	/*to skip the piece itself*/ 
							continue;
						if (chb[k][l].square[0] == input[1] && chb[k][l].square[1] == input[2]) {
							if (input[0] == 'R') {
								if (conflict == false) {
									conflict = true;
									goto EXIT_LOOP;
								}
								conflict = false;
								memcpy(temp,retvalue,3);
							} else {
								return retvalue;
							}
						}
					}
					l = j;
					for (k = 0; k < 8; k++) {
						if (k == i)
							continue;
						if (chb[k][l].square[0] == input[1] && chb[k][l].square[1] == input[2]) {
							if (input[0] == 'R') {
								if (conflict == false) {
									conflict = true;
									goto EXIT_LOOP;
								}
								conflict = false;
								memcpy(temp,retvalue,3);
							} else {
								return retvalue;
							}
						}
					}
				}
				if (input[0] == 'B' || input[0] == 'Q') {	/*different if, to check for queen's diagonal moves*/
					k = i - 1; 
					l = j - 1;
					while ((k <= 7 && k >= 0) && (l >= 0 && l <= 7)) {
						if (chb[k][l].square[0] == input[1] && chb[k][l].square[1] == input[2]) {
							if (input[0] == 'B') {
								if (conflict == false) {
									conflict = true;
									goto EXIT_LOOP;
								}
								conflict = false;
								memcpy(temp,retvalue,3);
							} else {
								return retvalue;
							}
						}
						k--;
						l--;
					}
					k = i - 1;
					l = j + 1;
					while ((k >= 0 && k <= 7) && (l <= 7 && l >= 0)) {
						if (chb[k][l].square[0] == input[1] && chb[k][l].square[1] == input[2]) {
							if (input[0] == 'B') {
								if (conflict == false) {
									conflict = true;
									goto EXIT_LOOP;
								}
								conflict = false;
								memcpy(temp,retvalue,3);
							} else {
								return retvalue;
							}
						}
						k--;
						l++;
					} 
					k = i + 1;
					l = j - 1;
					while ((k <= 7 && k >= 0) && (l >= 0 && l <= 7)) {
						if (chb[k][l].square[0] == input[1] && chb[k][l].square[1] == input[2]) {
							if (input[0] == 'B') {
								if (conflict == false) {
									conflict = true;
									goto EXIT_LOOP;
								}
								conflict = false;
								memcpy(temp,retvalue,3);
							} else {
								return retvalue;
							}
						}
						k++;
						l--;
					}
					k = i + 1;
					l = j + 1;
					while ((k <= 7 && k >= 0) && (l <= 7 && l >= 0)) {
						if (chb[k][l].square[0] == input[1] && chb[k][l].square[1] == input[2]) {
							if (input[0] == 'B') {
								if (conflict == false) {
									conflict = true;
									goto EXIT_LOOP;
								}
								conflict = false;
								memcpy(temp,retvalue,3);
							} else {
								return retvalue;
							}
						}
						k++;
						l++;
					}
				}
			}
		}
	}
	EXIT_LOOP:
	if (conflict == true) {
		retvalue = realloc(retvalue, 5);
		retvalue[2] = temp[0];
		retvalue[3] = temp[1];
		retvalue[4] = '\0';
		free(temp);
		return retvalue;
	} else if(conflict == false) {
		return temp;
	}
	return NULL;
}

extern void printInstructions(void)
{
	printf("%s%s\n%s\n%s\n%s\n%s\n%s %s\n%s\n",
	"Please enter your move in the following format: ", "\'xyz\'", 
	"x is the piece you want to move,",
	"y is the letter of the column and",
	"z is the number of the row.",
	"Acceptable values for x are: R for Rook, N for Knight, B for Bishop, Q for Queen, K for King and P for Pawn.",
	"Acceptable values for y are lowercase letters",
	"from \'a\' to \'h\' and for z numbers from 1 to 8.",
	"For example to move Bishop to e2 type Be2 or Pawn to a4 type Pa4.");
}

extern void clear_screen(void)
{
#ifndef _WIN32
	char buf[1024];
	char *str;

	tgetent(buf, getenv("TERM"));
	str = tgetstr("cl", NULL);
	fputs(str, stdout);
#else
	/*puts( "\033[2J" ); */
	/*clear screen using ASCII; doesn't work as well, only use it if you can't install libncurses
	 *don't forget to delete or comment lines 324-331*/
	system("cls");
#endif
} 

bool movePiece(ch_template chb[][8], char *plInput, char piece[2], int color)
{
	int startx, starty, endx, endy; /*cords for the current tile and for the tile to move the piece to*/

	endx = plInput[2] - '1';
	endy = plInput[1] - 65;
	startx = piece[1] - '1';
	starty = piece[0] - 65;
	if (!piecesOverlap(chb, startx, starty, endx, endy, plInput[0])) {
		if (chb[endx][endy].c != color) {	/*checks whether it's a piece of the same color or not*/
			chb[endx][endy].occ = true;
			chb[endx][endy].current = chb[startx][starty].current;
			chb[endx][endy].c = color;
			chb[startx][starty].occ = false;
			chb[startx][starty].c = EMPTY;
			chb[startx][starty].current = 'e';
			if (plInput[0] == 'P' && ((startx == 1 && endx == 0) || (startx == 6 && endx == 7))) {
				/*options for the promotion of the pawn*/
				char *promote_selection;
				printf("What piece do you want to promote your Pawn into? ");
				RETRY:promote_selection = getPlayerInput();
				if (!strcmp(promote_selection, "queen") || !strcmp(promote_selection, "Q") ||
					!strcmp(promote_selection, "Queen") || !strcmp(promote_selection, "q")) {
					chb[endx][endy].current = 'Q';
				} else if (!strcmp(promote_selection, "bishop") || !strcmp(promote_selection, "B") ||
					!strcmp(promote_selection, "Bishop") || !strcmp(promote_selection, "b")) {
					chb[endx][endy].current = 'B';
				} else if (!strcmp(promote_selection, "Knight") || !strcmp(promote_selection, "N") ||
					!strcmp(promote_selection, "knight") || !strcmp(promote_selection, "n")) {
					chb[endx][endy].current = 'N';
				} else {
					free(promote_selection);
					printf("Bad input, please enter a valid piece: ");
					goto RETRY;
				} 
				free(promote_selection);
			}
			return true;
		}
	}
	return false;
}

bool piecesOverlap(ch_template chb[][8], const int sx, const int sy,
			    const int ex, const int ey, const char piece)
{
	int tempx = sx, tempy = sy;

	if (piece == 'R' || (piece == 'Q' && (sx == ex || sy == ey))) {
		if (!(sx-ex)) {
			if (sy > ey) {
				tempy--;
				for (; tempy > ey; tempy--) {
					if (chb[sx][tempy].occ)
						return true;
				}
			} else {
				tempy++;
				for (; tempy < ey; tempy++) {
					if (chb[sx][tempy].occ)
						return true;
				}
			}
		} else {
			if (sx > ex) {
				tempx--;
				for (; tempx > ex; tempx--) {
					if (chb[tempx][sy].occ)
						return true;
				}
			} else {
				tempx++;
				for (; tempx < ex; tempx++) {
					if (chb[tempx][sy].occ)
						return true;
				}
			}
		}
	} else if (piece == 'B' || piece == 'Q') {
		tempx = ex;
		tempy = ey;
		if (ex>sx && ey>sy) {
			tempx--;
			tempy--;
			while (tempx > sx && tempy > sy) {
				if (chb[tempx--][tempy--].occ)
					return true;
			}
		} else if (ex<sx && ey>sy) {
			tempx++;
			tempy--;
			while (tempx < sx && tempy > sy) {
				if (chb[tempx++][tempy--].occ)
					return true;
			}
		} else if (ex>sx && ey<sy) {
			tempx--;
			tempy++;
			while (tempx > sx && tempy < sy) {
				if (chb[tempx--][tempy++].occ)
					return true;
			}
		} else {
			tempx++;
			tempy++;
			while (tempx < sx && tempy < sy) {
				if (chb[tempx++][tempy++].occ)
					return true;
			}
		}
	}
	return false;
}

void findKState(ch_template chb[][8], KingState *WK, KingState *BK)
{
	int i, j, WKx, WKy, BKx, BKy;
	int B_check_count = 0, W_check_count = 0;

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if (chb[i][j].current == 'K') {
				if (chb[i][j].c == WHITE) {
					WKx = i;
					WKy = j;
				} else {
					BKx = i;
					BKy = j;
				}
			}
		}
	}

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if (chb[i][j].current == 'P') {
				if (chb[i][j].c == BLACK) {
					if ((j+1) == (WKy+1) && (i-1) == (WKx+1)) {
						WKingLife[2][2] = 1;
						*WK = safe_check;
					} else if ((j-1) == (WKy-1) && (i-1) == (WKx+1)) {
						WKingLife[2][0] = 1;
						*WK = safe_check;
					} else if (((j+1) == WKy && (i-1) == WKx) || ((j-1) == WKy && (i-1) == WKx)) {
						W_check_count++;
					}
				} else if (chb[i][j].c == WHITE) {
					if ((j+1) == (BKy+1) && (i+1) == (BKx-1)) {
						BKingLife[0][2] = 1;
						*BK = safe_check;
					} else if ((j-1) == (BKy-1) && (i+1) == (BKx-1)) {
						BKingLife[0][0] = 1;
						*BK = safe_check;
					} else if (((j+1) == BKy && (i+1) == BKx) || ((j-1) == BKy && (i+1) == BKx)) {
						B_check_count++;
					}
				}
			} else if (chb[i][j].current == 'R' || chb[i][j].current == 'Q') {
				if (chb[i][j].c == BLACK) {
					if ((WKx == i) && (piecesOverlap(chb,i,j,WKx,WKy,'R') == false)) {
						W_check_count++;
					} else if ((WKy == j) && (piecesOverlap(chb,i,j,WKx,WKy,'R') == false)) {
						W_check_count++;
					}
				} else if (chb[i][j].c == WHITE) {
					if ((BKx == i) && (piecesOverlap(chb,i,j,WKx,WKy,'R') == false)) {
						B_check_count++;
					} else if ((BKy == j) && (piecesOverlap(chb,i,j,BKx,BKy,'R') == false)) {
						B_check_count++;
					}
				}
			} 
			if (chb[i][j].current == 'B') {
				if (chb[i][j].c == BLACK) {
					if (king_is_threatened(WKx, WKy, i, j, 'B') == true) {
						if (piecesOverlap(chb, i, j, WKx, WKy, 'B') == false) {
							W_check_count++;
						}
					}
				} else if (chb[i][j].c == WHITE) {
					if (king_is_threatened(BKx, BKy, i, j, 'B') == true) {
						if (piecesOverlap(chb, i, j, BKx, BKy, 'B') == false) {
							B_check_count++;
						}
					}
				}
			}
		}
	}
	if (W_check_count > 0) {
		*WK = check;
		WKingLife[1][1] = 1;
	} else if (!W_check_count) {
		*WK = safe;
		WKingLife[1][1] = 0;
	}
	if (B_check_count > 0) {
		*BK = check;
		BKingLife[1][1] = 1;
	} else if (!B_check_count) {
		*BK = safe;
		BKingLife[1][1] = 0;
	}
}

bool king_is_threatened(int Kx, int Ky, int xpiece, int ypiece, char c)
{
	int k, l;

	if (c == 'B' || c == 'Q') {
		if (Kx == xpiece || Ky == ypiece) {
			return false;
		}
		if (Kx > xpiece && Ky > ypiece) {
			k = xpiece + 1;
			l = ypiece + 1;
			while ((k <= 7 && k >= 0) && (l <= 7 && l >= 0)) {
				if (k == Kx && l == Ky) {
					return true;
				}
				k++;
				l++;
			}
		} else if (Kx < xpiece && Ky > ypiece) {
			k = xpiece - 1;
			l = ypiece + 1;
			while ((k >= 0 && k <= 7) && (l <= 7 && l >= 0)) {
				if (k == Kx && l == Ky) {
					return true;
				}
				k--;
				l++;
			} 
		} else if (Kx > xpiece && Ky < ypiece) {
			k = xpiece + 1;
			l = ypiece - 1;
			while ((k <= 7 && k >= 0) && (l >= 0 && l <= 7)) {
				if (k == Kx && l == Ky) {
					return true;
				}
				k++;
				l--;
			}
		} else {
			k = xpiece - 1; 
			l = ypiece - 1;
			while ((k <= 7 && k >= 0) && (l >= 0 && l <= 7)) {
				if (k == Kx && l == Ky) {
					return true;
				}
				k--;
				l--;
			}
		}
	}
	return false;
}

void k_domain_ctrl(int x_p, int y_p, int Kx, int Ky, char color)
{
	KingDomain KD[3][3] = {{{Kx-1, Ky-1},{Kx-1, Ky},{Kx-1, Ky+1}},
			{{Kx, Ky-1},{Kx, Ky},{Kx, Ky+1}},
			{{Kx+1, Ky-1},{Kx+1, Ky},{Kx+1, Ky+1}}};
	int k, l;

	for (k = 0; k < 3; k++) {
		for (l = 0; l < 3; l++) {
			if (k == l) continue;
			if (KD[k][l].x == x_p && KD[k][l].y == y_p) {
				if (color == 'B') {
				BKingLife[k][l] = 1;
				} else {
				WKingLife[k][l] = 1;
				}
			}
		}
	}
}

KingState check_mate(char Kcolor)
{
	int i, j, Wcounter = 0, Bcounter = 0;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			if (WKingLife[i][j])
				Wcounter++;
			if (BKingLife[i][j])
				Bcounter++;
		}
	}
	if (Wcounter == 9 && Kcolor == 'W')
		return checkmate;
	if (Bcounter == 9 && Kcolor == 'B')
		return checkmate;
	return safe;
}

void date_filename(char *buf, int ln)
{
	time_t t_epc = time(NULL);
	struct tm t;

	t = *localtime(&t_epc);
	strftime(buf, ln, "%a %Y-%m-%d %H%M%S.txt", &t);
}

void write_to_log(int round, FILE* logf, char *plInput, char piece[2])
{
	static unsigned short c = 1;

	if (round == WHITE) {
		fprintf(logf, "Round  #%d:\tWhite moves ", c);
	} else {
		fprintf(logf, "            \tBlack moves ");
		c++;
	}
	if (plInput[0] == 'P') {
		fprintf(logf, "pawn ");
	} else if (plInput[0] == 'R') {
		fprintf(logf, "rook ");
	} else if (plInput[0] == 'N') {
		fprintf(logf, "knight ");
	} else if (plInput[0] == 'B') {
		fprintf(logf, "bishop ");
	} else if (plInput[0] == 'Q') {
		fprintf(logf, "queen ");
	} else if (plInput[0] == 'K') {
		fprintf(logf, "king ");
	}
	fprintf(logf, "from %c%c to %c%c\n", piece[0], piece[1], plInput[1], plInput[2]);
}

char *getPlayerInput(void)
{
	size_t len = 0, max = 1;
	int count = 0;
	char c = 0;
	char *str_in = calloc(max, 1);
	char *str_temp = str_in;

	if(!str_in)
		return str_in;
	while ((c = getchar()) != '\n') {
		str_in[len++] = c;
		if (len == max) {
			max++;
			str_temp = realloc(str_in, max);
			if (!str_temp) {
				return str_in;
			} else {
				str_in = str_temp;
			}
		} else {
			clear_buffer();
			break;
		}
		count++;
	}
	if (!count) {
		str_in[1] = '\0';
		fflush(stdin);
		return str_in;
	} else
		str_in[len] = '\0';
	return realloc(str_in, len);
}

char *pieceConflict(const char *piece_pos, const char p)
{
	static char *temp, fpiece[3], name[7];

	switch (p) {
		case 'P':
			strcpy(name, "Pawn");
			break;
		case 'B':
			strcpy(name, "Bishop");
			break;
		case 'R':
			strcpy(name, "Rook");
			break;
		case 'N':
			strcpy(name, "Knight");
			break;
		default:
			strcpy(name, " ");
			break;
	}
	if (piece_pos[0] != piece_pos[2]) {
		printf("%s %s %s %s?\n%s", "Did you mean to move the left",  
				name, "or the right", name,
				"Please specify with either 'left' or 'right': ");
		do {
			temp = getPlayerInput();
		} while (strcmp(temp, "left") != 0 && strcmp(temp, "right") != 0);
		if ((strcmp(temp, "right") == 0 && piece_pos[0] < piece_pos[2]) || (strcmp(temp, "left") == 0 && piece_pos[0] > piece_pos[2])) {
			fpiece[0] = piece_pos[2];
			fpiece[1] = piece_pos[3];
		} else {
			memcpy(fpiece, piece_pos, 2);
		}
	} else {
		printf("%s %s %s %s %s\n%s", "Did you mean to move the",  
				name, "above or the", name, "below?",
				"Please specify with either 'up' or 'down': ");
		do {
			temp = getPlayerInput();
		} while (strcmp(temp, "up") != 0 && strcmp(temp, "down") != 0);
		if ((strcmp(temp, "up") == 0 && piece_pos[1] > piece_pos[3]) || (strcmp(temp, "down") == 0 && piece_pos[1] < piece_pos[3])) {
			fpiece[0] = piece_pos[2];
			fpiece[1] = piece_pos[3];
		} else {
			memcpy(fpiece, piece_pos, 2);
		}
	}
	fpiece[2] = '\0';
	return fpiece;
}

extern void printBanner(const char *banner)
{
	int i, j, c = 0, len = (int)strlen(banner);
#if !defined (__MINGW32__) || !defined(_WIN32)
	struct timespec *start_t = malloc(sizeof(struct timespec));
	struct timespec *end_t = malloc(sizeof(struct timespec));

	*start_t = (struct timespec){0, (BANNER_SPEED)*1000000};
	*end_t = (struct timespec){0, (BANNER_SPEED)*1000000};
#endif
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 2*len; j++) {
			if (!j || !i || i == 2 || j == (2*len - 1))
				printf("*");
			else if (j >= (int)((2*len)/4) && c < len && i == 1)
				printf("%c", banner[c++]);
			else
				printf(" ");
			fflush(stdout);
#if !defined(__MINGW32__) || !defined(_WIN32)
			nanosleep(start_t, end_t);
#else
			Sleep(BANNER_SPEED);
#endif
		}
		printf("\n");
	}
#if !defined (__MINGW32__) || !defined(_WIN32)
	free(start_t);
	free(end_t);
#endif
}
