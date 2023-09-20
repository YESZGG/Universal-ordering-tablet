#ifndef TOUCH_H
#define TOUCH_H

#include <iostream>
#include <cstdlib>
#include <cstring>
extern "C"
{
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
}

class Touch
{
public:
    // 构造函数
    Touch();

    // 析构函数
    ~Touch();

    // 获取滑动方向 1 上 2 下 3 左 4 右
    int getSlide(int& clickX, int& clickY);
    // 获取触摸坐标值
    void getCoordinate(int &x, int &y);
private:
    int fd_ts;
};

#endif  // TOUCH_H
