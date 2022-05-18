#ifndef _GOBANG_VCF_H
#define _GOBANG_VCF_H 1

#include "../gobang.h"

#define VCF_DEPTH_LIMIT (20)
#define VCT_DEPTH_LIMIT (12)
#define VCF_TIME_LIMIT (1.0)
#define VCT_TIME_LIMIT (3.5)
#define VCF_WIN_SCORE (100000)
#define VCF_OPT_SCORE (20000)

extern int VCF_wrapper(gobang_type player, int *optimal_x, int *optimal_y);
extern int VCT_wrapper(gobang_type player, int *optimal_x, int *optimal_y);

#endif /* _GOBANG_VCF_H */
