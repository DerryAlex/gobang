#ifndef _GOBANG_UI_H
#define _GOBANG_UI_H 1

#include "../gobang.h"

extern void UI_draw(gobang_type type, uint8_t column, uint8_t row);
extern int UI_main(int argc, char *argv[]);
extern void UI_info(const char *str);

#endif /* _GOBANG_UI_H */
