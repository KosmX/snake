#ifndef SNAKE_STRUCTS
#define SNAKE_STRUCTS

typedef enum Direcion{
    UP,
    RIGHT,
    DOWN,
    LEFT,
}Direction;

typedef struct Vec2i{
    int x;
    int y;
}Pos;

typedef struct snakeChain
{
    int num;
    struct Vec2i pos;
    struct snakeChain *next;

}snakeChain;

typedef union unichar{
    int isUnicone : 1;
    struct{
        char c[4];
    }bytes;

}unichar;
typedef struct chunk  //struct stores 2 chars and it's color :D
{
    unichar chars[2];
    /*
    struct{
        int fg : 3; //3 bit color codes.
        int bg : 3; //red green blue black white and 3 other (idk these)
    }color;*/
}chunk;

typedef struct state{
    struct Vec2i displaySize;
    struct Vec2i displayPos;
    int commands[2];
}globalState;

typedef struct linkedString{
    unichar value;
    struct linkedString *next;
}linkedString;

typedef struct chunkMatrix{
    chunk **matrix;
    int width;
    int height;
}Matrix;

typedef struct food{
    Pos pos;
    int rand;
    struct food *next;
}food;

typedef struct screenData{
    Pos pos;
    Pos size;
    int repeatMap;
    int isXRepeat;
    int isYRepeat;
}screenData;

#endif