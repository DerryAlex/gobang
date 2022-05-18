#ifndef _GOBANG_AI_H
#define _GOBANG_AI_H 1

#include "../gobang.h"

#define AI_TIME_LIMIT (14.5)

extern void ai_init(void);
extern void ai_update(gobang_type type, uint8_t x, uint8_t y);
extern void ai_play(double time, gobang_type player, uint8_t *x, uint8_t *y);

#ifndef GOBANG_DISABLE_VERBOSE_MESSAGE
extern void ai_debug(uint8_t x, uint8_t y);
#endif /* GOBANG_DISABLE_VERBOSE_MESSAGE */

#endif /* _GOBANG_AI_H */
