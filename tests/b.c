#include<stdio.h>
#include "b.h"

#include "a.h"

void test(int i){
    printf("%d", i);
}

int main(int argc, char const *argv[])
{
    addOne(2);
    return 0;
}
