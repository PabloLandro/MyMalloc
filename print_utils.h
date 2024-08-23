#ifndef PRINT_UTILS_H

#define UTILS_H

#define PU_BLACK 30
#define PU_RED 31
#define PU_GREEN 32
#define PU_YELLOW 33
#define PU_BLUE 34
#define PU_MAGENTA 35
#define PU_CYAN 36
#define PU_WHITE 37

void set_foreground(int color);
void set_background(int color);
void set_bold();
void set_underlined();
void reset_style();

#endif