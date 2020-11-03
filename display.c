typedef struct block  //struct stores 2 chars and it's color:D
{
    int c1;
    int c2;
    struct{
        int fg : 3;
        int bg : 3;
    }color;
}block;