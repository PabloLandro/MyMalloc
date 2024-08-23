#include <stdio.h>

void set_foreground(int color) {
    printf("\033[38;5;%dm", color);
}

void set_background(int color) {
    printf("\033[48;5;%dm", color);
}

void set_bold() {
    printf("\033[1m");
}

void set_underlined() {
    printf("\033[4m");
}

void reset_style() {
    printf("\033[0m");
}