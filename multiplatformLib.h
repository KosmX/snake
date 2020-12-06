#ifndef SNAKE_MULTIPLATFORM
#define SNAKE_MULTIPLATFORM
//include guard
#include "structs.h"

struct Vec2i getWindowSize(void);

int initMultiplatform(int unicode);

void endMultiplatformLib(void);

void refreshScreen(void);

int getNextChar(void);

void unisleep(int milisec);

int mod(int i, int base, int repeat);

#endif