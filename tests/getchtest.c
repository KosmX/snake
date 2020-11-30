
#include<stdio.h>
#include<termios.h>
#include<unistd.h>
#include<sys/select.h>
#include<sys/ioctl.h>

int _kbhit(){
    int kbhits;
    ioctl(0, FIONREAD, &kbhits);
    return kbhits;
}

int main(int argc, char const *argv[])
{

    //struct termios info;          // This is from stackoverflow
    //tcgetattr(0, &info);          /* get current terminal attirbutes; 0 is the file descriptor for stdin */
    //info.c_lflag &= ~ICANON;      /* disable canonical mode */
    //info.c_cc[VMIN] = 1;          /* don't wait for keypress, return EOF instead */
    //info.c_cc[VTIME] = 1;         /* no timeout */
    //info.c_cc[ECHO] = 0; //Turn off echo
    //tcsetattr(0, TCSANOW, &info); /* set immediately */

    //initscr();
    //noecho();
    //cbreak();
    //scrollok(stdscr, true);
    //nodelay(stdscr, true);

    struct termios info;
    tcgetattr(0, &info); //get attr
    info.c_lflag &= ~ICANON; //turn off canonical mode
    info.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &info); //set attr
    setbuf(stdin, NULL); //???
    


    int c = 0;
    sleep(1);
    int l = _kbhit();
    c = getchar();
    printf("\nint: %d\nchar: %c\nkbhit: %d\n", c, c, l);

    sleep(5);
    c = 0;
    while(c != -1){
        c = getchar();
        printf("\nint: %d\nchar: %c\n", c, c);
    }
    return 0;
}
