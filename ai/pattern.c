#include "pattern.h"

static const int pattern_score[9] = {0, 1, 4, 8, 32, 32, 256, 1024, 1024};

gobang_pattern_type pattern_type[256][256][2]; /* <L4,L3,L2,L1,R1,R2,R3,R4> */
int pattern_score_advanced[9][9][9][9][2];

static inline gobang_pattern_type
check_five(uint8_t me, uint8_t opp)
{
	uint8_t cnt = 0;
	for (uint8_t i = 8; i != 0; i >>= 1)
		if ((me & i) && !(opp & i)) cnt++;
		else break;
	for (uint8_t i = 16; i != 0; i <<= 1)
		if ((me & i) && !(opp & i)) cnt++;
		else break;
	if (cnt + 1 > 5) return LONG;
	else if (cnt + 1 == 5) return FIVE;
	else return 0;
}

static inline gobang_pattern_type
check_four(int type, uint8_t me, uint8_t opp)
{
	uint8_t cnt = 0;
	for (uint8_t i = 1; i != 0; i <<= 1)
		if (!(me & i) && !(opp & i))
		{
			if (pattern_type[me | i][opp][type] == FIVE) cnt++;
			if (pattern_type[me | i][opp][type] == LONG && type == 0) cnt++;
		}
	if (cnt >= 2) return FOUR;
	else if (cnt == 1) return FOUR_SLEEP;
	else return 0;
}

static inline bool
check_pattern(int type, uint8_t me, uint8_t opp, gobang_pattern_type ptype)
{
	for (uint8_t i = 1; i != 0; i <<= 1)
		if (!(me & i) && !(opp & i) && pattern_type[me | i][opp][type] == ptype) return true;
	return false;
}

static inline void
get_pattern_type(void)
{
	for (int i = 0; i < 256; i++)
		for (int j = 0; j < 256; j++)
			pattern_type[i][j][0] = pattern_type[i][j][1] = check_five(i, j);
	for (int i = 0; i < 256; i++)
		for (int j = 0; j < 256; j++)
			for (int p = 0; p < 2; p++)
				if (!pattern_type[i][j][p]) pattern_type[i][j][p] = check_four(p, i, j);
	for (int k = THREE; k >= TWO_SLEEP; k--)
		for (int i = 0; i < 256; i++)
			for (int j = 0; j < 256; j++)
				for (int p = 0; p < 2; p++)
					if (!pattern_type[i][j][p] && check_pattern(p, i, j, k + 2)) pattern_type[i][j][p] = k;
}

static inline void
get_pattern_score_advanced(void)
{
	int tmp_count[9] = {0};
	for (int i1 = OTHER; i1 <= LONG; i1++)
	{
		tmp_count[i1]++;
		for (int i2 = OTHER; i2 <= LONG; i2++)
		{
			tmp_count[i2]++;
			for (int i3 = OTHER; i3 <= LONG; i3++)
			{
				tmp_count[i3]++;
				for (int i4 = OTHER; i4 <= LONG; i4++)
				{
					tmp_count[i4]++;
					
					if (tmp_count[FIVE] >= 1 || tmp_count[LONG] >= 1) pattern_score_advanced[i1][i2][i3][i4][0] = pattern_score_advanced[i1][i2][i3][i4][1] = 100000; /* win in 1 step */
					else if (tmp_count[FOUR] >= 1 || tmp_count[FOUR_SLEEP] >= 2) pattern_score_advanced[i1][i2][i3][i4][0] = pattern_score_advanced[i1][i2][i3][i4][1] = 45000; /* win in 3 step, no more than half of 100000 */
					else if (tmp_count[FOUR_SLEEP] >= 1 && tmp_count[THREE] >= 1) pattern_score_advanced[i1][i2][i3][i4][0] = pattern_score_advanced[i1][i2][i3][i4][1] = 20000; /* win in 5 step(might lose), no more than half of 45000 */
					else if (tmp_count[THREE] >= 2) pattern_score_advanced[i1][i2][i3][i4][0] = pattern_score_advanced[i1][i2][i3][i4][1] = 10000; /* win in 5 step(might lose), no more than half of 20000 */
					else pattern_score_advanced[i1][i2][i3][i4][0] = pattern_score_advanced[i1][i2][i3][i4][1] = 0;
					
					if (tmp_count[FIVE] == 0 && (tmp_count[LONG] >= 1 || tmp_count[FOUR] + tmp_count[FOUR_SLEEP] >= 2 || tmp_count[THREE] >= 2)) pattern_score_advanced[i1][i2][i3][i4][1] = -110000; /* forbidden */
					
					pattern_score_advanced[i1][i2][i3][i4][0] += pattern_score[i1] + pattern_score[i2] + pattern_score[i3] + pattern_score[i4];
					pattern_score_advanced[i1][i2][i3][i4][1] += pattern_score[i1] + pattern_score[i2] + pattern_score[i3] + pattern_score[i4];
					
					tmp_count[i4]--;
				}
				tmp_count[i3]--;
			}
			tmp_count[i2]--;
		}
		tmp_count[i1]--;
	}
}

void
pattern_init(void)
{
	get_pattern_type();
	get_pattern_score_advanced();
}
