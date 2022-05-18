#include "gobang.h"
#include "referee.h"
#include "ui/ui.h"
#include "ai/ai.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static gobang_type player = BLACK;
static gobang_type map[COLUMNS][ROWS];
static uint8_t record[COLUMNS * ROWS * 2][2], rcnt = 0;

void
make_move(uint8_t column, uint8_t row)
{
	if (column == GOBANG_PASS || row == GOBANG_PASS)
	{
		record[rcnt][0] = GOBANG_PASS;
		record[rcnt][1] = GOBANG_PASS;
		rcnt++;
		player = GOBANG_TYPE_OPP(player);
		return;
	}
	
	assert(column < COLUMNS);
	assert(row < ROWS);
	if (map[column][row] != NONE)
	{
		UI_info("illegal operation");
		return;
	}
	map[column][row] = player;
	record[rcnt][0] = column;
	record[rcnt][1] = row;
	rcnt++;
	UI_draw(player, column, row);
	referee_update(player, column, row);
	if (check_win(player, column, row))
	{
		if (player == BLACK) UI_info("BLACK wins");
		else UI_info("WHITE wins");
	}
	else if (!check_legal(player, column, row)) UI_info("FORBIDDEN");
	else if (rcnt == COLUMNS * ROWS) UI_info("DRAW");
	ai_update(player, column, row);
	player = GOBANG_TYPE_OPP(player);
#ifndef GOBANG_DISABLE_VERBOSE_MESSAGE
	ai_debug(column, row);
#endif /* GOBANG_DISABLE_VERBOSE_MESSAGE */
}

void
withdraw(void)
{
	if (rcnt > 0)
	{
		rcnt--;
		if (record[rcnt][0] == GOBANG_PASS || record[rcnt][1] == GOBANG_PASS)
		{
			player = GOBANG_TYPE_OPP(player);
			return;
		}
		map[record[rcnt][0]][record[rcnt][1]] = NONE;
		UI_draw(NONE, record[rcnt][0], record[rcnt][1]);
		referee_update(NONE, record[rcnt][0], record[rcnt][1]);
		ai_update(NONE, record[rcnt][0], record[rcnt][1]);
		player = GOBANG_TYPE_OPP(player);
	}
}

void
ai_turn(void)
{
	uint8_t x, y;
	if (rcnt == 0)
	{
		make_move(COLUMNS / 2, ROWS / 2);
		return;
	}
	clock_t timer = clock();
	ai_play(AI_TIME_LIMIT, player, &x, &y);
	printf("Time cost: %lf\n", (double)(clock() - timer) / CLOCKS_PER_SEC);
	if (x == GOBANG_PASS || y == GOBANG_PASS) printf("AI choose PASS\n");
	else printf("AI choose %c%d\n", x + 'A', ROWS - y);
	make_move(x, y);
}

void
show_history(void)
{
	for (int i = 0; i < rcnt; i++)
		if (record[i][0] == GOBANG_PASS || record[i][1] == GOBANG_PASS) printf("PASS ");
		else printf("%c%d ", record[i][0] + 'A', ROWS - record[i][1]);
	putchar('\n');
}

int
main(int argc, char *argv[])
{
	srand(time(NULL));
	referee_init();
	ai_init();
	int status = UI_main(argc, argv);
	return status;
}
