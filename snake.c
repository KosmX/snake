#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<locale.h>
#include<ctype.h>

#include "structs.h"
#include "multiplatformLib.h"

//debugmalloc from infoc
#include "debugmalloc.h"

//There are lots of hints on https://infoc.eet.bme.hu/

//#define windows 1 with #ifndef __linux__ solved...
//typedef unsigned long long int pointer; //Not optimal and not required. sometimes even the worst idea...

//-----------methods--------------

int isUnicodeEncoding(int set){
    static int bl = 0;
    if(set){
        bl = 1;
    }
    return bl;
}

/**
 * Only the first byte is required
 */
int checkUnicharLen(char c){
    int i = 0;
    if(!isUnicodeEncoding(0)){
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

//fueoetoeoecsoeoe *next //what to NOT do

int readFile(FILE *file, Matrix *matrix){
    int c, len, maxLineLen = 0, lineCount = 1, lineLen = 0; //lineCount = (3,1) ??? why?... i was just bored
    struct Vec2i pos;
    pos.x = 0;
    pos.y = 0;
    linkedString *linkedStr = 0, *current = 0, *next;
    //linkedStr = malloc(sizeof(linkedString));
    while(c = fgetc(file), c != EOF){
        next = malloc(sizeof(linkedString));
        if(next == NULL){
            return EOF;
        }
        memset(next, 0, sizeof(linkedString)); //TODO remove after debugging
        next->next = 0;
        while (c == '\r')
        {
            c = fgetc(file);
            if(c == -1){
                break;
            }
        }
        next->value.bytes.c[0] = c;
        len = checkUnicharLen(c);
        if(isUnicodeEncoding(0) && len -1){
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

//Use what chunk,
//witch screen coordinates?
//just the screen data
void printChunk(chunk ch, Pos pos, screenData *scrDat){
    pos.x -= scrDat->pos.x;
    pos.y -= scrDat->pos.y;
    if(pos.x < 0 || pos.y < 0 || pos.x >= scrDat->pos.x || pos.y >= scrDat->pos.y){
        return; //if pos is not on the screen, just do nothing.
    }
    
}

int updateScreenSize(Matrix *map, screenData *scrDat){
    struct Vec2i size = getWindowSize();
    if(size.x == scrDat->size.x && size.y == scrDat->size.y){
        return 0; //no change happened
    }
    scrDat->size.x = size.x;
    scrDat->size.y = size.y;
    if(scrDat->repeatMap){
        scrDat->isXRepeat = size.x < map->width;
        scrDat->isYRepeat = size.y < map->height;
    }
    return 1;
}

//Update screen if required 
void  updateScreen(Matrix *map, screenData *scrDat, snakeChain *head){
    int do_update;
    do_update = updateScreenSize(map, scrDat);
    if(scrDat->isXRepeat){
        if(head->pos.x < scrDat->pos.x - 6){//TODO
            
        }
    }
    else{
        scrDat->pos.x = 0;
    }
}

//Tick the game, step the snake, collect and place food.
//check new direction
//update display
//uppdate food state
//step snake
void tick(Matrix *map, screenData *scrDat, snakeChain *snake, Direction *d){

}

//------------config loader------------

int loadConfig(int *tickSpeed, int *repeatMap){
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
        else if(strncmp(name, "repeatMap", 10) == 0){
            fscanf(config, " %d", repeatMap);
        }
        else{
            printf("Unknown keyword: %s", name);
        }
    }
    return 0;
}



//------------LOOP METHOD--------------

int loop(Matrix *matrix, int tickspeed){

    Direction d = DOWN;
    screenData scrDat;
    snakeChain snake;   //TODO init snake, screen data
    scrDat.size.x = 0;
    scrDat.size.y = 0;
    while(42){
        tick(matrix, &scrDat, &snake, &d);
        unisleep(tickspeed); //Special sleep to work both in windows and unix environment
    }
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
                printf("\nvalue: %hx, %hx, %hx, %hx\n",
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

    initMultiplatform(); // init stuff.

    int tickspeed = 100, repeatMap = 0; // if no config, default value

    //----load config----

    if(loadConfig(&tickspeed, &repeatMap)){
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
    //2 + 3;  //... this does nothing...
    int ret;
    char const *array[] = {argv[0], "map1.txt"};
    ret = core(2, array);
    printf("\npress any key to continue");
    getchar();
    //return 0, ret; //Miért van ez a függvény tele szeméttel??? pl 0, smt...

    return ret; // így szép.
}