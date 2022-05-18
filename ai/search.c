#include "search.h"
#include "map.h"
#include "timer.h"
#include "tran_table.h"
#include <stdio.h>
#include <stdlib.h>

static int depth_limit, width_limit;

static inline int
evaluation(int reward)
{
	int ret = 0;
	for (int i = 4; i < 4 + COLUMNS; i++)
		for (int j = 4; j < 4 + ROWS; j++)
		{
			if (map[i][j] == BLACK && map_score[i][j][1] >= SEARCH_WIN_SCORE) return SEARCH_THRESHOLD;
			if (map[i][j] == GREY && map_score[i][j][0] >= SEARCH_WIN_SCORE) return -SEARCH_THRESHOLD;
			if (map[i][j] != NONE) continue;
			if (map_score[i][j][1] > 0) ret += map_score[i][j][1];
			ret -= map_score[i][j][0];
		}
	return ret + reward;
}

static inline int
max(int a, int b)
{
	return (a > b)?a:b;
}

static inline int
min(int a, int b)
{
	return (a < b)?a:b;
}

#define A (*(int (*)[2])a)
#define B (*(int (*)[2])b)

static int
cmp(const void *a, const void *b)
{
	return (2 * max(map_score[B[0]][B[1]][0], map_score[B[0]][B[1]][1]) + max(min(map_score[B[0]][B[1]][0], map_score[B[0]][B[1]][1]), 0))   - (2 * max(map_score[A[0]][A[1]][0], map_score[A[0]][A[1]][1]) + max(min(map_score[A[0]][A[1]][0], map_score[A[0]][A[1]][1]), 0));
}

#undef A
#undef B

static int
alphabeta(gobang_type player, int depth, int alpha, int beta, int *optimal_x, int *optimal_y, int reward)
{
#ifndef GOBANG_ENABLE_REWARD
	reward = 0;
#endif /* GOBANG_ENABLE_REWARD */
#ifdef GOBANG_ENABLE_TRANSITION_TABLE
	int table_val;
	if (depth != 0 && tran_table_query(depth_limit - depth, alpha, beta, &table_val)) return table_val;
#endif /* GOBANG_ENABLE_TRANSITION_TABLE */
	if (depth >= depth_limit)
	{
#ifdef GOBANG_ENABLE_TRANSITION_TABLE
		table_val = evaluation(0);
		tran_table_update(table_val, 0, HASH_EXACT);
		return table_val; 
#else /* GOBANG_ENABLE_TRANSITION_TABLE */
		return evaluation(reward);
#endif /* GOBANG_ENABLE_TRANSITION_TABLE */
	}
	if (is_timeout(ai_timer)) return TERMINATE_VALUE;
	int best_x = 0, best_y = 0;
	int priority[COLUMNS * ROWS][2], cnt = 0;
	for (int i = 0; i < COLUMNS; i++)
		for (int j = 0; j < ROWS; j++)
		{
			priority[i * ROWS + j][0] = i + 4;
			priority[i * ROWS + j][1] = j + 4;
		}
	qsort(priority, COLUMNS * ROWS, sizeof (priority[0]), cmp);
	for (int i = 0; i < COLUMNS * ROWS && cnt < width_limit; i++)
	{
		if (map[priority[i][0]][priority[i][1]] != NONE) continue;
		if (neighbor[priority[i][0]][priority[i][1]] == 0) continue;
		if (map_score[priority[i][0]][priority[i][1]][player == BLACK] < 0) continue;
		if (best_x == 0 || best_y == 0)
		{
			best_x = priority[i][0];
			best_y = priority[i][1];
		}
		if (map_score[priority[i][0]][priority[i][1]][player == BLACK] >= SEARCH_WIN_SCORE)
		{
			if (depth == 0)
			{
				*optimal_x = priority[i][0];
				*optimal_y = priority[i][1];
			}
			if (player == BLACK)
			{
#ifdef GOBANG_ENABLE_TRANSITION_TABLE
				tran_table_update(SEARCH_THRESHOLD, depth_limit - depth, HASH_EXACT);
#endif /* GOBANG_ENABLE_TRANSITION_TABLE */
				return SEARCH_THRESHOLD;
			}
			else
			{
#ifdef GOBANG_ENABLE_TRANSITION_TABLE
				tran_table_update(-SEARCH_THRESHOLD, depth_limit - depth, HASH_EXACT);
#endif /* GOBANG_ENABLE_TRANSITION_TABLE */
				return -SEARCH_THRESHOLD;
			}
		}
		cnt++;
		map_update(player, priority[i][0] - 4, priority[i][1] - 4);
		int val = alphabeta(GOBANG_TYPE_OPP(player), depth + 1, alpha, beta, optimal_x, optimal_y, reward + ((player == BLACK)?map_score[priority[i][0]][priority[i][1]][1]:-map_score[priority[i][0]][priority[i][1]][0]));
		map_update(NONE, priority[i][0] - 4, priority[i][1] - 4);
		if (val == TERMINATE_VALUE) return TERMINATE_VALUE;
		if (player == BLACK)
		{
			if (val > alpha)
			{
				best_x = priority[i][0];
				best_y = priority[i][1];
				alpha = val;
			}
		}
		if (player == GREY)
		{
			if (val < beta)
			{
				best_x = priority[i][0];
				best_y = priority[i][1];
				beta = val;
			}
		}
		if (beta <= alpha)
		{
			if (depth == 0)
			{
				*optimal_x = best_x;
				*optimal_y = best_y;
			}
			if (player == BLACK)
			{
#ifdef GOBANG_ENABLE_TRANSITION_TABLE
				tran_table_update(beta, depth_limit - depth, HASH_BETA);
#endif /* GOBANG_ENABLE_TRANSITION_TABLE */
				return beta;
			}
			else
			{
#ifdef GOBANG_ENABLE_TRANSITION_TABLE
				tran_table_update(alpha, depth_limit - depth, HASH_ALPHA);
#endif /* GOBANG_ENABLE_TRANSITION_TABLE */
				return alpha;
			}
		}
	}
	if (depth == 0)
	{
		*optimal_x = best_x;
		*optimal_y = best_y;
	}
	if (player == BLACK)
	{
#ifdef GOBANG_ENABLE_TRANSITION_TABLE
		tran_table_update(alpha, depth_limit - depth, HASH_EXACT);
#endif /* GOBANG_ENABLE_TRANSITION_TABLE */
		return alpha;
	}
	else
	{
#ifdef GOBANG_ENABLE_TRANSITION_TABLE
		tran_table_update(beta, depth_limit - depth, HASH_EXACT);
#endif /* GOBANG_ENABLE_TRANSITION_TABLE */
		return beta;
	}
}

void
alphabeta_wrapper(gobang_type player, int *optimal_x, int *optimal_y)
{
	width_limit = CAND_PER_DEPTH;
	for (depth_limit = SEARCH_MIN_DEPTH; !is_timeout(ai_timer) && depth_limit <= COLUMNS * ROWS; depth_limit += 2)
	{
		int res, x = 0, y  = 0;
		res = alphabeta(player, 0, -SEARCH_THRESHOLD, SEARCH_THRESHOLD, &x, &y, 0);
		if (res != TERMINATE_VALUE)
		{
			*optimal_x = x;
			*optimal_y = y;
#ifndef GOBANG_DISABLE_VERBOSE_MESSAGE
			printf("depth=%d best=%c%d score:%d\n", depth_limit, x - 4 + 'A', ROWS - (y - 4), res);
#endif /* GOBANG_DISABLE_VERBOSE_MESSAGE */
		}
		if ((player == BLACK && res == SEARCH_THRESHOLD) || (player == GREY && res == -SEARCH_THRESHOLD)) return;
		if ((res == SEARCH_THRESHOLD || res == -SEARCH_THRESHOLD) && width_limit < COLUMNS * ROWS)
		{
			width_limit += 2;
			depth_limit -= 2;
		}
	}
}
