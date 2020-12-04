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

/**
 * @deprecated
 */
void refreshScreen(void){
    #ifdef __linux__
    //TODO
    #endif
}

#ifdef __linux__
struct termios term_config;
#endif

//Init platform specific IO
/**
 * Init platform dependent stuff, mostly the custom oi control
 */
void initMultiplatform(void){
    #ifdef __linux__
    struct termios info;
    tcgetattr(0, &info); //get attr
    term_config = info; //store the original terminal config
    info.c_lflag &= ~ICANON; //turn off canonical mode
    info.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &info); //set attr
    setbuf(stdin, NULL); //???
    #endif
}

/**
 * Unload and undo platform dependent configs
 */
void endMultiplatformLib(void){
    #ifdef __linux__
    tcsetattr(0, TCSANOW, &term_config); //undo terminal config changes
    #endif
}
#ifdef __linux__
//linux equivalent conio _kbhit
int _kbhit(){
    int kbhits;
    ioctl(0, FIONREAD, &kbhits);
    return kbhits;
}
#endif

/**
 * get the next char form stdin without waiting
 * 
 * @return EOF if no character on stdiní the char instead
 */
int getNextChar(void){
    if(_kbhit()){
        #ifdef __linux__
        return getchar();
        #else
        return _getch();
        #endif
    } 
    else{
        return EOF;
    }
}

/** 
 * @return returns with the size of the terminal window
 */
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

/**
 * sleep
 * 
 * @param milisec time to sleep in milis
 */
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


/**
 * work only in k > -m and k < 2m where k is the input m is the modulo
 * 
 * always return r >= 0 && < m
 * @param i input
 * @param base modulus base
 * @param repeat is in repeat mode
 * @return modulo i
 */
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
