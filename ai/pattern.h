#ifndef _GOBANG_PATTERN_H
#define _GOBANG_PATTERN_H 1

#include "../gobang.h"

typedef enum {OTHER, TWO_SLEEP, TWO, THREE_SLEEP, THREE, FOUR_SLEEP, FOUR, FIVE, LONG} gobang_pattern_type;

extern gobang_pattern_type pattern_type[256][256][2];
extern int pattern_score_advanced[9][9][9][9][2];

extern void pattern_init(void);

#endif /* _GOBANG_PATTERN_H */
