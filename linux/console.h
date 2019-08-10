#pragma once

void clear(void);
int cputc(int c);
int cputs(const char *s);
void setcolor(int clr);
int getcolor(void);
void getcur(int *x, int *y);
void setcur(int x, int y);
