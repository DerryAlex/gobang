#ifndef _GOBANG_H
#define _GOBANG_H 1

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>

#define COLUMNS (15)
#define ROWS (15)

typedef enum {NONE, BLACK, GREY, OUT} gobang_type;
#define GOBANG_TYPE_OPP(x) ((x) ^ BLACK ^ GREY)

#define GOBANG_PASS (200)

extern void make_move(uint8_t column, uint8_t row);
extern void withdraw(void);
extern void ai_turn(void);
extern void show_history(void);

#endif /* _GOBANG_H */
