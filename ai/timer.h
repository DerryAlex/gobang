#ifndef _GOBANG_TIMER_H
#define _GOBANG_TIMER_H 1

#include <time.h>
#include <stdbool.h>

extern clock_t ai_timer;

/* this function is expected to be expanded like macro */
static inline clock_t
setup_timer(double time)
{
	return clock() + time * CLOCKS_PER_SEC;
}

/* this function is expected to be expanded like macro */
static inline bool
is_timeout(clock_t time)
{
	return clock() >= time;
}

#endif /* _GOBANG_TIMER_H */
