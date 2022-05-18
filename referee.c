#include "referee.h"

static gobang_type map[COLUMNS][ROWS];

void
referee_init(void)
{
	/* do nothing */
}

void
referee_update(gobang_type type, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	map[column][row] = type;
}

static inline uint8_t
count_row(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t cnt = 0;
	for (uint8_t i = column; i < COLUMNS && map[i][row] == player; i--) cnt++;
	for (uint8_t i = column + 1; i < COLUMNS && map[i][row] == player; i++) cnt++;
	return cnt;
}

static inline uint8_t
count_column(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t cnt = 0;
	for (uint8_t i = row; i < ROWS && map[column][i] == player; i--) cnt++;
	for (uint8_t i = row + 1; i < ROWS && map[column][i] == player; i++) cnt++;
	return cnt;
}

static inline uint8_t
count_diagonal(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t cnt = 0;
	for (uint8_t i = column, j = row; i < COLUMNS && j < ROWS && map[i][j] == player; i--, j--) cnt++;
	for (uint8_t i = column + 1, j = row + 1; i < COLUMNS && j < ROWS && map[i][j] == player; i++, j++) cnt++;
	return cnt;
}

static inline uint8_t
count_diagonal2(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t cnt = 0;
	for (uint8_t i = column, j = row; i < COLUMNS && j < ROWS && map[i][j] == player; i--, j++) cnt++;
	for (uint8_t i = column + 1, j = row - 1; i < COLUMNS && j < ROWS && map[i][j] == player; i++, j--) cnt++;
	return cnt;
}

bool
check_win(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t cnt = count_row(player, column, row);
	if (cnt == 5 || (player == GREY && cnt > 5)) return true;
	cnt = count_column(player, column, row);
	if (cnt == 5 || (player == GREY && cnt > 5)) return true;
	cnt = count_diagonal(player, column, row);
	if (cnt == 5 || (player == GREY && cnt > 5)) return true;
	cnt = count_diagonal2(player, column, row);
	if (cnt == 5 || (player == GREY && cnt > 5)) return true;
	return false;
}

static inline uint8_t
count_4_row(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t cnt = 0, L0, L1;
	int8_t posx;
	bool flag = false;
	L0 = count_row(player, column, row);
	for (posx = column; posx >= 0 && map[posx][row] == player; posx--);
	if (posx >= 0 && map[posx][row] == NONE)
	{
		L1 = 1;
		if (posx > 0 && map[posx - 1][row] == player) L1 += count_row(player, posx - 1, row);
		if (L0 + L1 == 5)
		{
			if (L1 == 1) flag = true;
			cnt++;
		}
	}
	posx = posx + L0 + 1;
	if (posx < COLUMNS && map[posx][row] == NONE)
	{
		L1 = 1;
		if (posx < COLUMNS - 1 && map[posx + 1][row] == player) L1 += count_row(player, posx + 1, row);
		if (L0 + L1 == 5 && (L1 != 1 || !flag)) cnt++;
	}
	return cnt;
}

static inline uint8_t
count_4_column(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t cnt = 0, L0, L1;
	int8_t posy;
	bool flag = false;
	L0 = count_column(player, column, row);
	for (posy = row; posy >= 0 && map[column][posy] == player; posy--);
	if (posy >= 0 && map[column][posy] == NONE)
	{
		L1 = 1;
		if (posy > 0 && map[column][posy - 1] == player) L1 += count_column(player, column, posy - 1);
		if (L0 + L1 == 5)
		{
			if (L1 == 1) flag = true;
			cnt++;
		}
	}
	posy = posy + L0 + 1;
	if (posy < ROWS && map[column][posy] == NONE)
	{
		L1 = 1;
		if (posy < ROWS - 1 && map[column][posy + 1] == player) L1 += count_column(player, column, posy + 1);
		if (L0 + L1 == 5 && (L1 != 1 || !flag)) cnt++;
	}
	return cnt;
}

static inline uint8_t
count_4_diagonal(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t cnt = 0, L0, L1;
	int8_t posx, posy;
	bool flag = false;
	L0 = count_diagonal(player, column, row);
	for (posx = column, posy = row; posx >= 0 && posy >= 0 && map[posx][posy] == player;posx--,posy--);
	if (posx >= 0 && posy >= 0 && map[posx][posy] == NONE)
	{
		L1 = 1;
		if (posx > 0 && posy > 0 && map[posx - 1][posy - 1] == player) L1 += count_diagonal(player, posx - 1, posy - 1);
		if (L0 + L1 == 5)
		{
			if (L1 == 1) flag = 1;
			cnt++;
		}
	}
	posx = posx + L0 + 1;
	posy = posy + L0 + 1;
	if (posx < COLUMNS && posy < ROWS && map[posx][posy] == NONE)
	{
		L1 = 1;
		if (posx < COLUMNS - 1 && posy < ROWS - 1 && map[posx + 1][posy + 1] == player) L1 += count_diagonal(player, posx + 1, posy + 1);
		if (L0 + L1 == 5 && (L1 != 1 || !flag)) cnt++;
	}
	return cnt;
}

static inline uint8_t
count_4_diagonal2(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t cnt = 0, L0, L1;
	int8_t posx, posy;
	bool flag = false;
	L0 = count_diagonal2(player, column, row);
	for (posx = column, posy = row; posx >= 0 && posy < ROWS && map[posx][posy] == player; posx--, posy++);
	if (posx >= 0 && posy < ROWS && map[posx][posy] == NONE)
	{
		L1 = 1;
		if (posx > 0 && posy < ROWS - 1 && map[posx - 1][posy + 1] == player) L1 += count_diagonal2(player, posx - 1, posy + 1);
		if (L0 + L1 == 5)
		{
			if (L1 == 1) flag = 1;
			cnt++;
		}
	}
	posx = posx + L0 + 1;
	posy = posy - L0 - 1;
	if (posx < COLUMNS && posy >= 0 && map[posx][posy] == NONE)
	{
		L1 = 1;
		if (posx < COLUMNS - 1 && posy > 0 && map[posx + 1][posy - 1] == player) L1 += count_diagonal2(player, posx + 1, posy - 1);
		if (L0 + L1 == 5 && (L1 != 1 || !flag)) cnt++;
	}
	return cnt;
}

static inline uint8_t
count_4(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	return count_4_row(player, column, row) + count_4_column(player, column, row) + count_4_diagonal(player, column, row) + count_4_diagonal2(player, column, row);
}

static inline bool
check_free4_row(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t L0;
	int8_t posx;
	L0 = count_row(player, column, row);
	for (posx = column; posx >= 0 && map[posx][row] == player; posx--);
	if (L0 == 4 && posx >= 0 && map[posx][row] == NONE && posx + L0 + 1 < COLUMNS && map[posx + L0 + 1][row] == NONE && (posx == 0 || map[posx - 1][row] != player) && (posx + L0 + 1 == COLUMNS - 1 || map[posx + L0 + 1 + 1][row] != player)) return true;
	return false;
}

static inline bool
check_free4_column(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t L0;
	int8_t posy;
	L0 = count_column(player, column, row);
	for (posy = row; posy >= 0 && map[column][posy] == player; posy--);
	if (L0 == 4 && posy >= 0 && map[column][posy] == NONE && posy + L0 + 1 < ROWS && map[column][posy + L0 + 1] == NONE && (posy == 0 || map[column][posy - 1] != player) && (posy + L0 + 1 == ROWS - 1 || map[column][posy + L0 + 1 + 1] != player)) return true;
	return false;
}

static inline bool
check_free4_diagonal(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t L0;
	int8_t posx, posy;
	L0 = count_diagonal(player, column, row);
	for (posx = column, posy = row; posx >= 0 && posy >= 0 && map[posx][posy] == player; posx--, posy--);
	if (L0 == 4 && posx >= 0 && posy >= 0 && map[posx][posy] == NONE && posx + L0 + 1 < COLUMNS && posy + L0 + 1 < ROWS && map[posx + L0 + 1][posy + L0 + 1] == NONE && (posx == 0 || posy == 0 || map[posx - 1][posy - 1] != player) && (posx + L0 + 1 == COLUMNS - 1 || posy + L0 + 1 == ROWS - 1 || map[posx + L0 + 1 + 1][posy + L0 + 1 + 1] != player)) return true;
	return false;
}

static inline bool
check_free4_diagonal2(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t L0;
	int8_t posx, posy;
	L0 = count_diagonal2(player, column, row);
	for (posx = column, posy = row; posx >= 0 && posy < ROWS && map[posx][posy] == player; posx--, posy++);
	if (L0 == 4 && posx >= 0 && posy < ROWS && map[posx][posy] == NONE && posx + L0 + 1 < COLUMNS && posy - L0 - 1 >= 0 && map[posx + L0 + 1][posy - L0 - 1] == NONE && (posx == 0 || posy == ROWS - 1 || map[posx - 1][posy + 1] != player) && (posx + L0 + 1 == COLUMNS - 1 || posy - L0 - 1 == 0 || map[posx + L0 + 1 + 1][posy - L0 - 1 - 1] != player)) return true;
	return false;
}

static inline bool
check_free4(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	return check_free4_row(player, column, row) || check_free4_column(player, column, row) || check_free4_diagonal(player, column, row) || check_free4_diagonal2(player, column, row);
}

static inline uint8_t
count_free3_row(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t cnt = 0, L0, L1;
	int8_t posx;
	bool flag = false;
	L0 = count_row(player, column, row);
	for (posx = column; posx >= 0 && map[posx][row] == player; posx--);
	if (posx >= 0 && map[posx][row] == 0)
	{
		L1 = 1;
		if (posx > 0 && map[posx - 1][row] == player) L1 += count_row(player, posx - 1, row);
		if (L0 + L1 == 4)
		{
			map[posx][row] = player;
			if (!check_win(player, posx, row) && check_legal(player, posx, row) && check_free4(player, posx, row))
			{
				if (L1 == 1) flag = true;
				cnt++;
			}
			map[posx][row] = NONE;
		}
	}
	posx = posx + L0 + 1;
	if (posx < COLUMNS && map[posx][row] == NONE)
	{
		L1 = 1;
		if (posx < COLUMNS - 1 && map[posx + 1][row] == player) L1 += count_row(player, posx + 1, row);
		if (L0 + L1 == 4 && (L1 != 1 || !flag))
		{
			map[posx][row] = player;
			if (!check_win(player, posx, row) && check_legal(player, posx, row) && check_free4(player, posx, row)) cnt++;
			map[posx][row] = NONE;
		}
	}
	return cnt;
}

static inline uint8_t
count_free3_column(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t cnt = 0, L0, L1;
	int8_t posy;
	bool flag = false;
	L0 = count_column(player, column, row);
	for (posy = row; posy >= 0 && map[column][posy] == player; posy--);
	if (posy >= 0 && map[column][posy] == NONE)
	{
		L1 = 1;
		if (posy > 0 && map[column][posy - 1] == player) L1 += count_column(player, column, posy - 1);
		if (L0 + L1 == 4)
		{
			map[column][posy] = player;
			if (!check_win(player, column, posy) && check_legal(player, column, posy) && check_free4(player, column, posy))
			{
				if (L1 == 1) flag = true;
				cnt++;
			}
			map[column][posy] = NONE;
		}
	}
	posy = posy + L0 + 1;
	if (posy < ROWS && map[column][posy] == NONE)
	{
		L1 = 1;
		if (posy < ROWS - 1 && map[column][posy + 1] == player) L1 += count_column(player, column, posy + 1);
		if (L0 + L1 == 4 && (L1 != 1 || !flag))
		{
			map[column][posy] = player;
			if (!check_win(player, column, posy) && check_legal(player, column, posy) && check_free4(player, column, posy)) cnt++;
			map[column][posy] = NONE;
		}
	}
	return cnt;
}

static inline uint8_t
count_free3_diagnonal(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t cnt = 0, L0, L1;
	int8_t posx, posy;
	bool flag = false;
	L0 = count_diagonal(player, column, row);
	for (posx = column, posy = row; posx >= 0 && posy >= 0 && map[posx][posy] == player; posx--, posy--);
	if (posx >= 0 && posy >= 0 && map[posx][posy] == NONE)
	{
		L1 = 1;
		if (posx > 0 && posy > 0 && map[posx - 1][posy - 1] == player) L1 += count_diagonal(player, posx - 1, posy - 1);
		if (L0 + L1 == 4)
		{
			map[posx][posy] = player;
			if (!check_win(player, posx, posy) && check_legal(player, posx, posy) && check_free4(player, posx, posy))
			{
				if (L1 == 1) flag = true;
				cnt++;
			}
			map[posx][posy] = NONE;
		}
	}
	posx = posx + L0 + 1;
	posy = posy + L0 + 1;
	if (posx < COLUMNS && posy < ROWS && map[posx][posy] == NONE)
	{
		L1 = 1;
		if (posx < COLUMNS - 1 && posy < ROWS - 1 && map[posx + 1][posy + 1] == player) L1 += count_diagonal(player, posx + 1, posy + 1);
		if (L0 + L1 == 4 && (L1 != 1 || !flag))
		{
			map[posx][posy] = player;
			if (!check_win(player, posx, posy) && check_legal(player, posx,posy) && check_free4(player, posx, posy)) cnt++;
			map[posx][posy] = NONE;
		}
	}
	return cnt;
}

static inline uint8_t
count_free3_diagnonal2(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	uint8_t cnt = 0, L0, L1;
	int8_t posx, posy;
	bool flag = false;
	L0 = count_diagonal2(player, column, row);
	for (posx = column, posy = row; posx >= 0 && posy < ROWS && map[posx][posy] == player; posx--, posy++);
	if (posx >= 0 && posy < ROWS && map[posx][posy] == NONE)
	{
		L1 = 1;
		if (posx > 0 && posy < ROWS - 1 && map[posx - 1][posy + 1] == player) L1 += count_diagonal2(player, posx - 1, posy + 1);
		if (L0 + L1 == 4)
		{
			map[posx][posy] = player;
			if (!check_win(player, posx, posy) && check_legal(player, posx, posy) && check_free4(player, posx, posy))
			{
				if (L1 == 1) flag = true;
				cnt++;
			}
			map[posx][posy] = NONE;
		}
	}
	posx = posx + L0 + 1;
	posy = posy - L0 - 1;
	if (posx < COLUMNS && posy >= 0 && map[posx][posy] == NONE)
	{
		L1 = 1;
		if (posx < COLUMNS - 1 && posy > 0 && map[posx + 1][posy - 1] == player) L1 += count_diagonal2(player, posx + 1, posy - 1);
		if (L0 + L1 == 4 && (L1 != 1 || !flag))
		{
			map[posx][posy] = player;
			if (!check_win(player, posx, posy) && check_legal(player, posx, posy) && check_free4(player, posx, posy)) cnt++;
			map[posx][posy] = NONE;
		}
	}
	return cnt;
}

static inline uint8_t
count_free3(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	return count_free3_row(player, column, row) + count_free3_column(player, column, row) + count_free3_diagnonal(player, column, row) + count_free3_diagnonal2(player, column, row);
}

bool
check_legal(gobang_type player, uint8_t column, uint8_t row)
{
	assert(column < COLUMNS);
	assert(row < ROWS);
	if (player == GREY) return true;
	if (count_row(player, column, row) > 5 || count_column(player, column, row) > 5 || count_diagonal(player, column, row) > 5 || count_diagonal2(player, column, row) > 5 || count_4(player, column, row) >= 2 || count_free3(player, column, row) >= 2) return false;
	return true;
}
