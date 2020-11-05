#include "multiplatformLib.h"

#include "structs.h"

#ifdef __linux__
#include<sys/ioctl.h>
#include<unistd.h>
#include<time.h>
#else
#include<Windows.h>
#endif




//This package is designed to do the basic operations like get char without waiting for enter or sleep
//and be able to compile on multiple OS like linux and windows.


struct Vec2i getWindowSize(){
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


