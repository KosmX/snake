#include "debugmalloc.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<locale.h>
#include<ctype.h>
#include<time.h>

#include "structs.h"
#include "multiplatformLib.h"

//debugmalloc from infoc

#define green "\e[0;32m"
#define blue "\e[0;34m"

//There are lots of hints on https://infoc.eet.bme.hu/

//#define windows 1 with #ifndef __linux__ solved...

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
        #ifdef DEBUG
        memset(next, 0, sizeof(linkedString)); //fill it with zeros. only in debug mode
        #endif
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
            matrix->matrix[i][n].isFood = 0; //That is NOT food
            for(int a = 0; a < 2; a++){
                matrix->matrix[i][n].data.chars[a].bytes.c[0] = ' ';
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
                matrix->matrix[pos.x/2][pos.y].data.chars[pos.x%2].bytes.c[charPos] = current->value.bytes.c[charPos];
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
    //pos.x -= scrDat->pos.x;
    //pos.y -= scrDat->pos.y;
    if(pos.x < 0 || pos.y < 0 || pos.x >= scrDat->size.x || pos.y >= scrDat->size.y){
        return; //if pos is not on the screen, just do nothing.
    }
    printf("\e[%d;%dH", pos.y+1, pos.x *2+1);
    for(int i = 0; i < 2; i++){
        printChar(ch.data.chars[i]);
    }
    #ifdef DEBUG
    printf("\e[0;0H\n"); //to update terminal after EVERY print but drasticlally slowing it down
    #endif
}

/**Print characters to a map pos
 * 
 * It will check if the character is on the screen
 * 
 */
void print(chunk ch, Pos pos, screenData *scrDat, int width, int height){
    pos.x = pos.x - scrDat->pos.x;
    if(pos.x < 0){
        if(scrDat->isXRepeat){
            pos.x += width;
        }
        else{
            return;
        }
    }
    pos.y = pos.y - scrDat->pos.y;
    if(pos.y < 0){
        if(scrDat->isYRepeat){
            pos.y += height;
        }
        else{
            return;
        }
    }
    printChunk(ch, pos, scrDat);
}

void printSnakeChunk(snakeChain *snake){
    //TODO
}

int updateScreenSize(Matrix *map, screenData *scrDat){
    struct Vec2i size = getWindowSize();
    size.x = size.x / 2;
    size.y = size.y;// / 2;//Why half?
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
void updateScreen(Matrix *map, screenData *scrDat, snakeChain *head, Direction d){
    int do_update;
    do_update = updateScreenSize(map, scrDat);

    if(do_update){
        if(scrDat->size.x < map->width){
            scrDat->pos.x = mod((head->pos.x + scrDat->size.x) - scrDat->size.x / 2 ,map->width, scrDat->repeatMap);
        }else{
            scrDat->pos.x = 0;
        }
        if(scrDat->size.y < map->height){
            scrDat->pos.y = mod((head->pos.y + scrDat->size.y) - scrDat->size.y / 2 ,map->height, scrDat->repeatMap);
        }else{
            scrDat->pos.y = 0;
        }
    }
    else{
        int min_x = scrDat->pos.x;
        int max_x = scrDat->pos.x + scrDat->size.x;
        int min_y = scrDat->pos.y;
        int max_y = scrDat->pos.y + scrDat->size.y;
        switch (d)
        {
        case LEFT:
            if(scrDat->size.x < map->width && mod(head->pos.x - min_x, map->width, 1) < 6){
                scrDat->pos.x = mod(min_x - 1, map->width, scrDat->isXRepeat);
                do_update = 1;
            }
            break;
        case RIGHT:
            if(scrDat->size.x < map->width && mod(max_x - head->pos.x, map->width, 1) < 6){
                scrDat->pos.x = mod(mod(max_x + 1, map->width, scrDat->isXRepeat) - scrDat->size.x, map->width, true);
                do_update = 1; //1 equal true :D
            }
            break;
        case UP:
            if(scrDat->size.y < map->height && mod(head->pos.y - min_y, map->height, 1) < 6){
                scrDat->pos.y = mod(min_y - 1, map->height, scrDat->isYRepeat);
                do_update = 1;
            }
            break;
        case DOWN:
            if(scrDat->size.y < map->height && mod(max_y - head->pos.y, map->height, 1) < 6){
                scrDat->pos.y = mod(mod(max_y + 1, map->height, scrDat->isYRepeat) - scrDat->size.y, map->height, true);
                do_update = 1; //1 equal true :D
            }
            break;
        default:
            //printf("Something went wrong. Direction value is %d.\n", d);
            break;
        }
    }

    if(do_update){
        Pos pos;
        for (pos.y = scrDat->pos.y; pos.y < scrDat->pos.y + scrDat->size.y; pos.y++){
            printf("\e[%d;0H\e[K", pos.y - scrDat->pos.y + 1); //Clear the screen line
            if(pos.y >= map->height && !scrDat->isYRepeat){
                break;
            }
            for (pos.x = scrDat->pos.x; pos.x < scrDat->pos.x + scrDat->size.x; pos.x++){
                if(pos.x < map->width || scrDat->isXRepeat){
                    print(map->matrix[pos.x%map->width][pos.y%map->height], pos, scrDat, map->width, map->height);
                }
            }
        }
    }
}

void getControl(screenData *scrDat, Direction *direction){
    int i;
    while(i = getNextChar(), i != EOF){
        Direction next = NONE;
        switch (i)
        {
        case 'a':
            next = LEFT;
            break;
        
        case 's':
            next = DOWN;
            break;
        
        case 'w':
            next = UP;
            break;
        
        case 'd':
            next = RIGHT;
            break;
        
        default:
            next = NONE;
            continue; //we don't have to registrate a non command...
        }

        if(*direction == NONE){
            *direction = next;
            continue;
        }

        if(next == *direction){
            scrDat->commands[0] = NONE;
            scrDat->commands[1] = NONE;
            continue;
        }

        // [a && b || c]  =  [c || a && b]  =  [(a && b) || c]
        if(((*direction == UP || *direction == DOWN) && (next == RIGHT || next == LEFT))
        || ((*direction == RIGHT || *direction == LEFT) && (next == UP || next == DOWN))){
            scrDat->commands[0] = next;
        }
        else
        {
            if(scrDat->commands[0] != NONE){
                scrDat->commands[1] = next;
            }
        }
    }
}

int isNotWall(chunk c){
    return c.isFood || (c.data.chars[0].bytes.c[0] == ' ' && c.data.chars[1].bytes.c[0] == ' ');
}

int isAir(chunk c){
    return !c.isFood && isNotWall(c);
}

void updateFood(Matrix *map, int *foodTick, int feedAmount, snakeChain *firstSnake, screenData *scrDat){
    if((*foodTick)++ > feedAmount){
        for(int i = 0; i < 128; i++){
            Pos pos;
            int isFree = 1;
            snakeChain *snake = firstSnake;
            pos.x = rand()%map->width;
            pos.y = rand()%map->height;

            if(isAir(map->matrix[pos.x][pos.y])){
                while(snake != NULL){
                    if(snake->pos.x == pos.x && snake->pos.y == pos.y){
                        isFree = false;
                        break;
                    }
                    snake = snake->next;
                }
                if(!isFree){
                    continue;
                }
                //pos is available
                {
                    *foodTick = 0;
                    chunk c;
                    map->matrix[pos.x][pos.y].isFood = 1;
                    map->matrix[pos.x][pos.y].data.FRand = rand();
                    c.data.chars[0].bytes.c[0] = 'X';
                    c.data.chars[0].bytes.c[0] = 'X';
                    printf(green);
                    print(c, pos, scrDat, map->width, map->height);
                    printf("\e[0m");
                    break;
                }
            }
        }
    }
}

//EOF fatal error, 1 game over
int updateSnake(Matrix *map, screenData *scrDat, Direction d, snakeChain *head, int canBite){
    Pos pos = head->pos;
    switch (d)
    {
    case UP:
        pos.y--;
        break;
    case DOWN:
        pos.y++;
        break;
    case LEFT:
        pos.x--;
        break;
    case RIGHT:
        pos.x++;
        break;
    default:
        return EOF;
        break;
    }
    if(pos.x < 0 || pos.x >= map->width){
        if(scrDat->repeatMap){
            pos.x = mod(pos.x, map->width, true);
        }
        else
        {
            return 1; //GAME OVER reached map end;
        }
    }
    if(pos.y < 0 || pos.y >= map->height){
        if(scrDat->repeatMap){
            pos.y = mod(pos.y, map->height, true);
        }
        else
        {
            return 1; //GAME OVER reached map end;
        }
    }
    
    if(isNotWall(map->matrix[pos.x][pos.y])){
        int isFood = map->matrix[pos.x][pos.y].isFood;
        head->pos = pos;
        snakeChain *snake = head;
        Pos tmp_pos1 = head->pos, tmp_pos2;
        while (snake->next != 0)
        {
            snake = snake->next;
            chunk c;
            if(snake != head && snake->pos.x == head->pos.x && snake->pos.y == head->pos.y){
                if(snake->next != NULL){
                    if(canBite){
                        snakeChain *current = snake, *tmp_snek = snake;
                        tmp_snek = tmp_snek->next;
                        current->next = NULL;
                        while (tmp_snek != NULL)
                        {
                            current = tmp_snek;
                            tmp_snek = tmp_snek->next;
                            free(current);
                        }
                    }
                    else
                    {
                        return 1; //GAME OVER snake hit itself;
                    }
                }else
                {
                    isFood = false;
                }
                
                
            }
            tmp_pos2 = snake->pos;
            snake->pos = tmp_pos1;
            tmp_pos1 = tmp_pos2;

            c.data.chars[0].bytes.c[0] = '>';
            c.data.chars[1].bytes.c[0] = '<';            
            if(snake->next == NULL && isFood){
                isFood = false;
                snake->next = malloc(sizeof(snake));
                if(snake->next == NULL){
                    return EOF;
                }
                snake->next->next = NULL;
                snake->pos.x = -1;
                snake->pos.y = -1;
            }
            if(snake->pos.x == -1){
                continue;   //don't render snake part with initial position
            }
            printf(blue);
            print(c, snake->pos, scrDat, map->width, map->height); //TODO direction snake
            printf("\e[0m");

        }
        if(tmp_pos2.x != -1){
            chunk c; 
            c.data.chars[0].bytes.c[0] = ' ';
            c.data.chars[1].bytes.c[0] = ' ';
            print(c, tmp_pos2, scrDat, map->width, map->height); //set this to air.
        }
        chunk c;
        c.data.chars[0].bytes.c[0] = '(';
        c.data.chars[1].bytes.c[0] = ')';
        printf(blue);
        print(c, head->pos, scrDat, map->width, map->height); //TODO direction snake
        printf("\e[0m");
    }
    else
    {
        return 1; //GAME OVER hit the wall;
    }
    return 0;
}

//------------config loader------------

int loadConfig(int *tickSpeed, int *repeatMap, int *feedAmount, int *canBite){
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
        else if(strncmp(name, "repeat_map", 10) == 0){
            fscanf(config, " %d", repeatMap);
        }
        else if(strncmp(name, "feed_amount", 12) == 0){
            fscanf(config, " %d", feedAmount);
        }
        else if(strncmp(name, "can_bite", 9) == 0){
            fscanf(config, " %d", canBite);
        }
        else{
            printf("Unknown keyword: %s", name);
        }
    }
    return 0;
}



//------------LOOP METHODs--------------

//Tick the game, step the snake, collect and place food.
//check new direction
//update display
//update food state
//step snake
int tick(Matrix *map, screenData *scrDat, snakeChain *snake, Direction *d, int feedAmount, int canBite){
    static int foodTick = 0;
    getControl(scrDat, d);
    if(scrDat->commands[0] != NONE){
        *d = scrDat->commands[0];
    }
    updateScreen(map, scrDat, snake, *d);
    if(*d == NONE){
        chunk c;
        c.data.chars[0].bytes.c[0] = '(';
        c.data.chars[1].bytes.c[0] = ')';
        printf(blue);
        print(c, snake->pos, scrDat, map->width, map->height);
        printf("\e[0m");
        return 0; // waiting for user input.
    }
    updateFood(map, &foodTick, feedAmount, snake, scrDat);
    updateSnake(map, scrDat, *d, snake, canBite);
    //printf("\e[0;0H\n"); //Update the terminal (mostly on Unix based systems)
    fflush(stdout);
    return 0;
}

int loop(Matrix *matrix, int tickspeed, int repeatMap, int feedAmount, int canBite){

    Direction d = NONE; //Init with none
    screenData scrDat;
    snakeChain snake, *chain;

    while(true){
        Pos p;
        p.x = rand()%matrix->width;
        p.y = rand()&matrix->height;
        if(isNotWall(matrix->matrix[p.x][p.y])){
            snake.pos = p;
            break;
        }
    }
    chain = &snake;
    for(int i = 1; i < 3; i++){
        Pos p;
        p.x = -1;
        p.y = -1;
        chain->next = malloc(sizeof(snakeChain));
        if(chain->next == NULL){
            return -1;
        }
        chain = chain->next;
        chain->pos = p;
        chain->next = NULL;
    }

    scrDat.size.x = 0;
    scrDat.size.y = 0;
    scrDat.repeatMap = repeatMap;
    scrDat.commands[0] = NONE;
    scrDat.commands[1] = NONE;
    while(42){
        if(tick(matrix, &scrDat, &snake, &d, feedAmount, canBite)){
            break;
        }
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
                printChar(map->matrix[x][y].data.chars[i]);
                //unisleep(1000);
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
                (unsigned)map->matrix[x][y].data.chars[i].bytes.c[0],
                (unsigned)map->matrix[x][y].data.chars[i].bytes.c[1],
                (unsigned)map->matrix[x][y].data.chars[i].bytes.c[2],
                (unsigned)map->matrix[x][y].data.chars[i].bytes.c[3]);
                printf("as character: \"");
                printChar(map->matrix[x][y].data.chars[i]);
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

    srand(time(0)); // bee a bit more random

    int tickspeed = 100, repeatMap = 0, feedAmount = 20, canBite = true; // if no config, default value

    //----load config----

    if(loadConfig(&tickspeed, &repeatMap, &feedAmount, &canBite)){
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

    //----start game----


    _testprint(&map);

    loop(&map, tickspeed, repeatMap, feedAmount, canBite);

    //test code
    //printf("map:\n");
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

/**
 * debugger main function
 * in debug mode, starts the core with correct params
 * in release mode, it will use user input files.
 */
int main(int argc, char const *argv[])
{
    int walltest; //warning: unused variable ‘walltest’ [-Wunused-variable]

    //2 + 3;  //... this does nothing...
    #ifdef DEBUG
    int ret;
    char const *array[] = {argv[0], "map1.txt"};
    ret = core(2, array);
    printf("\npress any key to continue");
    getchar();
    //return 0, ret; //Miért van ez a függvény tele szeméttel??? pl 0, smt...

    return ret; // így szép.
    #else
    return core(argc, argv);
    debugmalloc_dump();
    #endif
}