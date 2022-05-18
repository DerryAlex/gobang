#include "tran_table.h"
#include <stdlib.h>
#include <time.h>

#ifdef GOBANG_ENABLE_TRANSITION_TABLE
struct table_item{
	int depth, value/*, best_move*/;
	gobang_hash_type item_type;
	uint64_t key;
}tran_table[TABLE_SIZE];

static uint64_t zobrist[COLUMNS][ROWS][2];
static uint64_t current_state;

static inline uint64_t
random64(void)
{
	return (rand() & 0xffff) | ((uint64_t)(rand() & 0xffff) << 16) | ((uint64_t)(rand() & 0xffff) << 32) | ((uint64_t)(rand() & 0xffff) << 48);
}

void
tran_table_init(void)
{
	srand(time(NULL));
	for (int i = 0; i < COLUMNS; i++)
		for (int j = 0; j < ROWS; j++)
			for (int k = 0; k < 2; k++) zobrist[i][j][k] = random64();
	current_state = 0;
}

void
tran_table_tran(gobang_type player, uint8_t x, uint8_t y)
{
	current_state ^= zobrist[x][y][player == BLACK];
}

bool
tran_table_query(int depth, int alpha, int beta, int *val)
{
	if (current_state == tran_table[current_state % TABLE_SIZE].key)
	{
		if (tran_table[current_state % TABLE_SIZE].value == 1000000 || tran_table[current_state % TABLE_SIZE].value == -1000000)
		{
			*val = tran_table[current_state % TABLE_SIZE].value;
			return true;
		}
		if (tran_table[current_state % TABLE_SIZE].depth >= depth)
		{
			if (tran_table[current_state % TABLE_SIZE].item_type == HASH_EXACT)
			{
				*val = tran_table[current_state % TABLE_SIZE].value;
				return true;
			}
			if (tran_table[current_state % TABLE_SIZE].item_type == HASH_ALPHA && tran_table[current_state % TABLE_SIZE].value <= alpha)
			{
				*val = alpha;
				return true;
			}
			if (tran_table[current_state % TABLE_SIZE].item_type == HASH_BETA && tran_table[current_state % TABLE_SIZE].value >= beta)
			{
				*val = beta;
				return true;
			}
		}
	}
	return false;
}

void
tran_table_update(int val, int depth, gobang_hash_type type)
{
	//if (depth & 1) return;
	tran_table[current_state % TABLE_SIZE].depth = depth;
	tran_table[current_state % TABLE_SIZE].value = val;
	tran_table[current_state % TABLE_SIZE].item_type = type;
	tran_table[current_state % TABLE_SIZE].key = current_state;
}
#endif /* GOBANG_ENABLE_TRANSITION_TABLE */
