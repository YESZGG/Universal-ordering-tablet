#include <iostream>
#include "font_utf8.h"

font_utf8::font_utf8(LCD &lcd) // 注意每次定义 font 对象都会重新打开字库和映射！！
{
    f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");

    // 初始化字体输出框
    bit = new bitmap;

    bit->width = 800;
    bit->height = 480;
    bit->byteperpixel = 4;

    bit->map = (unsigned char *)lcd.getAddr();
}

font_utf8::~font_utf8()
{
    fontUnload(f);
}

// 设置字体大小
void font_utf8::set_font(int pixels)
{
    fontSetSize(f, pixels);
}

void font_utf8::set_text(s32 x, s32 y, const char *text, color c, s32 maxWidth)
{
    fontPrint(f, bit, x, y, (char *)text, c, maxWidth); // 显示汉字到屏幕中
}

#if 0
int main()
{

    font_utf8 f;
    // 设置字体大小
    f.set_font(32);

    // 显示字体
    f.set_text(100, 100, "早上好", getColor(0, 0, 255, 0), 0);

    f.set_text(200, 200, "下午好", getColor(0, 255, 0, 0), 0);

    f.set_text(200, 200, "晚上好", getColor(0, 0, 0, 255), 0);

    pause();
}
#endif