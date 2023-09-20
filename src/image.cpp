#include <iostream>
#include <cstdio>
#include "image.h"

using namespace std;

// IFM 图片文件管理类的构造函数
IFM::IFM(const string &path)
{
    char cmd[1024];
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "find %s -name '*.bmp'", path.c_str());

    FILE *fp = popen(cmd, "r");
    if (fp == NULL)
    {
        cout << "查找文件失败" << endl;
        return;
    }

    char buf[1024];
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        char *ret = fgets(buf, 1024, fp);
        if (ret == NULL)
        {
            cout << "检索完毕" << endl;
            break;
        }
        buf[strcspn(buf, "\n")] = '\0'; // Remove newline character

        string imagePath(buf); // Convert char* to string
        image_path.push_back(imagePath);

        cout << "image_path: " << image_path.back() << "\t" << image_path.size() << endl;
    }

    pclose(fp);
}

const string &IFM::get_index(int n)
{
    static string emptyString;

    if (n < 0 || n >= image_path.size())
    {
        cout << "越界访问" << endl;
        return emptyString;
    }

    return image_path[n];
}

int IFM::get_count()
{
    return image_path.size();
}

const string &IFM::get_front_path()
{
    static string emptyString;

    if (image_path.empty())
    {
        return emptyString;
    }

    return image_path.front();
}