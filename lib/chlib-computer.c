/*********************************************************************
 *                          chlib-computer.c                         *
 *            computer player implementation for chesslib            *
 *                                                                   *
 *                 (C)2015 <cyberchiller@gmail.com>                  *
 *                                                                   *
 *********************************************************************/

#include <chesslib.h>

#define MOVE_COUNT 50

#define NULL_50(x)                           \
{                                            \
    for (int g = 0; g < MOVE_COUNT; g++) {   \
        x[g] = NULL;                         \
    }                                        \
}

/*struct for each node/leaf of the AI move tree; the number of children is statically allocated*/
typedef struct TreeNode {
	char start[3], end[3];
	int color, score;
	unsigned short depth;
	struct TreeNode *child[MOVE_COUNT];
	struct TreeNode *parent;
} TreeNode;


/*********
 *globals* 
 *********/

static unsigned short max_depth;


/***********************************************************************
 *prototypes for functions used only in chlib-computer.c and chesslib.c*
 ***********************************************************************/

void _copyBoard(ch_template to[][8], ch_template from[][8]);
bool _makeMove(ch_template chb[][8], char *st_move, char *en_move, const int color, const bool ListCheck);


/********************************************************
 *prototypes for functions used only in chlib-computer.c*
 ********************************************************/

void _createAIMoveTree(TreeNode **curr_leaf, ch_template chb[][8], const int color, unsigned short depth_count);


char *getAImove(ch_template chb[][8], const int color, const unsigned short depth)
{
	if (!depth)
		return NULL;

	max_depth = depth;
	TreeNode *top = NULL;
	char *retvalue = NULL;
	_createAIMoveTree(&top, chb, color, 0);


	max_depth = 0;
	return retvalue;
}

/*void _printAIMoveTree(TreeNode *curr_leaf, unsigned short depth_count)
{
	for (int i = 0; i < MOVE_COUNT; i++) {
		
	}
}*/

void _createAIMoveTree(TreeNode **curr_leaf, ch_template chb[][8], const int color, unsigned short depth_count)
{
	ch_template next_chb[8][8];
	int move_list_count = 0;

	MoveNode *temp_moves[6];
	getAllMoves(chb, color);
	for (int i = 0; i < 6; i++) {
		temp_moves[i] = (color == BLACK)?b_moves[i]:w_moves[i];
	}
	if (!depth_count) {
		(*curr_leaf) = malloc(sizeof(TreeNode));
		(*curr_leaf)->depth = 0;
		(*curr_leaf)->parent = NULL;
	}
	for (int i = 0; i < MOVE_COUNT; i++) {
		if (!temp_moves[move_list_count]) move_list_count++;
		if (move_list_count > 5) {
			(*curr_leaf)->child[i] = NULL;
		} else {
			(*curr_leaf)->child[i] = malloc(sizeof(TreeNode));
			(*curr_leaf)->child[i]->parent = (*curr_leaf);
			(*curr_leaf)->child[i]->color = color;
			(*curr_leaf)->child[i]->depth = depth_count+1;
			memcpy((*curr_leaf)->child[i]->start, temp_moves[move_list_count]->start, 3);
			memcpy((*curr_leaf)->child[i]->end, temp_moves[move_list_count]->end, 3);
			temp_moves[move_list_count] = temp_moves[move_list_count]->nxt;
		}
	}

	if (depth_count <= max_depth) {
		for (int i = 0; i < MOVE_COUNT; i++) {
			if (!(*curr_leaf)->child[i]) {
				return;
			}
			_copyBoard(next_chb, chb);
			_makeMove(next_chb, (*curr_leaf)->child[i]->start, (*curr_leaf)->child[i]->end, (*curr_leaf)->child[i]->color, false);
			_createAIMoveTree(&((*curr_leaf)->child[i]), next_chb, (color == BLACK)?WHITE:BLACK, depth_count+1);
		}
	}
}
