/*********************************************************************
 *                          chlib-computer.c                         *
 *            computer player implementation for chesslib            *
 *                                                                   *
 *               (C)2015 <georgekoskerid@outlook.com>                *
 *                                                                   *
 *********************************************************************/

#include <chesslib.h>

#define NULL_50(x)                   \
{                                    \
    for (int g = 0; g < 50; g++) {   \
        x[g] = NULL;                 \
    }                                \
}

/*struct for each node/leaf of the AI move tree; the number of children is statically allocated*/
typedef struct TreeNode {
	char start[3], end[3];
	int color, score;
	unsigned short depth;
	struct TreeNode *child[50];
	struct TreeNode *parent;
} TreeNode;


/*********
 *globals* 
 *********/

static unsigned short max_depth;
static TreeNode *top;


/***********************************************************************
 *prototypes for functions used only in chlib-computer.c and chesslib.c*
 ***********************************************************************/

void _copyBoard(ch_template to[][8], ch_template from[][8]);
bool _makeMove(ch_template chb[][8], char *st_move, char *en_move, const int color, const bool ListCheck);


/********************************************************
 *prototypes for functions used only in chlib-computer.c*
 ********************************************************/

/*void _createCPUMoveTree(ch_template chb[][8], const int color);


char *getAImove(ch_template chb[][8], const int color, const unsigned short depth)
{
	max_depth = depth;
	if (!max_depth)
		return NULL;

	char *retvalue = NULL;


	max_depth = 0;
	return retvalue;
}

void _createCPUMoveTree(ch_template chb[][8], const int color)
{
	ch_template next_chb[8][8];
	TreeNode *top = malloc(sizeof(TreeNode));
	top->depth = 0;
	NULL_50(top->child);
	top->parent = NULL;
	TreeNode *currAILeaf = top;
	
	MoveNode *curr_moves[6];
	for (int i = 0; i < 6; i++) {
		curr_moves[i] = (color == BLACK)?b_moves:w_moves;
	}
	while (currAILeaf->depth != max_depth) {
		copyBoard(next_chb, chb);
		for (int i = 0; i < 6; i++) {
			
		}
	}

	return top;
}
*/