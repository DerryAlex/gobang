#include "ui.h"
#include <ctype.h>
#include <stdio.h>

static const char *table_chars[9] = {"┼","┷","┯","┠","┗","┏","┨","┛","┓"};

static gobang_type map[COLUMNS][ROWS];

void UI_draw(gobang_type type, uint8_t column, uint8_t row)
{
	map[column][row] = type;
	for (int j = 0; j < ROWS; j++)
	{
		for (int i = 0; i < COLUMNS; i++)
		{
			if (i == column && j == row && type != NONE) printf("%s", (type == BLACK)?"▲":"△");
			else if (map[i][j] == BLACK) printf("●");
			else if (map[i][j] == GREY) printf("○");
			else printf("%s", table_chars[(j == ROWS - 1) * 1 + (j == 0) * 2 + (i == 0) * 3 + (i == COLUMNS - 1) * 6]);
		}
		printf(" %d\n", ROWS - j);
	}
	for (int i = 0; i < COLUMNS; i++) printf("%c", i + 'A');
	putchar('\n');
}

int UI_main(int argc, char *argv[])
{
	printf("input coordinate(e.g. h8) or command(q:ai, w:undo, p:pass, r:history)\n");
	
	char buf[256];
	int x, y;
	while (1)
	{
		scanf("%s", buf);
		if (toupper(buf[0]) == 'W') withdraw();
		else if (toupper(buf[0]) == 'Q') ai_turn();
		else if (toupper(buf[0]) == 'P') make_move(GOBANG_PASS, GOBANG_PASS);
		else if (toupper(buf[0]) == 'R') show_history();
		else
		{
			x = toupper(buf[0]) - 'A';
			sscanf(buf + 1, "%d", &y);
			y = ROWS - y;
			if (0 <= x && x < COLUMNS && 0 <= y && y < ROWS) make_move(x, y);
			else printf("illegal operation\n");
		}
	}
	return 0;
}

void UI_info(const char *str)
{
	printf("%s\n", str);
}
