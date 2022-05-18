#ifndef _GOBANG_SEARCH_H
#define _GOBANG_SEARCH_H 1

#include "../gobang.h"

#define CAND_PER_DEPTH (8)
#define TERMINATE_VALUE (1234567890)
#define SEARCH_THRESHOLD (1000000)
#define SEARCH_WIN_SCORE (100000)
#define SEARCH_MIN_DEPTH (8)

extern void alphabeta_wrapper(gobang_type player, int *optimal_x, int *optimal_y);

#endif /* _GOBANG_SEARCH_H */
