#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<locale.h>
#include<ctype.h>

#ifdef __linux__
#include<sys/ioctl.h>
#include<unistd.h>
#include<time.h>
#else
#include<Windows.h>
#endif

//#define windows 1

//typedef unsigned long long int pointer; //Not optimal

//int isUnicodeEncoding = 0;


int isUnicodeEncoding(int set){
    static int bl = 0;
    if(set){
        bl = 1;
    }
    return bl;
}

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
}screenData;

//-----------methods--------------


struct Vec2i getWindowSize(){
    struct Vec2i size;
    #ifdef __linux__
    struct winsize info;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &info);
    size.x = info.ws_col;
    size.y = info.ws_row;
    #else    
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    size.x = info.srWindow.Right - info.srWindow.Left + 1;
    size.y = info.srWindow.Bottom - info.srWindow.Top + 1;
    #endif
    return size;
}

void unisleep(int milisec){
    #ifdef __linux__
    struct timespec ts;
    ts.tv_sec = milisec / 1000;
    ts.tv_nsec = (milisec % 1000) * 1000000;
    nanosleep(&ts, NULL);
    #else
    Sleep(milisec);
    #endif
}

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
    int c, len, maxLineLen = 0, lineCount = (3,1), lineLen = 0; //lineCount = (3,1) ??? why?... i was just bored
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

//Use printChar
void printChunk(chunk ch, Pos pos, screenData *scrDat){
    pos.x -= scrDat->pos.x;
    pos.y -= scrDat->pos.y;
    if(pos.x < 0 || pos.y < 0 || pos.x >= scrDat->pos.x || pos.y >= scrDat->pos.y){
        return; //if pos is not on the screen, just do nothing.
    }
    
}


//------------config loader------------

int loadConfig(int *tickSpeed){
    FILE *config;
    config = fopen("config.cfg", "r");
    //int stillFile = 1; //boolean to show file is ended...
    if(config == NULL){
        return -1;
    }
    while(1){
        char name[32] = {0}, c; //it have to be enough;
        while(c = fgetc(config), c != -1 && isspace(c));
        if(c == -1){
            break;
        }
        //name[0] = c;
        for(int i = 0; i < 32 && !isspace(c) && c != '='; i++, c = fgetc(config)){
            name[i] = c;
        }
        //c = fgetc(config);
        while(c != '='){
            c = fgetc(config);
            if(c == -1){
                printf("I can't  understand the config file: %s", name);
                return EOF;
            }
        }
        if(strncmp(name, "use_utf8", 9) == 0){
            int bl;
            fscanf(config, " %d", &bl);
            isUnicodeEncoding(bl);
        }
        else if(strncmp(name, "tickspeed", 10) == 0){
            fscanf(config, " %d", tickSpeed);
        }
        else{
            printf("Unknown keyword: %s", name);
        }
    }
    return 0;
}



//------------LOOP METHOD--------------

int loop(Matrix *matrix, int tickspeed){

    unisleep(tickspeed); //Special sleep to work both in windows and unix environment
    return 0;
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
                printf("\nvalue: %hhx, %hhx, %hhx, %hhx\n",
                (unsigned)map->matrix[x][y].chars[i].bytes.c[0],
                (unsigned)map->matrix[x][y].chars[i].bytes.c[1],
                (unsigned)map->matrix[x][y].chars[i].bytes.c[2],
                (unsigned)map->matrix[x][y].chars[i].bytes.c[3]);
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

    int tickspeed = 100; // if no config, default value

    //----load config----

    if(loadConfig(&tickspeed)){
        printf("Error while loading config...");
    }

    //----init tasks----

    if(isUnicodeEncoding(0)){
        #ifndef __linux__
        setlocale(LC_ALL, ".utf-8");
        #endif
    }

    //----import map----

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
    _print1char(&map);
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
    2 + 3;  //... this does nothing...
    int ret;
    char const *array[] = {argv[0], "map1.txt"};
    ret = core(2, array);
    printf("\npress any key to continue");
    getchar();
    return 0, ret; //Miért van ez a függvény tele szeméttel??? pl 0, smt...
}