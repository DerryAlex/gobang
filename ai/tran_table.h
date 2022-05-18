#ifndef _GOBANG_TT_H
#define _GOBANG_TT_H 1

#include "../gobang.h"

#define TABLE_SIZE (1 << 23)

typedef enum {HASH_EXACT, HASH_ALPHA, HASH_BETA} gobang_hash_type;

#ifdef GOBANG_ENABLE_TRANSITION_TABLE
extern void tran_table_init(void);
extern void tran_table_tran(gobang_type player, uint8_t x, uint8_t y);
extern bool tran_table_query(int depth, int alpha, int beta, int *val);
extern void tran_table_update(int val, int depth, gobang_hash_type type);
#endif /* GOBANG_ENABLE_TRANSITION_TABLE */

#endif /* _GOBANG_TT_H */
