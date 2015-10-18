/*********************************************************************
 *                            chlib-cli.c                            *
 *               functions for command-line chess game               *
 *                                                                   *
 *                  (C)2015 <cyberchiller@gmail.com>                 *
 *                                                                   *
 *********************************************************************/

#include <chesslib.h>
#include <chlib-cli.h>

extern void clear_buffer(void)
{
	char clbuf;
	while ((clbuf=getchar()) != '\n');
}

void printBoard(ch_template chb[][8], const char p)
{
#ifdef _WIN32
	HANDLE cmdhandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO cmdinfo;
	WORD sv_att;
	int i, j, max, y = 0, x = 0;

	GetConsoleScreenBufferInfo(cmdhandle, &cmdinfo);
	sv_att = cmdinfo.wAttributes;

	printf("    a   b   c   d   e   f   g   h  \n");
	for (i = 0; i < (max = (MOS%2)?MOS:(MOS-1)); i++) {
		if(i%2 == true) printf("%d ", y + 1);
		else printf("  ");
		for (j = 0; j < max; j++) {
			if (!i) { 
				if(!j) printf("%c", 218);
				else if(j == (max-1)) printf("%c", 191);
				else if(j%2) printf("%c%c%c", 196,196,196);
				else if(!(j%2)) printf("%c", 194);
			} else if (i == max-1) {
				if(!j)printf("%c", 192);
				else if(j == (max-1)) printf("%c", 217);
				else if(j%2) printf("%c%c%c", 196,196,196);
				else if(!(j%2)) printf("%c", 193);
			} else if (i%2) {
				if(!(j%2)) printf("%c", 179);
				else {
					if (chb[y][x].occ == false)
						printf("   ");
					else {
						if (chb[y][x].c == BLACK)
							SetConsoleTextAttribute(cmdhandle, FOREGROUND_RED); 
						else
							SetConsoleTextAttribute(cmdhandle, FOREGROUND_GREEN);
						printf(" %c ", chb[y][x].current);
						SetConsoleTextAttribute(cmdhandle, sv_att);
					}
					x++;
				}
			} else if (!(i%2)) {
				if(!j) printf("%c", 195);
				else if(j == (max-1)) printf("%c", 180);
				else if(j%2) printf("%c%c%c", 196,196,196);
				else if(!(j%2)) printf("%c", 197);
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
#else
	int i, j, max, y = 0, x = 0;

		printf("   \033[1m a   b   c   d   e   f   g   h \033[0m \n");
	for (i = 0; i < (max = (MOS%2)?MOS:(MOS-1)); i++) {
		if(i%2 == true) printf("\033[1m%d\033[0m ", 8 - y);
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
						if (p == 'a') {
							if (chb[y][x].c == BLACK)
								printf(" %s%c%s ", KRED, chb[y][x].current, RESET);
							else
								printf(" %s%c%s ", KYEL, chb[y][x].current, RESET);
						} else {
							if (chb[y][x].current == 'P') {
								if (chb[y][x].c == BLACK)
									printf(" \u265F ");
								else
									printf(" \u2659 ");
							} else if (chb[y][x].current == 'Q') {
								if (chb[y][x].c == BLACK)
									printf(" \u265B ");
								else
									printf(" \u2655 ");
							} else if (chb[y][x].current == 'B') {
								if (chb[y][x].c == BLACK)
									printf(" \u265D ");
								else
									printf(" \u2657 ");
							} else if (chb[y][x].current == 'R') {
								if (chb[y][x].c == BLACK)
									printf(" \u265C ");
								else
									printf(" \u2656 ");
							} else if (chb[y][x].current == 'N') {
								if (chb[y][x].c == BLACK)
									printf(" \u265E ");
								else
									printf(" \u2658 ");
							} else if (chb[y][x].current == 'K') {
								if (chb[y][x].c == BLACK)
									printf(" \u265A ");
								else
									printf(" \u2654 ");
							}
						}
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
			printf(" \033[1m%d\033[0m", 8 - y);
			y++;
		} else
			printf("  ");
		printf("\n");
	}
	printf("   \033[1m a   b   c   d   e   f   g   h \033[0m \n");
	printf("\n");
#endif
}

bool validInput(const char *input, int *errPtr)
{
	if (strlen(input) > 3) {
		return false;
	}
	if (!strchr("rnbqkpRNBQKP", *input)) {
		*errPtr = 5;
		return false;
	}
	if (!strchr("abcdefghABCDEFGH", input[1])) {
		*errPtr = 6;
		return false;
	}
	if (!strchr("12345678", input[2])) {
		*errPtr = 7;
		return false;
	}
	return true;
}

extern void printBanner(const char *banner)
{
	int i, j, c = 0, len = (int)strlen(banner);
#if !defined (__MINGW32__) || !defined(_WIN32)
	struct timespec *start_t = alloca(sizeof(struct timespec));
	struct timespec *end_t = alloca(sizeof(struct timespec));

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
}

char *getPlayerInput(void)
{
	size_t len = 0, max = 1;
	int count = 0;
	int c = 0;
	char *str_in = malloc(max);
	char *str_temp = NULL;

	if(!str_in)
		return str_in;
	while ((c = fgetc(stdin)) != '\n') {
		str_in[len++] = c;
		if (len == max) {
			max++;
			str_temp = realloc(str_in, max);
			if (!str_temp) {
				return str_in;
			} else {
				str_in = str_temp;
				str_temp = NULL;
			}
		}
		count++;
	}
	if (!count) {
		str_in[1] = '\0';
		return str_in;
	} else
		str_in[len] = '\0';
	return realloc(str_in, len);
}

extern void clear_screen(void)
{
#ifndef _WIN32
# if 0
	char buf[1024];
	char *str;

	tgetent(buf, getenv("TERM"));
	str = tgetstr("cl", NULL);
	fputs(str, stdout);
# else
	/*Weird stuff: I'm using system("clear") because if I try to 
	 *compile it with tgetent and tgetstr I get double free corruption,
	 *which I don't get if I compile without AI. If you know 
	 *what's happening please contact me.*/
	system("clear");
# endif
#else
	system("cls");
#endif
	/*puts( "\033[2J" );
	 *Note: Clear screen using ASCII; doesn't look that good.
	 *Only use it if you can't install libncurses and
	 *don't forget to delete or comment lines 614-623*/
}
