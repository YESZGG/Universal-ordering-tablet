#ifndef FOOD_H_
#define FOOD_H_

#include <iostream>
#include <map>
#include <list>
#include <cstring>
#include <string>
#include <vector>
#include <fstream> // ifstream 头文件
#include <sstream> // stringstram ss 头文件

using namespace std;

class Food
{
public:
    Food() {}
    Food(string name, int num, float price, int status, string image_path) : name(name), num(num), price(price), status(0), image_path(image_path) {}

    void init_food();                                              // 初始化食物
    void show_food() const;                                        // 显示食物信息
    string get_image_path() const;                                 // 获取食物图片路径
    string get_food_name() const;                                  // 获取食物名称
    float get_food_price() const;                                  // 获取食物价格
    int get_food_quantity() const;                                 // 获取食物数量
    string get_food_name(int index, const string &menuName) const; // 获取食物名称
    float get_food_price(int index, const string &menuName) const; // 获取食物价格
    int get_food_quantity(int index, const string &menuName) const;    // 获取食物数量
    vector<string> getFoodImagePaths(const string &menuName);          // 获取食物图片路径存储到vector容器中
    void readFoodInfo(const string &filename, const string &menuName); // 从文本中获取食物信息
    map<string, list<Food>> &getMenu();                                // 获取 Menu

private:
    string name;
    int num;
    float price;
    int status;
    string image_path;
    map<string, list<Food>> menu;
};

#endif
