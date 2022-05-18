#ifndef _GOBANG_MAP_H
#define _GOBANG_MAP_H 1

#include "../gobang.h"

extern gobang_type map[COLUMNS + 8][ROWS + 8];
extern int map_score[COLUMNS + 8][ROWS + 8][2];
extern int neighbor[COLUMNS + 8][ROWS + 8];
extern int map_bit[COLUMNS + 8][ROWS + 8][2][4];

extern void map_init(void);
extern void map_update(gobang_type type, uint8_t x, uint8_t y); /* 0-index */

#endif /* _GOBANG_MAP_H */
