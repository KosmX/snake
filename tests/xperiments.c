#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>

#ifndef __linux__
#include<conio.h>
#endif

int xp1(void)
{
    unsigned long long int **map;
    int a, b, c;

    map = calloc(10, 8);
    for(int i = 0; i < 10; i++){
        map[i] = calloc(10, 8);
    }


    scanf("%d%d%d", &a, &b, &c);
    map[a][b] = c;
    
    scanf(" %d%d%d", &a, &b, &c);
    map[a][b] = c;
    
    scanf(" %d%d%d", &a, &b, &c);
    map[a][b] = c;
    
    for (int i = 0; i < 10; i++){
        for (int n = 0; n < 10; n++){
            printf(" %3d ", map[i][n]);
        }
        printf("\n");
    }

    return 0;
}

void xp2(void){
    char c;
    while(scanf("%c", &c) == 1){
        printf("%c", c);
    }
}

void xp3(){
    printf("%c", 0x00);
}

void xp4(){
    int i;
    char c;
    Sleep(5000);
    while(_kbhit()){
        c = getch();
        printf("%c", c);
    }
    
}

void xp5(){
    int x, y;
    scanf("%d%d", &x, &y);
    printf("\e[%d;%dHHello", x, y);
}

typedef struct{
    int x, y;
}Pos;

/*void xp6(){
    Pos a, b;
    a.x = 5;
    a.y = 2;
    b.x = 2+3;
    b.y = 1;
    b.y++;
    printf("%d\n", a == b);
}*/

int main(int argc, char const *argv[])
{
    12%5;
    xp5();
    return 0;
}

