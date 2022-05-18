#include "ai.h"
#include "pattern.h"
#include "map.h"
#include "vcf.h"
#include "search.h"
#include "timer.h"
#include "tran_table.h"
#include <stdlib.h>
#include <stdio.h>

void
ai_init(void)
{
	pattern_init();
	map_init();
#ifdef GOBANG_ENABLE_TRANSITION_TABLE
	tran_table_init();
#endif /* GOBANG_ENABLE_TRANSITION_TABLE */
}

void
ai_update(gobang_type type, uint8_t x, uint8_t y)
{
	map_update(type, x, y);
}

void
ai_play(double time, gobang_type player, uint8_t *x, uint8_t *y)
{
	ai_timer = setup_timer(time);
	int optimal_x = 0, optimal_y = 0, result;
	result = VCF_wrapper(player, &optimal_x, &optimal_y);
	if (result != 0)
	{
#ifndef GOBANG_DISABLE_VERBOSE_MESSAGE
		printf("VCF: %d\n", result);
#endif /* GOBANG_DISABLE_VERBOSE_MESSAGE */
		*x = optimal_x - 4;
		*y = optimal_y - 4;
		return;
	}
#ifndef GOBANG_DISABLE_VERBOSE_MESSAGE
	printf("VCF: 0\n");
#endif /* GOBANG_DISABLE_VERBOSE_MESSAGE */
	result = VCT_wrapper(player, &optimal_x, &optimal_y);
	if (result == 1)
	{
#ifndef GOBANG_DISABLE_VERBOSE_MESSAGE
		printf("VCT: %d\n", result);
#endif /* GOBANG_DISABLE_VERBOSE_MESSAGE */
		*x = optimal_x - 4;
		*y = optimal_y - 4;
		return;
	}
#ifndef GOBANG_DISABLE_VERBOSE_MESSAGE
	if (result != 0) printf("VCT suggestion (%d): %c%d\n", result, optimal_x - 4 + 'A', ROWS - (optimal_y - 4));
	else printf("VCT: 0\n");
#endif /* GOBANG_DISABLE_VERBOSE_MESSAGE */
	alphabeta_wrapper(player, &optimal_x, &optimal_y);
	if (optimal_x != 0 && optimal_y != 0)
	{
		*x = optimal_x - 4;
		*y = optimal_y - 4;
		return;
	}
	result = -1;
	for (int i = 4; i < COLUMNS + 4; i++)
		for (int j = 4; j < ROWS + 4; j++)
			if (map[i][j] == NONE && map_score[i][j][player == BLACK] > result)
			{
				result = map_score[i][j][player == BLACK];
				optimal_x = i;
				optimal_y = j;
			}
	if (optimal_x == 0 || optimal_y == 0)
	{
		*x = GOBANG_PASS;
		*y = GOBANG_PASS;
	}
	else
	{
		*x = optimal_x - 4;
		*y = optimal_y - 4;
	}
}

#ifndef GOBANG_DISABLE_VERBOSE_MESSAGE
static inline int
showevaluation(void)
{
	int ret = 0;
	for (int i = 4; i < 4 + COLUMNS; i++)
		for (int j = 4; j < 4 + ROWS; j++)
		{
			if (map[i][j] == BLACK && map_score[i][j][1] >= 100000) return 1000000;
			if (map[i][j] == GREY && map_score[i][j][0] >= 100000) return -1000000;
			if (map[i][j] != NONE) continue;
			if (map_score[i][j][1] > 0) ret += map_score[i][j][1];
			ret -= map_score[i][j][0];
		}
	return ret;
}

static inline void
showbit(uint8_t n1, uint8_t n2)
{
	for (uint8_t i = 1; i != 0; i <<= 1)
	{
		if (n1 & i) putchar('@');
		else if (n2 & i) putchar('*');
		else putchar(' ');
		if (i == 8) putchar('x');
	}
}

static inline void
showpattern(gobang_pattern_type ptype1, gobang_pattern_type ptype2)
{
	switch (ptype1)
	{
		case LONG: printf("6"); break;
		case FIVE: printf("5"); break;
		case FOUR: printf("4"); break;
		case FOUR_SLEEP: printf("4-"); break;
		case THREE: printf("3"); break;
		case THREE_SLEEP: printf("3-"); break;
		case TWO: printf("2"); break;
		case TWO_SLEEP: printf("2-"); break;
		default: printf("0");
	}
	putchar(',');
	switch (ptype2)
	{
		case LONG: printf("6"); break;
		case FIVE: printf("5"); break;
		case FOUR: printf("4"); break;
		case FOUR_SLEEP: printf("4-"); break;
		case THREE: printf("3"); break;
		case THREE_SLEEP: printf("3-"); break;
		case TWO: printf("2"); break;
		case TWO_SLEEP: printf("2-"); break;
		default: printf("0");
	}
}

void
ai_debug(uint8_t best_x, uint8_t best_y)
{
	best_x += 4;
	best_y += 4;
	
	showbit(map_bit[best_x][best_y][1][0], map_bit[best_x][best_y][0][0]);
	showpattern(pattern_type[map_bit[best_x][best_y][1][0]][map_bit[best_x][best_y][0][0]][1], pattern_type[map_bit[best_x][best_y][0][0]][map_bit[best_x][best_y][1][0]][0]);
	printf("\n");
	showbit(map_bit[best_x][best_y][1][1], map_bit[best_x][best_y][0][1]);
	showpattern(pattern_type[map_bit[best_x][best_y][1][1]][map_bit[best_x][best_y][0][1]][1], pattern_type[map_bit[best_x][best_y][0][1]][map_bit[best_x][best_y][1][1]][0]);
	printf("\n");
	showbit(map_bit[best_x][best_y][1][2], map_bit[best_x][best_y][0][2]);
	showpattern(pattern_type[map_bit[best_x][best_y][1][2]][map_bit[best_x][best_y][0][2]][1], pattern_type[map_bit[best_x][best_y][0][2]][map_bit[best_x][best_y][1][2]][0]);
	printf("\n");
	showbit(map_bit[best_x][best_y][1][3], map_bit[best_x][best_y][0][3]);
	showpattern(pattern_type[map_bit[best_x][best_y][1][3]][map_bit[best_x][best_y][0][3]][1], pattern_type[map_bit[best_x][best_y][0][3]][map_bit[best_x][best_y][1][3]][0]);
	printf("\n");
	printf("(%d, %d) %d\n\n", map_score[best_x][best_y][1], map_score[best_x][best_y][0], showevaluation());
}
#endif /* GOBANG_DISABLE_VERBOSE_MESSAGE */
