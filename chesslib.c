#include "chesslib.h"


extern void clear_buffer(void)
{
	char clbuf;
	while ((clbuf=getchar()) != '\n');
}

void initChessboard(ch_template chb[][8], unsigned k, char col)	//k is row, col is column
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
			chb[k][col - 65].c = WHITE;	//colorize the pieces 
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
	int i, j;

	printf("    a   b   c   d   e   f   g   h  \n");
	for (i = 0; i < 8; i++) {
		printf("%d | ", i + 1);
		for (j = 0; j < 8; j++) {
			if (chb[i][j].occ == false) {
				printf("  | ");
			} else {
				if (chb[i][j].c == BLACK)
					printf("%s%c%s | ", KRED, chb[i][j].current, RESET);
				else
					printf("%s%c%s | ", KYEL, chb[i][j].current, RESET);
			}
		}
		printf("%d", i + 1);
		printf("\n");
	}
	printf("    a   b   c   d   e   f   g   h  \n");
	printf("\n\n");
}

bool validInput(const char *input, int *errPtr)
{
	if (!strcmp(input, "help")) {
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
	int i, j, k, l, count, double_attack = 3;	/*double_attack: int to check for two pieces attacking the same piece*/
	char *retvalue = calloc(3, sizeof(*retvalue));
	char temp[2];
	
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
						if ((i == 6) && (l == i - 2) && (j == k) && (chb[l][k].occ == false)) 
							return retvalue;	//check for castling
						if ((i-1 == 0) && (chb[i-1][j].square[0] == input[1]) && (chb[i-1][j].square[1] == input[2]) && (chb[i-1][j].occ == false)) {
							chb[i-1][j].current = 'Q';
							chb[i-1][j].occ = true;
							chb[i][j].current = 'e';
							chb[i][j].occ = false;
							free(retvalue);
							return "qn";
						}
						if ((i-1 == 0) && (chb[i-1][j+1].square[0] == input[1] && chb[i-1][j+1].square[1] == input[2]) && (chb[i-1][j+1].occ == true) && (chb[i-1][j+1].c != color)) {
							chb[i-1][j+1].current = 'Q';
							chb[i][j].current = 'e';
							chb[i][j].occ = false;
							free(retvalue);
							return "qn";
						}
						if ((i-1 == 0) && (chb[i-1][j-1].square[0] == input[1] && chb[i-1][j-1].square[1] == input[2]) && (chb[i-1][j-1].occ == true) && (chb[i-1][j-1].c != color)) {
							chb[i-1][j-1].current = 'Q';
							chb[i][j].current = 'e';
							chb[i][j].occ = false;
							free(retvalue);
							return "qn";
						}
						if (chb[i-1][j].occ == false) {
							if (chb[i-1][j].square[0] == input[1] && chb[i-1][j].square[1] == input[2]) {
								return retvalue;
							}
						}
						if (chb[i-1][j+1].occ == true) {
							if (chb[i-1][j+1].square[0] == input[1] && chb[i-1][j+1].square[1] == input[2]) {
								if (temp[0] != retvalue[0] && temp[1] == retvalue[1]) {
									double_attack = true;
									goto EXIT_LOOP;
								}
								double_attack = false;
								temp[0] = retvalue[0];
								temp[1] = retvalue[1];
							}
						} 
						if (chb[i-1][j-1].occ == true) {
							if (chb[i-1][j-1].square[0] == input[1] && chb[i-1][j-1].square[1] == input[2]) {
								if (temp[0] != retvalue[0] && temp[1] == retvalue[1]) {
									double_attack = true;
									goto EXIT_LOOP;
								}
								double_attack = false;
								temp[0] = retvalue[0];
								temp[1] = retvalue[1];
							}
						}
					} else {
						if ((i == 1) && (l == i + 2) && (j == k) && (chb[l][k].occ == false))
							return retvalue;	//check for castling
						if ((i+1) == 0 && chb[i+1][j].square[0] == input[1] && chb[i+1][j].square[1] == input[2] && chb[i+1][j].occ == false) {
							chb[i+1][j].current = 'Q';
							chb[i-1][j].occ = true;
							chb[i][j].current = 'e';
							chb[i][j].occ = false;
							free(retvalue);
							return "qn";
						}
						if ((i+1) == 0 && chb[i+1][j+1].square[0] == input[1] && chb[i+1][j+1].square[1] == input[2] && chb[i+1][j+1].occ == true && chb[i+1][j+1].c != color) {
							chb[i+1][j+1].current = 'Q';
							chb[i][j].current = 'e';
							chb[i][j].occ = false;
							free(retvalue);
							return "qn";
						}
						if ((i+1) == 0 && chb[i+1][j-1].square[0] == input[1] && chb[i+1][j-1].square[1] == input[2] && chb[i+1][j-1].occ == true && chb[i-1][j-1].c != color) {
							chb[i+1][j-1].current = 'Q';
							chb[i][j].current = 'e';
							chb[i][j].occ = false;
							free(retvalue);
							return "qn";
						}
						if (chb[i+1][j].occ == false){
							if (chb[i+1][j].square[0] == input[1] && chb[i+1][j].square[1] == input[2]) {
								return retvalue;
							}
						}
						if (chb[i+1][j+1].occ == true){
							if (chb[i+1][j+1].square[0] == input[1] && chb[i+1][j+1].square[1] == input[2]) {
								if (temp[0] != retvalue[0] && temp[1] == retvalue[1]) {
									double_attack = true;
									goto EXIT_LOOP;
								}
								double_attack = false;
								temp[0] = retvalue[0];
								temp[1] = retvalue[1];
							}
						}
						if (chb[i+1][j-1].occ == true){
							if (chb[i+1][j-1].square[0] == input[1] && chb[i+1][j-1].square[1] == input[2]) {
								if (temp[0] != retvalue[0] && temp[1] == retvalue[1]) {
									double_attack = true;
									goto EXIT_LOOP;
								}
								double_attack = false;
								temp[0] = retvalue[0];
								temp[1] = retvalue[1];
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
							return retvalue;
						}
					}
				} else if (input[0] == 'R' || input[0] == 'Q') {
					k = i;
					for (l=0; l < 8; l++) {
						if (l == j) 
							continue;	//to skip the piece itself
						if (chb[k][l].square[0] == input[1] && chb[k][l].square[1] == input[2]) {
							return retvalue;
						}
					}
					l = j;
					for (k = 0; k < 8; k++) {
						if (k == i)
							continue;
						if (chb[k][l].square[0] == input[1] && chb[k][l].square[1] == input[2]) {
							return retvalue;
						}
					}
				}
				if (input[0] == 'B' || input[0] == 'Q') {	//different if, to check for queen's diagonal moves
					k = i - 1; 
					l = j - 1;
					while ((k <= 7 && k >= 0) && (l >= 0 && l <= 7)) {
						if (chb[k][l].square[0] == input[1] && chb[k][l].square[1] == input[2]) {
							return retvalue;
						}
						k--;
						l--;
					}
					k = i - 1;
					l = j + 1;
					while ((k >= 0 && k <= 7) && (l <= 7 && l >= 0)) {
						if (chb[k][l].square[0] == input[1] && chb[k][l].square[1] == input[2]) {
							return retvalue;
						}
						k--;
						l++;
					} 
					k = i + 1;
					l = j - 1;
					while ((k <= 7 && k >= 0) && (l >= 0 && l <= 0)) {
						if (chb[k][l].square[0] == input[1] && chb[k][l].square[1] == input[2]) {
							return retvalue;
						}
						k++;
						l--;
					}
					k = i + 1;
					l = j + 1;
					while ((k <= 7 && k >= 0) && (l <= 7 && l >= 0)) {
						if (chb[k][l].square[0] == input[1] && chb[k][l].square[1] == input[2]) {
							return retvalue;
						}
						k++;
						l++;
					}
				}
			}
		}
	}
	EXIT_LOOP:
	if (double_attack == true) {
		retvalue = realloc(retvalue, 5);
		retvalue[2] = temp[0];
		retvalue[3] = temp[1];
		retvalue[4] = '\0';
		return retvalue;
	} else if(double_attack == false) {
		return retvalue;
	}
	return NULL;
}

extern void printInstructions(void)
{
	printf("%s%s\n%s\n%s\n%s\n%s\n\t%s\n\n\n",
	"Please enter your move in the following format: ", "\'xyz\'", 
	"where x is the piece you want to move,",
	"y is the letter of the column and z is the number of the row",
	"acceptable values for x are: R for Rook, N for Knight, B for Bishop, Q for Queen, K for King and P for Pawn",
	"acceptable values for y are lowercase letters from \'a\' to \'h\' and for z numbers from 1 to 8",
	"for example to move Bishop to e2 type Be2 or Pawn to a4 type Pa4");
}

extern void clear_screen(void)
{
	char buf[1024];
	char *str;
	
	tgetent(buf, getenv("TERM"));
	str = tgetstr("cl", NULL);
	fputs(str, stdout);
	/*puts( "\033[2J" ); */
	/*clear screen using ASCII; doesn't work as well, only use it if you can't install libncurses
	 *don't forget to delete or comment lines 324-331*/
} 

bool movePiece(ch_template chb[][8], char *plInput, char piece[2], int color)
{
	int startx, starty, endx, endy; //cords for the current tile and for the tile to move the piece to
	
	endx = plInput[2] - '1';
	endy = plInput[1] - 65;
	startx = piece[1] - '1';
	starty = piece[0] - 65;
	if (piecesOverlap(chb, startx, starty, endx, endy, plInput[0]) == false) {
		if (chb[endx][endy].c != color) {
			chb[startx][starty].occ = false;
			chb[startx][starty].c = EMPTY;
			chb[endx][endy].occ = true;
			chb[endx][endy].current = chb[startx][starty].current;
			chb[endx][endy].c = color;
			return true;
		}
	}
	return false;
}

bool piecesOverlap(ch_template chb[][8], const int sx, const int sy, const int ex, const int ey, char piece)
{
	/*int tempx = sx, tempy = sy;
	unsigned p_distance;
	
	if (piece == 'R' || piece == 'Q') {
		p_distance = (sx == ex)? abs(sy-ey):abs(sx-ex);
		if ((sx-ex)) {
			if (sy > ey) {
				printf("t\n");
				for (; tempy > ey; --tempy)
					printf("startx %d starty %d endx %d endy %d \n", tempx, tempy, ex, ey);
					if (chb[sx][tempy].occ)
						return true;
			} else {
				printf("t2\n");
				for (; tempy < ey; ++tempy)
					printf("startx %d starty %d endx %d endy %d \n", tempx, tempy, ex, ey);
					if (chb[sx][tempy].occ)
						return true;
			}
		} else {
			if (sx > ex) {
				printf("t3\n");
				for (; tempx > ex; --tempx)
					printf("startx %d starty %d endx %d endy %d \n", tempx, tempy, ex, ey);
					if (chb[tempx][sy].occ)
						return true;
			} else {
				printf("t4\n");
				for (; tempx < ex; ++tempx)
					printf("startx %d starty %d endx %d endy %d \n", tempx, tempy, ex, ey);
					if (chb[tempx][sy].occ)
						return true;
			}
		}
	}*/
	return false;
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
	static unsigned c = 1;

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
	char c, count = 0, *str_in = calloc(max, sizeof(*str_in)), *str_temp = str_in;
	
	if (!str_in)
		return str_in;
	while ((c = getchar()) != '\n') {
		str_in[len++] = c;
		if (len == max && len <= 5){
			str_temp = realloc(str_in, max*sizeof(*str_in));
			if (!str_temp){ 
				return str_in;
			} else {
				str_in = str_temp;
			}
			max+=1;
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
	} else {
		str_in[len] = '\0';
	}
	return realloc(str_in, len*sizeof(*str_in));
}

char *pawnConflict(const char *pawn_pos)
{
	static char *temp, fpawn[3];
	
	printf("%s\n%s", "Did you mean to move the left Pawn or the right Pawn?",
				  "Please specify with either 'left' or 'right': ");
	do {
		temp = getPlayerInput();
	} while (strcmp(temp, "left") != 0 && strcmp(temp, "right") != 0);
	if (strcmp(temp, "right") == 0) {
		memcpy(fpawn, pawn_pos, 2);
	} else {
		fpawn[0] = pawn_pos[2];
		fpawn[1] = pawn_pos[3];
	}
	fpawn[2] = '\0';
	return fpawn;
}

extern void printBanner(const char *banner)
{
	struct timespec *start_t = malloc(sizeof(struct timespec));
	struct timespec *end_t = malloc(sizeof(struct timespec));
	int i, j, c = 0, len = (int)strlen(banner);
	
	*start_t = (struct timespec){0, (BANNER_SPEED)*1000000};
	*end_t = (struct timespec){0, (BANNER_SPEED)*1000000};
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 2*len; j++) {
			if (!j || !i || i == 2 || j == (2*len - 1))
				printf("*");
			else if (j >= (int)((2*len)/4) && c < len && i == 1)
				printf("%c", banner[c++]);
			else
				printf(" ");
			fflush(stdout);
			nanosleep(start_t, end_t);
		}
		printf("\n");
	}
	free(start_t);
	free(end_t);
}
