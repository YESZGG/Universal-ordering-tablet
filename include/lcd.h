#ifndef LCD_H
#define LCD_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
extern "C"
{
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
}

using namespace std;

class LCD
{
private:
    int fd_lcd;
    unsigned int *addr;
    // const char lcdPath[20];
    string lcdPath;

public:
    LCD();
    ~LCD();
    // 颜色
    enum COLOR
    {
        RED = 0X00FF0000,
        GREEN = 0x0000FF00,
        BLUE = 0x000000FF,
        BLACK = 0x00000000,
        WHITE = 0x00FFFFFF
    };
    bool openLcd();
    void closeLcd();
    bool mapMemory();
    unsigned int *getAddr() const; // Getter for addr
    void unmapMemory();
    void displayColor(unsigned int color);
    friend class BMP;
};

#endif // LCD_H
