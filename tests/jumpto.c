#include<stdio.h>

int main(int argc, char const *argv[])
{
    int x, y;
    while(scanf(" %d%d", &x, &y) == 2){
        printf("\e[%d;%dH", y, x);
    }
    return 0;
}
