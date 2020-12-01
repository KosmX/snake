#include "debugmalloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
int main(void) {
    char txt[] = "hello";
 
    char *p = malloc((strlen(txt) + 1) * sizeof(char));
    strcpy(p, txt);
    printf("TXT: %s\n", p);
    debugmalloc_dump();
    free(p);
 
    return 0;
}