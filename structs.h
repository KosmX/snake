#ifndef SNAKE_STRUCTS
#define SNAKE_STRUCTS

/**
 * Enum, storing possible moving directions
 */
typedef enum Direction{
    NONE = -1,
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3,
}Direction;

/**
 * 2 dimensional integer vector
 * or position
 */
typedef struct Vec2i{
    int x;
    int y;
}Pos;

/**
 * Storing snake linked list.
 * The head can count as a sentry (but it is storing data but cannot be removed...)
 */
typedef struct snakeChain
{
    /**
     * snake part distance from head. Feature wasn't added
     */
    int num;
    /**
     * Snake part's pos
     */
    struct Vec2i pos;
    /**
     * direction. Feature wasn't added
     */
    Direction dir;
    /**
     * Why do I have to document this. It is obvious
     */
    struct snakeChain *next;

}snakeChain;

/**
 * Storing 4 bytes for 1 utf-8 encoded character
 * *utf-8 does not always 4 bytes long, can be shorter...
 * 
 * If not using utf-8 encoding, the 1st byte will be used
 * 
 * in utf-8, the first bit can tell is it longer?
 * this is why I used union
 */
typedef union unichar{
    int isUnicone : 1;
    struct{
        char c[4];
    }bytes;

}unichar;

/**
 * 1 1 by 2 chunk. Can store a boolean, is it a food chunk, or it's just a wall or air...
 * stores 2 unichar
 */
typedef struct chunk
{
    /**
     * is this a food?
     */
    int isFood :1;
    /**
     * If food, it can store a random value. Foods can have multiple texture. I need the random there
     * or it stores 2 unichars.
     */
    union
    {
        unichar chars[2];
        int FRand;
    }data;
    
    /*
    colorable feature was not completed...
    struct{
        int fg : 3; //3 bit color codes.
        int bg : 3; //red green blue black white and 3 other (idk these)
    }color;*/
}chunk;

/**
 * While loading the map file, I need to store EVERY characters whitout knowing it's size.
 */
typedef struct linkedString{
    unichar value;
    struct linkedString *next;
}linkedString;

/**
 * The snake's map is a matrix made of chunks.
 */
typedef struct chunkMatrix{
    /**
     * The 2d array itself
     */
    chunk **matrix;
    /**
     * I won't comment the height. It's the matrix width...
     */
    int width;
    /**
     * I've commented this. Why?
     */
    int height;
}Matrix;

/**
 * storing texture for foods
 * and storing it's length
 */
typedef struct foodText{
    int len;
    /**
     * a chunk array containing food textures
     */
    chunk *text;
}foodText;

/**Storing important information from the screen
 * and from some other things...
 * 
 */
typedef struct screenData{
    /**
     * The position of the camera
     */
    Pos pos;
    /**the terminal window's size in chunks
     */
    Pos size;
    /**
     * Does the map repeating
     */
    int repeatMap;
    /**
     * If the map does repeating, is it repeating on the screen X axis
     * If the window is bigger than the map, it won't be printed multiple times
     */
    int isXRepeat;
    /**
     * same as the isXRepeat just in the Y axis
     */
    int isYRepeat;
    /**
     * Snake can store 2 commands. easier to do a U-turn
     */
    Direction commands[2];
    /**Storing the food textures
     */
    foodText foodTexture;//not real texture, but characters
}screenData;

#endif