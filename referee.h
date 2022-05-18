#ifndef _GOBANG_REFEREE_H
#define _GOBANG_REFEREE_H 1

#include "gobang.h"

extern void referee_init(void);
extern void referee_update(gobang_type type, uint8_t column, uint8_t row);
extern bool check_win(gobang_type player, uint8_t column, uint8_t row);
extern bool check_legal(gobang_type player, uint8_t column, uint8_t row);

#endif /* _GOBANG_REFEREE_H */
