#ifndef SNAKE_MULTIPLATFORM
#define SNAKE_MULTIPLATFORM
//include guard
#include "structs.h"

struct Vec2i getWindowSize(void);

void initMultiplatform(void);

void unisleep(int milisec);

#endif