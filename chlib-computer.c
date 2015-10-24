/*********************************************************************
 *                            computer.c                             *
 *            computer player implementation for chesslib            *
 *                                                                   *
 *               (C)2015 <georgekoskerid@outlook.com>                *
 *                                                                   *
 *********************************************************************/

#include "chesslib.h"

static unsigned total_moves;

typedef struct PMoveNode {
	char move[4];
	struct PMoveNode *nxt;
} PMoveNode;


PMoveNode *addMove(PMoveNode *head, char m[4])
{
	total_moves++;
	if (!head) {
		head = malloc(sizeof(PMoveNode));
		memcpy(head->move, m, 4);
		head->nxt = NULL;
		return head;
	}
	PMoveNode *new = malloc(sizeof(PMoveNode));
	memcpy(new->move, m, 4);
	new->nxt = head;
	return new;
}

#ifndef NDEBUG
void printList(PMoveNode *head)
{
	if (!head)
		return;
	do {
		printf("%s\n", head->move);
		head = head->nxt;
	} while (head);
}
#endif

void freeList(PMoveNode *head)
{
	PMoveNode *temp;
	while (head != NULL) {
		temp = head;
		head = head->nxt;
		free(temp);
	}
}

PMoveNode *getMoves(PMoveNode *AIlist, ch_template chb[][8])
{
	int i, j, k, l;
	char tmp[4];
	
	tmp[3] = '\0';
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if(chb[i][j].c == BLACK) {
				tmp[0] = chb[i][j].current;
				if (chb[i][j].current == 'P') {
					tmp[2] = i-1 + '1';
					if (chb[i-1][j].occ == false) {
						tmp[1] = j + 'A';
						AIlist = addMove(AIlist, tmp);
					}
					if (chb[i-1][j+1].occ == true && chb[i-1][j+1].c != BLACK) {
						tmp[1] = j+1 + 'A';
						AIlist = addMove(AIlist, tmp);
					} 
					if (chb[i-1][j-1].occ == true && chb[i-1][j-1].c != BLACK) {
						tmp[1] = j-1 + 'A';
						AIlist = addMove(AIlist, tmp);
					}
				}
				if (chb[i][j].current == 'B' || chb[i][j].current == 'Q') {
					k = i - 1; 
					l = j - 1;
					while ((k <= 7 && k >= 0) && (l >= 0 && l <= 7)) {
						if (chb[k][l].c != chb[i][j].c) {
							tmp[1] = l + 'A';
							tmp[2] = k + '1';
							AIlist = addMove(AIlist, tmp);
						}
						if (chb[k][l].occ == true) break;
						k--;
						l--;
					}
					k = i - 1;
					l = j + 1;
					while ((k >= 0 && k <= 7) && (l <= 7 && l >= 0)) {
						if (chb[k][l].c != chb[i][j].c) {
							tmp[1] = l + 'A';
							tmp[2] = k + '1';
							AIlist = addMove(AIlist, tmp);
						}
						if (chb[k][l].occ == true) break;
						k--;
						l++;
					} 
					k = i + 1;
					l = j - 1;
					while ((k <= 7 && k >= 0) && (l >= 0 && l <= 7)) {
						if (chb[k][l].c != chb[i][j].c) {
							tmp[1] = l + 'A';
							tmp[2] = k + '1';
							AIlist = addMove(AIlist, tmp);
						}
						if (chb[k][l].occ == true) break;
						k++;
						l--;
					}
					k = i + 1;
					l = j + 1;
					while ((k <= 7 && k >= 0) && (l <= 7 && l >= 0)) {
						if (chb[k][l].c != chb[i][j].c) {
							tmp[1] = l + 'A';
							tmp[2] = k + '1';
							AIlist = addMove(AIlist, tmp);
						}
						if (chb[k][l].occ == true) break;
						k++;
						l++;
					}
				}
				if (chb[i][j].current == 'N') {
					int knightrow[] = {i-2,i-2,i-1,i-1,i+1,i+1,i+2,i+2};
					int knightcol[] = {j-1,j+1,j-2,j+2,j-2,j+2,j-1,j+1};
					int count = 0;
					for (; count < 8; count++) {
						if (chb[knightrow[count]][knightcol[count]].c != chb[i][j].c) {
							if (knightrow[count] > -1 && knightrow[count] < 8
								&& knightcol[count] > -1 && knightcol[count] < 8) {
								tmp[1] = knightcol[count] + 'A';
								tmp[2] = knightrow[count] + '1';
								AIlist = addMove(AIlist, tmp);
							}
						}
					}
				}
				if (chb[i][j].current == 'R' || chb[i][j].current == 'Q') {
					k = i;
					for (l = j+1; l <= 7; l++) {
						if (chb[k][l].c == chb[i][j].c)
							break;
						tmp[1] = chb[k][l].square[0];
						tmp[2] = chb[k][l].square[1];
						AIlist = addMove(AIlist, tmp);
						if (chb[k][l].occ == true)
							break;
					}
					for (l = j-1; l >= 0; l--) {
						if (chb[k][l].c == chb[i][j].c)
							break;
						tmp[1] = chb[k][l].square[0];
						tmp[2] = chb[k][l].square[1];
						AIlist = addMove(AIlist, tmp);
						if (chb[k][l].occ == true)
							break;
					}
					
					l = j;
					for (k = i+1; k <= 7; k++) {
						if (chb[k][l].c == chb[i][j].c)
							break;
						tmp[1] = chb[k][l].square[0];
						tmp[2] = chb[k][l].square[1];
						AIlist = addMove(AIlist, tmp);
						if (chb[k][l].occ == true)
							break;
					}
					for (k = i-1; k >= 0; k--) {
						if (chb[k][l].c == chb[i][j].c)
							break;
						tmp[1] = chb[k][l].square[0];
						tmp[2] = chb[k][l].square[1];
						AIlist = addMove(AIlist, tmp);
						if (chb[k][l].occ == true)
							break;
					}
				}
				if (chb[i][j].current == 'K') {
					for (k = i - 1; k < i + 2; k++){
						for (l = j - 1; l < j + 2; l++){
							if ((i == k && j == l) || (chb[k][l].c == chb[i][j].c)
								|| k > 7 || k < 0 || l > 7 || l < 0)
								continue;
							tmp[1] = chb[k][l].square[0];
							tmp[2] = chb[k][l].square[1];
							AIlist = addMove(AIlist, tmp);
						}
					}
				}
				if (check_castling.KBlack) {
					if (check_castling.BR_right && !piecesOverlap(chb, 7, ('H'-'A'), 7, 4, 'R'))
						AIlist = addMove(AIlist, "KG8");
					if (check_castling.BR_left && !piecesOverlap(chb, 7, ('A'-'A'), 7, 4, 'R'))
						AIlist = addMove(AIlist, "KC8");
				}
			}
		}
	}
	return AIlist;
}

char *getRandMove(PMoveNode *head)
{
	char *d = malloc(4);
	unsigned short t = 1, s;
	srand(time(NULL));
	s = rand()%total_moves;
	while (head!=NULL) {
		if (s == t) {
			memcpy(d, head->move, 4);
			break;
		}
		head = head->nxt;
		t++;
	}
	return d;
}

char *AImove(ch_template chb[][8])
{
	PMoveNode *AIl = NULL;
	char *retvalue = malloc(4), *temp = malloc(4);
	
	total_moves = 0;
	AIl = getMoves(AIl, chb);
	memcpy(temp, getRandMove(AIl), 4);
	strcpy(retvalue,temp);
	free(temp);
	freeList(AIl);
	return retvalue;
}
