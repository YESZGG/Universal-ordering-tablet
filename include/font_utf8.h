#ifndef FONT_UFT8_H_
#define FONT_UFT8_H_

#include "lcd.h"
extern "C"
{
#include "font.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
}

class font_utf8
{
public:
    font_utf8(LCD &lcd); // 注意每次定义 font 对象都会重新打开字库和映射！！

    ~font_utf8();

    // 设置字体大小
    void set_font(int pixels);

    void set_text(s32 x, s32 y, const char *text, color c, s32 maxWidth);

private:
    font *f;
    bitmap *bit; // 字体输出框
};
#endif