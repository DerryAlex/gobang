#include "map.h"
#include "pattern.h"
#include "tran_table.h"

gobang_type map[COLUMNS + 8][ROWS + 8];
int map_bit[COLUMNS + 8][ROWS + 8][2][4]; /* X-axis, Y-axis, DIAGONAL-axis, DIAGONAL2-axis */
int map_score[COLUMNS + 8][ROWS + 8][2];
int neighbor[COLUMNS + 8][ROWS + 8];

void
map_init(void)
{
	for (int i = 0; i < COLUMNS + 8; i++)
		for (int j = 0; j < ROWS + 8; j++)
		{
			if (4 <= i && i < COLUMNS + 4 && 4 <= j && j < ROWS + 4) continue;
			
			map[i][j] = OUT;
			
			for (int k = 1; k <= 4; k++)
			{
				if (i - k < 0) break;
				map_bit[i - k][j][0][0] |= 1 << (4 - k);
				map_bit[i - k][j][1][0] |= 1 << (4 - k);
			}
			for (int k = 1; k <= 4; k++)
			{
				if (i + k >= COLUMNS + 8) break;
				map_bit[i + k][j][0][0] |= 1 << (3 + k);
				map_bit[i + k][j][1][0] |= 1 << (3 + k);
			}
			
			for (int k = 1; k <= 4; k++)
			{
				if (j - k < 0) break;
				map_bit[i][j - k][0][1] |= 1 << (4 - k);
				map_bit[i][j - k][1][1] |= 1 << (4 - k);
			}
			for (int k = 1; k <= 4; k++)
			{
				if (j + k >= ROWS + 8) break;
				map_bit[i][j + k][0][1] |= 1 << (3 + k);
				map_bit[i][j + k][1][1] |= 1 << (3 + k);
			}
			
			for (int k = 1; k <= 4; k++)
			{
				if (i - k < 0 || j - k < 0) break;
				map_bit[i - k][j - k][0][2] |= 1 << (4 - k);
				map_bit[i - k][j - k][1][2] |= 1 << (4 - k);
			}
			for (int k = 1; k <= 4; k++)
			{
				if (i + k >= COLUMNS + 8 || j + k >= ROWS + 8) break;
				map_bit[i + k][j + k][0][2] |= 1 << (3 + k);
				map_bit[i + k][j + k][1][2] |= 1 << (3 + k);
			}
			
			for (int k = 1; k <= 4; k++)
			{
				if (i - k < 0 || j + k >= ROWS + 8) break;
				map_bit[i - k][j + k][0][3] |= 1 << (4 - k);
				map_bit[i - k][j + k][1][3] |= 1 << (4 - k);
			}
			for (int k = 1; k <= 4; k++)
			{
				if (i + k >= COLUMNS + 8 || j - k < 0) break;
				map_bit[i + k][j - k][0][3] |= 1 << (3 + k);
				map_bit[i + k][j - k][1][3] |= 1 << (3 + k);
			}
		}
}

static inline int
update_map_score(int which, int patterns[2][4])
{
	return pattern_score_advanced[pattern_type[patterns[which][0]][patterns[1 ^ which][0]][which]]
	                             [pattern_type[patterns[which][1]][patterns[1 ^ which][1]][which]]
	                             [pattern_type[patterns[which][2]][patterns[1 ^ which][2]][which]]
	                             [pattern_type[patterns[which][3]][patterns[1 ^ which][3]][which]]
	                             [which];
}

void
map_update(gobang_type type, uint8_t x, uint8_t y) /* 0-index */
{
	x += 4;
	y += 4;
	
	for (int k = 1; k <= 4; k++)
	{
		map_bit[x - k][y][0][0] &= ~((map[x][y] == GREY) << (4 - k));
		map_bit[x - k][y][1][0] &= ~((map[x][y] == BLACK) << (4 - k));
		map_bit[x - k][y][0][0] |= (type == GREY) << (4 - k);
		map_bit[x - k][y][1][0] |= (type == BLACK) << (4 - k);
		map_score[x - k][y][0] = update_map_score(0, map_bit[x - k][y]);
		map_score[x - k][y][1] = update_map_score(1, map_bit[x - k][y]);
	}
	for (int k = 1; k <= 4; k++)
	{
		map_bit[x + k][y][0][0] &= ~((map[x][y] == GREY) << (3 + k));
		map_bit[x + k][y][1][0] &= ~((map[x][y] == BLACK) << (3 + k));
		map_bit[x + k][y][0][0] |= (type == GREY) << (3 + k);
		map_bit[x + k][y][1][0] |= (type == BLACK) << (3 + k);
		map_score[x + k][y][0] = update_map_score(0, map_bit[x + k][y]);
		map_score[x + k][y][1] = update_map_score(1, map_bit[x + k][y]);
	}
	
	for (int k = 1; k <= 4; k++)
	{
		map_bit[x][y - k][0][1] &= ~((map[x][y] == GREY) << (4 - k));
		map_bit[x][y - k][1][1] &= ~((map[x][y] == BLACK) << (4 - k));
		map_bit[x][y - k][0][1] |= (type == GREY) << (4 - k);
		map_bit[x][y - k][1][1] |= (type == BLACK) << (4 - k);
		map_score[x][y - k][0] = update_map_score(0, map_bit[x][y - k]);
		map_score[x][y - k][1] = update_map_score(1, map_bit[x][y - k]);
	}
	for (int k = 1; k <= 4; k++)
	{
		map_bit[x][y + k][0][1] &= ~((map[x][y] == GREY) << (3 + k));
		map_bit[x][y + k][1][1] &= ~((map[x][y] == BLACK) << (3 + k));
		map_bit[x][y + k][0][1] |= (type == GREY) << (3 + k);
		map_bit[x][y + k][1][1] |= (type == BLACK) << (3 + k);
		map_score[x][y + k][0] = update_map_score(0, map_bit[x][y + k]);
		map_score[x][y + k][1] = update_map_score(1, map_bit[x][y + k]);
	}
	
	for (int k = 1; k <= 4; k++)
	{
		map_bit[x - k][y - k][0][2] &= ~((map[x][y] == GREY) << (4 - k));
		map_bit[x - k][y - k][1][2] &= ~((map[x][y] == BLACK) << (4 - k));
		map_bit[x - k][y - k][0][2] |= (type == GREY) << (4 - k);
		map_bit[x - k][y - k][1][2] |= (type == BLACK) << (4 - k);
		map_score[x - k][y - k][0] = update_map_score(0, map_bit[x - k][y - k]);
		map_score[x - k][y - k][1] = update_map_score(1, map_bit[x - k][y - k]);
	}
	for (int k = 1; k <= 4; k++)
	{
		map_bit[x + k][y + k][0][2] &= ~((map[x][y] == GREY) << (3 + k));
		map_bit[x + k][y + k][1][2] &= ~((map[x][y] == BLACK) << (3 + k));
		map_bit[x + k][y + k][0][2] |= (type == GREY) << (3 + k);
		map_bit[x + k][y + k][1][2] |= (type == BLACK) << (3 + k);
		map_score[x + k][y + k][0] = update_map_score(0, map_bit[x + k][y + k]);
		map_score[x + k][y + k][1] = update_map_score(1, map_bit[x + k][y + k]);
	}
	
	for (int k = 1; k <= 4; k++)
	{
		map_bit[x - k][y + k][0][3] &= ~((map[x][y] == GREY) << (4 - k));
		map_bit[x - k][y + k][1][3] &= ~((map[x][y] == BLACK) << (4 - k));
		map_bit[x - k][y + k][0][3] |= (type == GREY) << (4 - k);
		map_bit[x - k][y + k][1][3] |= (type == BLACK) << (4 - k);
		map_score[x - k][y + k][0] = update_map_score(0, map_bit[x - k][y + k]);
		map_score[x - k][y + k][1] = update_map_score(1, map_bit[x - k][y + k]);
	}
	for (int k = 1; k <= 4; k++)
	{
		map_bit[x + k][y - k][0][3] &= ~((map[x][y] == GREY) << (3 + k));
		map_bit[x + k][y - k][1][3] &= ~((map[x][y] == BLACK) << (3 + k));
		map_bit[x + k][y - k][0][3] |= (type == GREY) << (3 + k);
		map_bit[x + k][y - k][1][3] |= (type == BLACK) << (3 + k);
		map_score[x + k][y - k][0] = update_map_score(0, map_bit[x + k][y - k]);
		map_score[x + k][y - k][1] = update_map_score(1, map_bit[x + k][y - k]);
	}
	
#ifdef GOBANG_ENABLE_TRANSITION_TABLE
	if (map[x][y] != NONE) tran_table_tran(map[x][y], x - 4, y - 4);
	map[x][y] = type;
	if (map[x][y] != NONE) tran_table_tran(map[x][y], x - 4, y - 4);
#else
	map[x][y] = type;
#endif /* GOBANG_ENABLE_TRANSITION_TABLE */
	
	for (int k1 = -2; k1 <= 2; k1++)
		for (int k2 = -2; k2 <= 2; k2++) neighbor[x + k1][y + k2] += (type == NONE)?-1:1;
}
