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
typedef struct MoveTreeNode {
	char start[3], end[3];
	int color, score;
	unsigned short depth;
	struct MoveTreeNode *child[MOVE_COUNT];
	struct MoveTreeNode *parent;
} MoveTreeNode;


/*********
 *globals* 
 *********/

static unsigned short max_depth;
static int CPU_PLAYER;
unsigned short __attribute__((unused)) total_black_pieces;
unsigned short __attribute__((unused)) total_white_pieces;


/******************************************************************
 *prototypes for functions used in chlib-computer.c and chesslib.c*
 ******************************************************************/

void _copyBoard(ch_template to[][8], ch_template from[][8]);
bool _makeMove(ch_template chb[][8], char *st_move, char *en_move, const int color, const bool ListCheck);


/***************************************************
 *prototypes for functions used in chlib-computer.c*
 ***************************************************/

void _createAIMoveTree(MoveTreeNode **curr_leaf, ch_template chb[][8], const int color, const unsigned short depth_count);
void _printAIMoveTree(MoveTreeNode *curr_leaf, const int color);
int _Evaluate(ch_template chb[][8], const int color);


char *getAImove(ch_template chb[][8], const int color, const unsigned short depth)
{
	if (!depth || (color != BLACK && color != WHITE))
		return NULL;

	CPU_PLAYER = color;
	max_depth = depth;
	MoveTreeNode *top = NULL;
	char *retvalue = NULL;
	_createAIMoveTree(&top, chb, color, 0);
#ifdef DEBUG
	_printAIMoveTree(top, color);
#endif


	max_depth = 0;
	return retvalue;
}

void _printAIMoveTree(MoveTreeNode *curr_leaf, const int color)
{
	if (!curr_leaf)
		return;
	for (int i = 0; i < MOVE_COUNT; i++) {
		if (curr_leaf->depth > max_depth || (!curr_leaf->child[i]))
			return;
		if (!(curr_leaf->depth)) 
			printf("At depth %d, initial moves for %s are: %s->%s\n", curr_leaf->depth, (color == BLACK)?"Black":"White", curr_leaf->child[i]->start, curr_leaf->child[i]->end);
		else
			printf("At depth %d, for %s move %s->%s: %s->%s\n", curr_leaf->depth, (color == BLACK)?"Black's":"White's", curr_leaf->start, curr_leaf->end, curr_leaf->child[i]->start, curr_leaf->child[i]->end);
		_printAIMoveTree(curr_leaf->child[i], (color == WHITE)?BLACK:WHITE);
	}
}

void _createAIMoveTree(MoveTreeNode **curr_leaf, ch_template chb[][8], const int color, const unsigned short depth_count)
{
	ch_template next_chb[8][8];
	int move_list_count = 0;
	unsigned short no_move_count = 0;

	MoveNode *temp_moves[6];
	getAllMoves(chb, color);
	for (int i = 0; i < 6; i++) {
		temp_moves[i] = (color == BLACK)?b_moves[i]:w_moves[i];
		if (!temp_moves[i]) no_move_count++;
	}

	/*moves that lead to checkmate for given cpu player are not added to the tree*/
	if (!no_move_count && CPU_PLAYER == color) {
		return;
	}

	if (!depth_count) {
		(*curr_leaf) = malloc(sizeof(MoveTreeNode));
		(*curr_leaf)->depth = 0;
		(*curr_leaf)->parent = NULL;
	}
	for (int i = 0; i < MOVE_COUNT; i++) {
		while (!temp_moves[move_list_count]) {
			move_list_count++;
		}
		if (move_list_count > 5) {
			(*curr_leaf)->child[i] = NULL;
		} else {
			(*curr_leaf)->child[i] = malloc(sizeof(MoveTreeNode));
			(*curr_leaf)->child[i]->parent = (*curr_leaf);
			(*curr_leaf)->child[i]->color = color;
			(*curr_leaf)->child[i]->depth = depth_count+1;
			(*curr_leaf)->child[i]->start[0] = temp_moves[move_list_count]->start[0];
			(*curr_leaf)->child[i]->start[1] = temp_moves[move_list_count]->start[1];
			(*curr_leaf)->child[i]->start[2] = '\0';
			(*curr_leaf)->child[i]->end[0] = temp_moves[move_list_count]->end[0];
			(*curr_leaf)->child[i]->end[1] = temp_moves[move_list_count]->end[1];
			(*curr_leaf)->child[i]->end[2] = '\0';
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
#ifndef DEBUG
			(*curr_leaf)->child[i]->score = _Evaluate(next_chb, color);
#endif
			_createAIMoveTree(&((*curr_leaf)->child[i]), next_chb, (color == BLACK)?WHITE:BLACK, depth_count+1);
		}
	}
}

int _Evaluate(ch_template chb[][8], const int color)
{
	unsigned short b_pawn_count, b_queen_count, b_king_count, b_bishop_count, b_rook_count, b_knight_count;
	unsigned short w_pawn_count, w_queen_count, w_king_count, w_bishop_count, w_rook_count, w_knight_count;
	int black_material_balance, white_material_balance;

	b_pawn_count = b_queen_count = b_king_count = b_bishop_count = b_rook_count = b_knight_count = 0;
	w_pawn_count = w_queen_count = w_king_count = w_bishop_count = w_rook_count = w_knight_count = 0;
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if (chb[row][col].current == PAWN) (chb[row][col].c == BLACK)?(b_pawn_count++):(w_pawn_count++);
			if (chb[row][col].current == QUEEN) (chb[row][col].c == BLACK)?(b_queen_count++):(w_queen_count++);
			if (chb[row][col].current == KING) (chb[row][col].c == BLACK)?(b_king_count++):(w_king_count++);
			if (chb[row][col].current == BISHOP) (chb[row][col].c == BLACK)?(b_bishop_count++):(w_bishop_count++);
			if (chb[row][col].current == ROOK) (chb[row][col].c == BLACK)?(b_rook_count++):(w_rook_count++);
			if (chb[row][col].current == KNIGHT) (chb[row][col].c == BLACK)?(b_knight_count++):(w_knight_count++);
		}
	}
	black_material_balance = (b_pawn_count*100) + (b_queen_count*900) + (b_rook_count*500) + (b_bishop_count*325) + (b_knight_count*300);
	white_material_balance = (w_pawn_count*100) + (w_queen_count*900) + (w_rook_count*500) + (w_bishop_count*325) + (w_knight_count*300);
	if ((!w_king_count && color == WHITE) || (!b_king_count && color == BLACK)) {
		return -1;
	}

	return (color == BLACK)?black_material_balance:white_material_balance;
}
