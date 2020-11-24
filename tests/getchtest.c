
#include<stdio.h>
#include<termios.h>
#include<unistd.h>

int main(int argc, char const *argv[])
{

    struct termios info;          // This is from stackoverflow
    tcgetattr(0, &info);          /* get current terminal attirbutes; 0 is the file descriptor for stdin */
    info.c_lflag &= ~ICANON;      /* disable canonical mode */
    info.c_cc[VMIN] = 0;          /* don't wait for keypress, return EOF instead */
    info.c_cc[VTIME] = 0;         /* no timeout */
    tcsetattr(0, TCSANOW, &info); /* set immediately */

    int c = 0;

    sleep(1);
    c = getchar();
    printf("\nint: %d\nchar: %c\n", c, c);

    sleep(5);
    c = 0;
    while(c != -1){
        c = getchar();
        printf("\nint: %d\nchar: %c\n", c, c);
    }
    return 0;
}
