#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//#define windows 1

//typedef unsigned long long int pointer; //Not optimal

int isUnicodeEncoding = 1;

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

/**
 * Only the first byte is required
 */
int checkUnicharLen(char c){
    int i = 0;
    if(!isUnicodeEncoding){
        return 1; //int windows-xyzw every char has 1 len;
    }
    if(!(c & 0x80)){
        return 1;
    }
    while(c & 0x80){
        i++;
        c = c << 1;
        if(i > 8){
            return EOF;
        }
    }
    return i;
}

void printChar(unichar c){
    int len;
    len = checkUnicharLen(c.bytes.c[0]);
    for(int i = 0; i < len; i++){
        printf("%c", c.bytes.c[i]);
    }
}

//fueoetoeoecsoeoe *next

int readFile(FILE *file, Matrix *matrix){
    int c, len, maxLineLen = 0, lineCount = (3,1), lineLen = 0;
    struct Vec2i pos;
    pos.x = 0;
    pos.y = 0;
    linkedString *linkedStr = 0, *current = 0, *next;
    //linkedStr = malloc(sizeof(linkedString));
    while(c = fgetc(file)){
        next = malloc(sizeof(linkedString));
        if(next == NULL){
            return EOF;
        }
        memset(next, 0, sizeof(linkedString)); //TODO remove after debugging
        next->next = 0;
        if(c == -1){
            break;
        }
        while (c == '\r')
        {
            c = fgetc(file);
            if(c == -1){
                break;
            }
        }
        next->value.bytes.c[0] = c;
        len = checkUnicharLen(c);
        if(isUnicodeEncoding && len -1){
            for (int i = 1; i < len; i++){
                c = fgetc(file);
                if(c == -1){
                    return EOF;
                }
                next->value.bytes.c[i] = c;
            }
        }
        else if(c == '\n'){       //checking newline (unichar can't be a newline char (\n))
            lineCount++;
            if(lineLen > maxLineLen){
                maxLineLen = lineLen;
            }
            lineLen = -1;
        }
        lineLen++;
        //next.value = chars;
        if(current != 0){
            current->next = next;
        }
        else
        {
            linkedStr = next;
            //current = next;
        }
        current = next;
    }
    maxLineLen = maxLineLen / 2 + maxLineLen % 2;
    matrix->height = lineCount;
    matrix->width = maxLineLen;
    matrix->matrix = calloc(maxLineLen, sizeof(chunk*));
    if(matrix->matrix == NULL){
        printf("failed to allocate memory for the matrix");
        return EOF-1;
    }
    for (int i = 0; i < maxLineLen; i++){
        matrix->matrix[i] = calloc(lineCount, sizeof(chunk));
        if(matrix->matrix[i] == NULL){
            printf("failed to allocate memory for the matrix");
            return EOF-1;
        }
        for(int n = 0; n < lineCount; n++){
            for(int a = 0; a < 2; a++){
                matrix->matrix[i][n].chars[a].bytes.c[0] = ' ';
            }
        }
    }

    current = linkedStr;
    while(current != 0){
        //tmp = current;
        //current = current->next;
        //free(tmp);
        if(current->value.bytes.c[0] == '\n'){
            pos.x = 0;
            pos.y++;
            //break;
        }
        else{
            for(int charPos = 0; charPos < 4; charPos++){
                matrix->matrix[pos.x/2][pos.y].chars[pos.x%2].bytes.c[charPos] = current->value.bytes.c[charPos];
            }
            pos.x++; 
        }
        next = current;
        current = current->next;
        free(next);
    }
    return 0;
}

void rmMatrix(Matrix *map){
    for(int i = 0; i < map->width; i++){
        free(map->matrix[i]);
    }
    free(map->matrix);
}


//------------TESTING METHODS-------------
void _testprint(Matrix *map){
    for (int y = 0; y < map->height; y++){
        for(int x = 0; x < map->width; x++){
            for(int i = 0; i < 2; i++){
                //printf("%c", map->matrix[x][y].chars->bytes.c[i * 4]); //WTF... I didn't indexed correctly, but accidentaly I've got the right value...
                printChar(map->matrix[x][y].chars[i]);
            }
            //printf("|");
        }
        printf("\n");
    }
}

void _print1char(Matrix *map){
    int x, y;
    while(scanf(" %d%d", &x, &y) == 2){
        if(x >= 0 && y >= 0 && x < map->width && y < map->height){
            for(int i = 0;i < 2; i++){
                printf("\nvalue: %d, %d, %d, %d\n", map->matrix[x][y].chars[i].bytes.c[0], map->matrix[x][y].chars[i].bytes.c[1], map->matrix[x][y].chars[i].bytes.c[2], map->matrix[x][y].chars[i].bytes.c[3]);
                printf("as character: \"");
                printChar(map->matrix[x][y].chars[i]);
                printf("\"\n");
            }
        }
    }
}


//-------------CORE METHOD----------------

int core(int argc, char const *argv[])
{
    FILE *f;
    Matrix map;
    if(argc == 1){
        printf("Usage: snake <map name> [<snake skin>]");
        return 0;
    }
    else{
        f = fopen(argv[1], "rb");
        if(f == NULL){
            printf("Map file not found: %s", argv[1]);
            return EOF;
        }
        readFile(f, &map);
    }

    //test code
    printf("map:\n");
    _testprint(&map);
    //_print1char(&map);
    /* code */
    //free stuff
    rmMatrix(&map);

    return 0;
}


/*
int main(int argc, char const *argv[])
{
    return core(argc, argv);
}
*/

int main(int argc, char const *argv[])
{
    2 + 3;
    int ret;
    char const *array[] = {argv[0], "snake/map1.txt"};
    ret = core(2, array);
    printf("\npress any key to continue");
    getchar();
    return 0, ret; //Miért van ez a függvény tele szeméttel??? pl 0, smt...
}