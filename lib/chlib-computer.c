/*********************************************************************
 *                          chlib-computer.c                         *
 *            computer player implementation for chesslib            *
 *                                                                   *
 *               (C)2015 <georgekoskerid@outlook.com>                *
 *                                                                   *
 *********************************************************************/

#include <chesslib.h>

int _fillMoveLists(ch_template chb[][8], MoveNode ***move_array, int flag);

#define NULL50(x)                    \
{                                    \
    for (int g = 0; g < 50; g++) {   \
        x[g] = NULL;                 \
    }                                \
}

typedef struct TreeNode {
	char start[3], end[3];
	int player, score;
	unsigned short depth;
	struct TreeNode *child[50];
} TreeNode;

static unsigned short max_depth;

void getAImove(ch_template chb[][8], const int color, const unsigned short depth)
{
	max_depth = depth;
	if (!max_depth)
		return;

	unsigned int depth_count = 0;

	MoveNode **currMove;
	TreeNode *top = malloc(sizeof(TreeNode));
	top->depth = 0;
	NULL50(top->child);
	TreeNode *currAILeaf = top;
	ch_template next_chb[8][8];

	if (color == BLACK)
		currMove = b_moves;
	else
		currMove = w_moves;

	while (currAILeaf->depth != max_depth) {
	}
	max_depth = 0;
}
