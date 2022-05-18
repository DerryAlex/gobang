#include "vcf.h"
#include "map.h"
#include "pattern.h"
#include "timer.h"
#include <stdlib.h>
#include <stdio.h>

#define A (*(int (*)[2])a)
#define B (*(int (*)[2])b)

static int
cmp0(const void *a, const void *b)
{
	return map_score[B[0]][B[1]][0] - map_score[A[0]][A[1]][0];
}

static int
cmp1(const void *a, const void *b)
{
	return map_score[B[0]][B[1]][1] - map_score[A[0]][A[1]][1];
}

#undef A
#undef B

static inline bool
is_consecutive4(int pos1[2], int pos2[2])
{
	int delta0 = abs(pos1[0] - pos2[0]), delta1 = abs(pos1[1] - pos2[1]);
	return (delta0 == 0 && delta1 <= 4) || (delta1 == 0 && delta0 <= 4) || (delta0 == delta1 && delta0 <= 4);
}

static inline bool
is_consecutive3(int pos1[2], int pos2[2])
{
	int delta0 = abs(pos1[0] - pos2[0]), delta1 = abs(pos1[1] - pos2[1]);
	return (delta0 <= 2 && delta1 <= 2) || (delta0 == 0 && delta1 == 3) || (delta1 == 0 && delta0 == 3) || (delta0 == 3 && delta1 == 3);
}

static inline bool
is_four(gobang_type player, uint8_t x, uint8_t y) /* 4-index */
{
	return pattern_type[map_bit[x][y][player == BLACK][0]][map_bit[x][y][player == GREY][0]][player == BLACK] >= FOUR_SLEEP || pattern_type[map_bit[x][y][player == BLACK][1]][map_bit[x][y][player == GREY][1]][player == BLACK] >= FOUR_SLEEP || pattern_type[map_bit[x][y][player == BLACK][2]][map_bit[x][y][player == GREY][2]][player == BLACK] >= FOUR_SLEEP || pattern_type[map_bit[x][y][player == BLACK][3]][map_bit[x][y][player == GREY][3]][player == BLACK] >= FOUR_SLEEP;
}

static inline bool
is_free_three(gobang_type player, uint8_t x, uint8_t y) /* 4-index */
{
	return pattern_type[map_bit[x][y][player == BLACK][0]][map_bit[x][y][player == GREY][0]][player == BLACK] >= THREE || pattern_type[map_bit[x][y][player == BLACK][1]][map_bit[x][y][player == GREY][1]][player == BLACK] >= THREE || pattern_type[map_bit[x][y][player == BLACK][2]][map_bit[x][y][player == GREY][2]][player == BLACK] >= THREE || pattern_type[map_bit[x][y][player == BLACK][3]][map_bit[x][y][player == GREY][3]][player == BLACK] >= THREE;
}

static int depth_limit, max_depth, last_depth;
static clock_t timer;
static bool extensible;
static int move[COLUMNS * ROWS][2];

static int
VCF(gobang_type player, int depth, int *optimal_x, int *optimal_y)
{
	if (is_timeout(timer))
	{
		extensible = true;
		return 0;
	}
	if (depth > max_depth) max_depth = depth;
	last_depth = depth;
	int priority[2][COLUMNS * ROWS][2];
	for (int i = 0; i < COLUMNS; i++)
		for (int j = 0; j < ROWS; j++)
		{
			priority[0][i * ROWS + j][0] = priority[1][i * ROWS + j][0] = i + 4;
			priority[0][i * ROWS + j][1] = priority[1][i * ROWS + j][1] = j + 4;
		}
	qsort(priority[0], COLUMNS * ROWS, sizeof (priority[0][0]), cmp0);
	qsort(priority[1], COLUMNS * ROWS, sizeof (priority[1][0]), cmp1);
	for (int i = 0; i < COLUMNS * ROWS; i++) /* attack 5 */
	{
		if (map[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]] != NONE) continue;
		if (map_score[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]][player == BLACK] >= VCF_WIN_SCORE)
		{
			move[depth][0] = priority[player == BLACK][i][0];
			move[depth][1] = priority[player == BLACK][i][1];
			if (depth == 0)
			{
				*optimal_x = priority[player == BLACK][i][0];
				*optimal_y = priority[player == BLACK][i][1];
			}
			return 1;
		}
	}
	for (int i = 0; i < COLUMNS * ROWS; i++) /* defend 5 */
	{
		if (map[priority[player == GREY][i][0]][priority[player == GREY][i][1]] != NONE) continue;
		if (map_score[priority[player == GREY][i][0]][priority[player == GREY][i][1]][player == GREY] >= VCF_WIN_SCORE)
		{
			move[depth][0] = priority[player == GREY][i][0];
			move[depth][1] = priority[player == GREY][i][1];
			if (map_score[priority[player == GREY][i][0]][priority[player == GREY][i][1]][player == BLACK] < 0)
			{
				if (depth == 0)
				{
					*optimal_x = priority[player == GREY][i][0];
					*optimal_y = priority[player == GREY][i][1];
				}
				return -1;
			}
			map_update(player, priority[player == GREY][i][0] - 4, priority[player == GREY][i][1] - 4);
			int ret = -VCF(GOBANG_TYPE_OPP(player), depth + 1, optimal_x, optimal_y);
			map_update(NONE, priority[player == GREY][i][0] - 4, priority[player == GREY][i][1] - 4);
			if (depth == 0)
			{
				*optimal_x = priority[player == GREY][i][0];
				*optimal_y = priority[player == GREY][i][1];
			}
			if (ret > 0) return 1;
			if (ret < 0) return -1;
			return 0;
		}
	}
	for (int i = 0; i < COLUMNS * ROWS; i++) /* attack 44, free4 */
	{
		if (map[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]] != NONE) continue;
		if (map_score[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]][player == BLACK] >= 2 * VCF_OPT_SCORE)
		{
			move[depth][0] = priority[player == BLACK][i][0];
			move[depth][1] = priority[player == BLACK][i][1];
			if (depth == 0)
			{
				*optimal_x = priority[player == BLACK][i][0];
				*optimal_y = priority[player == BLACK][i][1];
			}
			return 1;
		}
	}
	if (depth >= depth_limit)
	{
		extensible = true;
		return 0;
	}
	for (int i = 0; i < COLUMNS * ROWS; i++) /* attack 43 */
	{
		if (map[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]] != NONE) continue;
		if (map_score[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]][player == BLACK] >= VCF_OPT_SCORE)
		{
			move[depth][0] = priority[player == BLACK][i][0];
			move[depth][1] = priority[player == BLACK][i][1];
			map_update(player, priority[player == BLACK][i][0] - 4, priority[player == BLACK][i][1] - 4);
			int ret = -VCF(GOBANG_TYPE_OPP(player), depth + 1, optimal_x, optimal_y);
			map_update(NONE, priority[player == BLACK][i][0] - 4, priority[player == BLACK][i][1] - 4);
			if (ret > 0)
			{
				if (depth == 0)
				{
					*optimal_x = priority[player == BLACK][i][0];
					*optimal_y = priority[player == BLACK][i][1];
				}
				return 1;
			}
		}
	}
	for (int i = 0; i < COLUMNS * ROWS; i++) /* attack 4 */
	{
		if (map_score[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]][player == BLACK] < 0) break;
		if (map[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]] != NONE) continue;
		if (map_score[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]][player == BLACK] >= VCF_OPT_SCORE) continue;
		if (is_four(player, priority[player == BLACK][i][0], priority[player == BLACK][i][1]))
		{
			move[depth][0] = priority[player == BLACK][i][0];
			move[depth][1] = priority[player == BLACK][i][1];
			map_update(player, priority[player == BLACK][i][0] - 4, priority[player == BLACK][i][1] - 4);
			int ret = -VCF(GOBANG_TYPE_OPP(player), depth + 1, optimal_x, optimal_y);
			map_update(NONE, priority[player == BLACK][i][0] - 4, priority[player == BLACK][i][1] - 4);
			if (ret > 0)
			{
				if (depth == 0)
				{
					*optimal_x = priority[player == BLACK][i][0];
					*optimal_y = priority[player == BLACK][i][1];
				}
				return 1;
			}
		}
	}
	return 0;
}

static int
VCT(gobang_type player, int depth, int *optimal_x, int *optimal_y) /* with a high correct rate, not totally correct */
{
	if (is_timeout(timer))
	{
		extensible = true;
		return 0;
	}
	if (depth > max_depth) max_depth = depth;
	last_depth = depth;
	
	int priority[2][COLUMNS * ROWS][2];
	for (uint8_t i = 0; i < COLUMNS; i++)
		for (uint8_t j = 0; j < ROWS; j++)
		{
			priority[0][i * ROWS + j][0] = priority[1][i * ROWS + j][0] = i + 4;
			priority[0][i * ROWS + j][1] = priority[1][i * ROWS + j][1] = j + 4;
		}
	qsort(priority[0], COLUMNS * ROWS, sizeof (priority[0][0]), cmp0);
	qsort(priority[1], COLUMNS * ROWS, sizeof (priority[1][0]), cmp1);
	
	for (int i = 0; i < COLUMNS * ROWS; i++) /* attack 5 */
	{
		if (map[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]] != NONE) continue;
		if (map_score[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]][player == BLACK] >= VCF_WIN_SCORE)
		{
			move[depth][0] = priority[player == BLACK][i][0];
			move[depth][1] = priority[player == BLACK][i][1];
			if (depth == 0)
			{
				*optimal_x = priority[player == BLACK][i][0];
				*optimal_y = priority[player == BLACK][i][1];
			}
			return 1;
		}
	}
	
	for (int i = 0; i < COLUMNS * ROWS; i++) /* defend 5 */
	{
		if (map[priority[player == GREY][i][0]][priority[player == GREY][i][1]] != NONE) continue;
		if (map_score[priority[player == GREY][i][0]][priority[player == GREY][i][1]][player == GREY] >= VCF_WIN_SCORE)
		{
			move[depth][0] = priority[player == GREY][i][0];
			move[depth][1] = priority[player == GREY][i][1];
			if (depth == 0)
			{
				*optimal_x = priority[player == GREY][i][0];
				*optimal_y = priority[player == GREY][i][1];
			}
			if (map_score[priority[player == GREY][i][0]][priority[player == GREY][i][1]][player == BLACK] < 0) return -1;
			map_update(player, priority[player == GREY][i][0] - 4, priority[player == GREY][i][1] - 4);
			int ret = -VCT(GOBANG_TYPE_OPP(player), depth + 1, optimal_x, optimal_y);
			map_update(NONE, priority[player == GREY][i][0] - 4, priority[player == GREY][i][1] - 4);
			if (ret > 0) return 1;
			if (ret < 0) return -1;
			return 0;
		}
	}
	
	for (int i = 0; i < COLUMNS * ROWS; i++) /* attack 44, free4 */
	{
		if (map[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]] != NONE) continue;
		if (map_score[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]][player == BLACK] >= 2 * VCF_OPT_SCORE)
		{
			move[depth][0] = priority[player == BLACK][i][0];
			move[depth][1] = priority[player == BLACK][i][1];
			if (depth == 0)
			{
				*optimal_x = priority[player == BLACK][i][0];
				*optimal_y = priority[player == BLACK][i][1];
			}
			return 1;
		}
	}
	
	if (depth >= depth_limit)
	{
		extensible = true;
		return 0;
	}
	
	bool threat = false;
	bool alive = false;

	for (int i = 0; i < COLUMNS * ROWS; i++) /* attack 43 */
	{
		if (map[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]] != NONE) continue;
		if (map_score[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]][player == BLACK] >= VCF_OPT_SCORE)
		{
			move[depth][0] = priority[player == BLACK][i][0];
			move[depth][1] = priority[player == BLACK][i][1];
			map_update(player, priority[player == BLACK][i][0] - 4, priority[player == BLACK][i][1] - 4);
			int ret = -VCT(GOBANG_TYPE_OPP(player), depth + 1, optimal_x, optimal_y);
			map_update(NONE, priority[player == BLACK][i][0] - 4, priority[player == BLACK][i][1] - 4);
			if (ret > 0)
			{
				if (depth == 0)
				{
					*optimal_x = priority[player == BLACK][i][0];
					*optimal_y = priority[player == BLACK][i][1];
				}
				return 1;
			}
			if (ret == 0 && !alive)
			{
				if (depth == 0)
				{
					*optimal_x = priority[player == BLACK][i][0];
					*optimal_y = priority[player == BLACK][i][1];
				}
				alive = true;
			}
		}
	}
	
	for (int i = 0; i < COLUMNS * ROWS; i++) /* attack 4 */
	{
		if (map_score[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]][player == BLACK] < 0) break;
		if (map[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]] != NONE) continue;
		if (map_score[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]][player == BLACK] >= VCF_OPT_SCORE) continue;
		if (is_four(player, priority[player == BLACK][i][0], priority[player == BLACK][i][1]) && (depth < 2 || is_consecutive4(move[depth - 2], priority[player == BLACK][i]))) /* not go far, consecutive attack */
		{
			move[depth][0] = priority[player == BLACK][i][0];
			move[depth][1] = priority[player == BLACK][i][1];
			map_update(player, priority[player == BLACK][i][0] - 4, priority[player == BLACK][i][1] - 4);
			int ret = -VCT(GOBANG_TYPE_OPP(player), depth + 1, optimal_x, optimal_y);
			map_update(NONE, priority[player == BLACK][i][0] - 4, priority[player == BLACK][i][1] - 4);
			if (ret > 0)
			{
				if (depth == 0)
				{
					*optimal_x = priority[player == BLACK][i][0];
					*optimal_y = priority[player == BLACK][i][1];
				}
				return 1;
			}
			if (ret == 0 && !alive)
			{
				if (depth == 0)
				{
					*optimal_x = priority[player == BLACK][i][0];
					*optimal_y = priority[player == BLACK][i][1];
				}
				alive = true;
			}
		}
	}
	
	for (int i = 0; i < COLUMNS * ROWS; i++) /* defend 44, free4 */
	{
		if (map[priority[player == GREY][i][0]][priority[player == GREY][i][1]] != NONE) continue;
		if (map_score[priority[player == GREY][i][0]][priority[player == GREY][i][1]][player == GREY] >= 2 * VCF_OPT_SCORE)
		{
			threat = true;
			for (int off_x = -3; off_x <= 3; off_x++)
			{
				int tmp_x = priority[player == GREY][i][0] + off_x;
				if (tmp_x < 4 || tmp_x >= 4 + COLUMNS) continue;
				for (int off_y = - 3; off_y <= 3; off_y++)
				{
					int tmp_y = priority[player == GREY][i][1] + off_y;
					if (tmp_y < 4 || tmp_y >= 4 + ROWS) continue;
					if (off_x != 0 && off_y != 0 && off_x != off_y && off_x != - off_y) continue;
					if (map[tmp_x][tmp_y] != NONE) continue;
					if (!is_four(GOBANG_TYPE_OPP(player), tmp_x, tmp_y)) continue;
					if (map_score[tmp_x][tmp_y][player == BLACK] < 0) continue;
					move[depth][0] = tmp_x;
					move[depth][1] = tmp_y;		
					map_update(player, tmp_x - 4, tmp_y - 4);
					int ret = -VCT(GOBANG_TYPE_OPP(player), depth + 1, optimal_x, optimal_y);
					map_update(NONE, tmp_x - 4, tmp_y - 4);
					if (ret > 0)
					{
						if (depth == 0)
						{
							*optimal_x = tmp_x;
							*optimal_y = tmp_y;
						}
						return 1;
					}
					if (ret == 0 && !alive)
					{
						if (depth == 0)
						{
							*optimal_x = tmp_x;
							*optimal_y = tmp_y;
						}
						alive = true;
					}
				}
			}
			if (depth == 0 && *optimal_x == 0 && *optimal_y == 0)
			{
				*optimal_x = priority[player == GREY][i][0];
				*optimal_y = priority[player == GREY][i][1];
			}
		}
	}
	if (threat)
	{
		if (!alive) return -1;
		return 0;
	}
	else
	{
		if (depth == 0)
		{
			*optimal_x = 0;
			*optimal_y = 0;
		}
	}
	
	//for (int i = 0; i < COLUMNS * ROWS; i++) /* defend 4 */
  	//{
  		//if (map_score[priority[player == GREY][i][0]][priority[player == GREY][i][1]][player == GREY] < 0) break;
  		//if (map[priority[player == GREY][i][0]][priority[player == GREY][i][1]] != NONE) continue;
 		//if (map_score[priority[player == GREY][i][0]][priority[player == GREY][i][1]][player == GREY] >= 2 * VCF_OPT_SCORE) continue;
  		//if (is_four(GOBANG_TYPE_OPP(player), priority[player == GREY][i][0], priority[player == GREY][i][1])) return 0; /* cut (for speed and simplicity in logic) */
  	//}
	
	for (int i = 0; i < COLUMNS * ROWS; i++) /* attack 33 */
	{
		if (map[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]] != NONE) continue;
		if (map_score[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]][player == BLACK] >= VCF_OPT_SCORE / 2)
		{
			move[depth][0] = priority[player == BLACK][i][0];
			move[depth][1] = priority[player == BLACK][i][1];
			map_update(player, priority[player == BLACK][i][0] - 4, priority[player == BLACK][i][1] - 4);
			int ret = -VCT(GOBANG_TYPE_OPP(player), depth + 1, optimal_x, optimal_y);
			map_update(NONE, priority[player == BLACK][i][0] - 4, priority[player == BLACK][i][1] - 4);
			if (ret > 0)
			{
				if (depth == 0)
				{
					*optimal_x = priority[player == BLACK][i][0];
					*optimal_y = priority[player == BLACK][i][1];
				}
				return 1;
			}
		}
	}
	
	for (int i = 0; i < COLUMNS * ROWS; i++) /* attack free3 */
	{
		if (map_score[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]][player == BLACK] < 0) break;
		if (map[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]] != NONE) continue;
		if (map_score[priority[player == BLACK][i][0]][priority[player == BLACK][i][1]][player == BLACK] >= VCF_OPT_SCORE / 2 || is_four(player, priority[player == BLACK][i][0], priority[player == BLACK][i][1])) continue;
		if (is_free_three(player, priority[player == BLACK][i][0], priority[player == BLACK][i][1]) && (depth < 2 || is_consecutive3(move[depth - 2], priority[player == BLACK][i]))) /* not go far, consecutive attack */
		{
			move[depth][0] = priority[player == BLACK][i][0];
			move[depth][1] = priority[player == BLACK][i][1];
			map_update(player, priority[player == BLACK][i][0] - 4, priority[player == BLACK][i][1] - 4);
			int ret = -VCT(GOBANG_TYPE_OPP(player), depth + 1, optimal_x, optimal_y);
			map_update(NONE, priority[player == BLACK][i][0] - 4, priority[player == BLACK][i][1] - 4);
			if (ret > 0)
			{
				if (depth == 0)
				{
					*optimal_x = priority[player == BLACK][i][0];
					*optimal_y = priority[player == BLACK][i][1];
				}
				return 1;
			}
		}
	}

	return 0;
}

int
VCF_wrapper(gobang_type player, int *optimal_x, int *optimal_y)
{
	timer = setup_timer(VCF_TIME_LIMIT);
	for (depth_limit = VCF_DEPTH_LIMIT; !is_timeout(timer); depth_limit += 2)
	{
		int x = 0, y = 0, ret;
		max_depth = 0;
		extensible = false;
		ret = VCF(player, 0, &x, &y);
#ifndef GOBANG_DISABLE_VERBOSE_MESSAGE
		printf("VCF: %d - %d\n", depth_limit, max_depth);
#endif /* GOBANG_DISABLE_VERBOSE_MESSAGE */
		if ((x != 0) && (y != 0))
		{
			*optimal_x = x;
			*optimal_y = y;
#ifndef GOBANG_DISABLE_VERBOSE_MESSAGE
			if (last_depth > 0)
			{
				printf("Variation: ");
				for (int i = 0; i < last_depth; i++) printf("%c%d ", move[i][0] - 4 + 'A', ROWS - (move[i][1] - 4));
				printf("\n");
			}
#endif /* GOBANG_DISABLE_VERBOSE_MESSAGE */
			if (ret == 0) return -2;
			else return ret;
		}
		if (!extensible) break;
	}
#ifndef GOBANG_DISABLE_VERBOSE_MESSAGE
	if (extensible) printf("VCF timeout\n");
#endif /* GOBANG_DISABLE_VERBOSE_MESSAGE */
	
	return 0;
}

int
VCT_wrapper(gobang_type player, int *optimal_x, int *optimal_y)
{
	timer = setup_timer(VCT_TIME_LIMIT);
	for (depth_limit = VCT_DEPTH_LIMIT; !is_timeout(timer); depth_limit += 2)
	{
		int x = 0, y = 0, ret;
		max_depth = 0;
		extensible = false;
		ret = VCT(player, 0, &x, &y);
#ifndef GOBANG_DISABLE_VERBOSE_MESSAGE
		printf("VCT: %d - %d\n", depth_limit, max_depth);
#endif /* GOBANG_DISABLE_VERBOSE_MESSAGE */
		if ((x != 0) && (y != 0))
		{
			*optimal_x = x;
			*optimal_y = y;
#ifndef GOBANG_DISABLE_VERBOSE_MESSAGE
			if (last_depth > 0)
			{
				printf("Variation: ");
				for (int i = 0; i < last_depth; i++) printf("%c%d ", move[i][0] - 4 + 'A', ROWS - (move[i][1] - 4));
				printf("\n");
			}
#endif /* GOBANG_DISABLE_VERBOSE_MESSAGE */
			if (ret == 0) return 2;
			else return ret;
		}
		if (!extensible) break;
	}
#ifndef GOBANG_DISABLE_VERBOSE_MESSAGE
	if (extensible) printf("VCT timeout\n");
#endif /* GOBANG_DISABLE_VERBOSE_MESSAGE */
	
	return 0;
}
