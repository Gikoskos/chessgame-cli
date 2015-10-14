/*********************************************************************
 *                          chlib-computer.c                         *
 *            computer player implementation for chesslib            *
 *                                                                   *
 *               (C)2015 <georgekoskerid@outlook.com>                *
 *                                                                   *
 *********************************************************************/

#include <chesslib.h>

/*typedef struct AItree {
	char start[3];
	char end[3];
	unsigned int depth;
	struct AItree *child[50] = {NULL};
} AItree;

static unsigned int max_depth;

void getAImove(ch_template chb[][8], const int color)
{
	if (!max_depth)
		return;

	MoveNode *currMove[6];
	AItree *top = malloc(sizeof(AItree));
	top->depth = 0;
	AItree *currAILeaf = top;
	ch_template next_chb[][8];
	unsigned int depth_count = 0;

	if (color == BLACK)
		currMove = b_moves;
	else
		currMove = w_moves;

	while (currAILeaf->depth != max_depth) {
		copyBoard(next_chb, chb);
		if (depth_count)
			gett
		for (int i = 0; i < 6; i++) {
			
		}
	}
	
}

void setAIDepth(unsigned int depth)
{
	max_depth = depth;
}

void deleteTree*/