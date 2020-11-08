#include "multiplatformLib.h"
#include<stdio.h>
#include "structs.h"

#ifdef __linux__
#include<sys/ioctl.h>
#include<unistd.h>
#include<time.h>
#include<termios.h>
#else
#include<Windows.h>
#include<conio.h> // read one char from stdio before pressing enter. essential for contol
#endif




//This package is designed to do the basic operations like get char without waiting for enter or sleep
//and be able to compile on multiple OS like linux and windows.

//Init some platform dependent configurations
void initMultiplatform(void){
    #ifdef __linux__
    struct termios info;          // This is from stackoverflow
    tcgetattr(0, &info);          /* get current terminal attirbutes; 0 is the file descriptor for stdin */
    info.c_lflag &= ~ICANON;      /* disable canonical mode */
    info.c_cc[VMIN] = 0;          /* don't wait for keypress, return EOF instead */
    info.c_cc[VTIME] = 0;         /* no timeout */
    tcsetattr(0, TCSANOW, &info); /* set immediately */
    #endif
}

//returns the char or EOF
int getNextChar(void){
    #ifdef __linux__
    return getchar();
    #else
    if(_kbhit()){
        return _getch();
    }
    else{
        return EOF;
    }
    #endif
}


struct Vec2i getWindowSize(void){
    struct Vec2i size;
    #ifdef __linux__
    struct winsize info;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &info);
    size.x = info.ws_col;
    size.y = info.ws_row;
    #else    
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    size.x = info.srWindow.Right - info.srWindow.Left + 1;
    size.y = info.srWindow.Bottom - info.srWindow.Top + 1;
    #endif
    return size;
}

void unisleep(int milisec){
    #ifdef __linux__
    struct timespec ts;
    ts.tv_sec = milisec / 1000;
    ts.tv_nsec = (milisec % 1000) * 1000000;
    nanosleep(&ts, NULL);
    #else
    Sleep(milisec);
    #endif
}


//NOT TRUE MOD FUNCTION
int mod(int i, int base, int repeat){
    if(i < 0){
        if(repeat){
           return i + base;
        }
        return 0;
    }
    else if(i >= base){
        if(repeat){
            return i - base;
        }
        return base - 1;
    }
    else{
        return i;
    }
}
