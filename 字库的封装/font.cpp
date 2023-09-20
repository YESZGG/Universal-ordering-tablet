#include <iostream>

using namespace std;

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
    font_utf8() // 注意每次定义 font 对象都会重新打开字库和映射！！
    {
        f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");

        // 初始化字体输出框
        bit = new bitmap;

        bit->width = 800;
        bit->height = 480;
        bit->byteperpixel = 4;

        int fd = open("/dev/fb0", O_RDWR);

        bit->map = (unsigned char *)mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        close(fd);
    }
    ~font_utf8()
    {
        fontUnload(f);
    }
    // 设置字体大小
    void set_font(int pixels)
    {
        fontSetSize(f, pixels);
    }

    void set_text(s32 x, s32 y, const char *text, color c, s32 maxWidth)
    {
        fontPrint(f, bit, x, y, (char *)text, c, maxWidth); // 显示汉字到屏幕中
    }

private:
    font *f;
    bitmap *bit; // 字体输出框
};

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
