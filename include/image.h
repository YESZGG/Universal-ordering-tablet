#ifndef IMAGE_H_
#define IMAGE_H_

#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <string> // 添加string头文件

using namespace std;

class IFM
{
public:
    IFM(const string &path = "./");

    const string &get_index(int n);
    int get_count();
    const string &get_front_path();
private:
    vector<string> image_path;
};
#endif