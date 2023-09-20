#include "touch.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480
#define THRESHOLD 75 // 滑动阀值

using namespace std;
// 构造函数
Touch::Touch()
{
    // 1.打开触摸屏设备文件
    this->fd_ts = open("/dev/input/event0", O_RDWR);
    if (this->fd_ts < 0)
    {
        cout << "初始化触摸屏失败" << endl;
        exit(0); // 退出进程
    }
    else
    {
        cout << "初始化触摸屏成功！" << endl;
    }
}

// 析构函数
Touch::~Touch()
{
    close(this->fd_ts);
}

// 获取滑动方向 1 上 2 下 3 左 4 右
int Touch::getSlide(int &clickX, int &clickY)
{
    int touchX, touchY; // 存储按下去的x y 坐标
    int startX, startY;
    int endX, endY;
    bool clickEvent = false; // 标记是否有点击事件发生

    struct input_event ts;
    memset(&ts, 0, sizeof(struct input_event));

    while (1)
    {
        read(this->fd_ts, &ts, sizeof(struct input_event)); // 读取触摸屏信息

        if (ts.type == EV_ABS && ts.code == ABS_X)
        {
            touchX = ts.value * SCREEN_WIDTH / 1024;
        }
        else if (ts.type == EV_ABS && ts.code == ABS_Y)
        {
            touchY = ts.value * SCREEN_HEIGHT / 600;
        }
        else if (ts.type == EV_KEY && ts.code == BTN_TOUCH)
        {
            if (ts.value == 1)
            {
                startX = touchX;
                startY = touchY;
            }
            else if (ts.value == 0)
            {
                endX = touchX;
                endY = touchY;
                int diffX = abs(endX - startX);
                int diffY = abs(endY - startY);

                if (diffX < THRESHOLD && diffY < THRESHOLD)
                { // 点击事件
                    cout << "点击" << endl;
                    clickX = touchX;
                    clickY = touchY;
                    clickEvent = true;
                }
                else if (diffX > diffY)
                { // 左右滑动
                    if (endX < startX)
                    {
                        cout << "左滑动" << endl;
                        return 3; // 返回左滑动事件类型
                    }
                    else
                    {
                        cout << "右滑动" << endl;
                        return 4; // 返回右滑动事件类型
                    }
                }
                else
                { // 上下滑动
                    if (endY < startY)
                    {
                        cout << "上滑动" << endl;
                        return 1; // 返回上滑动事件类型
                    }
                    else
                    {
                        cout << "下滑动" << endl;
                        return 2; // 返回下滑动事件类型
                    }
                }
            }
        }

        if (clickEvent)
        {
            return 0; // 返回点击事件类型
        }
    }
}

// 获取触摸坐标值
void Touch::getCoordinate(int &x, int &y)
{
    // 定义一个存储触摸屏信息的结构体
    struct input_event ts;
    memset(&ts, 0, sizeof(struct input_event));

    while (1)
    {
        // 读取触摸屏信息
        read(this->fd_ts, &ts, sizeof(struct input_event));

        // 处理触摸事件
        if (ts.type == EV_ABS && ts.code == ABS_X)
        {
            x = ts.value * SCREEN_WIDTH / 1024;
        }
        else if (ts.type == EV_ABS && ts.code == ABS_Y)
        {
            y = ts.value * SCREEN_HEIGHT / 600;
        }
        else if (ts.type == EV_KEY && ts.code == BTN_TOUCH && ts.value == 0)
        {
            break; // 触摸结束时退出循环
        }
    }
}