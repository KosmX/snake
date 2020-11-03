#include<stdio.h>
#include<stdlib.h>

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

int main(int argc, char const *argv[])
{
    xp3();
    return 0;
}

