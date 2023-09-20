#include <iostream>
#include "lcd.h"
#include "touch.h"
#include "food.h"
#include "bmp.h"
#include "image.h"
#include "font_utf8.h"
#include <cmath>   // 添加头文件
#include <sstream> // 添加头文件
#include <iomanip> // 添加头文件
#include <algorithm>

using namespace std;

const int BUFFER_SIZE = 800 * 480;

struct FoodButtonInfo
{
    string foodName;  // 食物名字
    string foodType;  // 食物类型
    string price;     // 食物价格
    int current_page; // 当前页数
    int x;            // 按钮x坐标
    int y;            // 按钮y坐标
};

struct SelectFood // 已选的食物
{
    string foodType; // 食物类型
    string foodName; // 食物名字
    string price;    // 食物价格
    int s_page;      // 当前页数
    int num;         // 食物数目
};

class Menu
{
public:
    LCD lcd;
    Touch touch;
    IFM image;
    BMP bmp;
    Food food;


    void showstart();
    // 显示按钮
    void showButton(int buttonX, int buttonY);
    // 初始化食物图片和食物信息
    void init_menu_food(font_utf8 &font, const string menuname);                                                                     // 初始化菜单栏中的图片
    void showFoodInfo(font_utf8 &font, const string &foodName, const string &price, int x, int y);                                   // 用字体显示食物名称，价格
    void showFoodInfo(font_utf8 &font, const string &foodName, const string &price, const string &quantity, int x, int y);           // 用字体显示食物信息
    void showMenuFoodImages(font_utf8 &font, const string menuname, const vector<string> &imagePaths, int maxCount, int &x, int &y); // 显示每个菜单中的食物图片
    void showPayInfo(font_utf8 &font, int &x, int &y);
    void showSelectedFood();
    void calculateTotalPrice(font_utf8 &font); // 计算总价
    void handleTouchClick(font_utf8 &font, int x, int y, int startingIndex);
    void displayMenuInfo(font_utf8 &font, const string &foodType, const string &foodName,
                         const string &price, int quantity, int fontX, int fontY);
    void clearSelectFood(); // 清除已选食物
    void payBills(font_utf8 &font, int &x, int &y);
    friend bool compareSelectFood(const SelectFood &a, const SelectFood &b);

private:
    int currentPage = 0;
    float totalPrice = 0.0; // 食物总价格
    int startingIndex = 0;
    list<SelectFood> selectFood;            // 已选食物信息和数量
    vector<FoodButtonInfo> foodButtonInfos; // 定义vector容器 来存储结构体信息
};

void Menu::clearSelectFood()
{
    cout << "清除已选食物" << endl;
    selectFood.clear();
}

void Menu::showstart()
{
    bmp.showBMP("./image/start1.bmp",lcd.getAddr());
    usleep(50000);
    bmp.showBMP("./image/start2.bmp",lcd.getAddr());
    usleep(50000);
    bmp.showBMP("./image/start3.bmp",lcd.getAddr());
    sleep(1);
}

void Menu::showFoodInfo(font_utf8 &font, const string &foodName, const string &price, int x, int y)
{
    font.set_text(x, y + 20, foodName.c_str(), getColor(0, 0, 0, 0), 0);
    font.set_text(x, y + 55, ("￥: " + price).c_str(), getColor(0, 0, 0, 0), 0);
}

void Menu::showFoodInfo(font_utf8 &font, const string &foodName, const string &price, const string &quantity, int x, int y)
{
    font.set_text(x, y + 20, foodName.c_str(), getColor(0, 0, 0, 0), 0);
    font.set_text(x, y + 55, ("￥: " + price).c_str(), getColor(0, 0, 0, 0), 0);
    font.set_text(x, y + 90, quantity.c_str(), getColor(0, 0, 0, 0), 0);
}

void Menu::showButton(int buttonX, int buttonY)
{
    bmp.showBMP("./image/button.bmp", lcd.getAddr(), buttonX, buttonY + 135);
}

void Menu::showSelectedFood()
{
    int offsetY = 0;
    for (auto &sf : selectFood)
    {
        cout << sf.foodType << ": " << sf.foodName << "\t价格:" << sf.price << "\t数目: " << sf.num << "\t页数:" << sf.s_page << endl;
    }
}

// 显示已点单的食物的总价格
void Menu::calculateTotalPrice(font_utf8 &font)
{
    totalPrice = 0.0;

    for (const auto &sf : selectFood)
    {
        float price = stof(sf.price);
        totalPrice += sf.num * price;
    }

    totalPrice = round(totalPrice * 100) / 100; // 四舍五入保留两位小数

    ostringstream streamObj;                             // 创建输出流对象
    streamObj << fixed << setprecision(2) << totalPrice; // 将浮点数转换为字符串，并保留两位小数
    string totalPriceStr = "总共:￥ " + streamObj.str(); // 拼接字符串

    bmp.show_color(lcd, 0xFFFFFFFF, 155, 430, 300, 480);
    font.set_text(160, 440, totalPriceStr.c_str(), getColor(0, 0, 0, 0), 0); // 显示总价
    cout << "已选食物的总价格为：" << totalPrice << endl;
}

void Menu::init_menu_food(font_utf8 &font, const string menuname)
{
    bmp.show_color(lcd, 0xFFFFFFFF, 150, 45, 800, 420);
    int init_x = 170, init_y = 45;
    vector<string> imagePaths = food.getFoodImagePaths(menuname);
    int numImages = imagePaths.size();

    for (int i = 0; i < min(numImages, 4); i++) // 最多显示4张图片
    {
        string imagePath = imagePaths[i];

        int imageIndex = i % 4;
        int currentStartX = init_x + (imageIndex % 2) * 305;
        int currentStartY = init_y + (imageIndex / 2) * 175;
        bmp.showBMP(imagePath, lcd.getAddr(), currentStartX, currentStartY);

        // 获取食物信息并显示
        string foodName = food.get_food_name(i, menuname); // 使用当前索引获取食物名字
        char price_str[50];
        sprintf(price_str, "%.2f", food.get_food_price(i, menuname)); // 使用当前索引获取食物价格
        string price(price_str);

        // 在相对应的图片右边显示食物信息，位置根据图片的坐标计算
        int infoX = currentStartX + 190; // 食物信息的X坐标为图片的右侧
        int infoY = currentStartY;       // 食物信息的Y坐标与图片相同
        showFoodInfo(font, foodName, price, infoX, infoY);

        // 在相对应的图片右边显示点餐按钮，位置根据图片的坐标计算
        int buttonX = currentStartX + 180;
        int buttonY = currentStartY;
        showButton(buttonX, buttonY);

        // 存储每个食物对应的按钮位置信息和食物名字
        FoodButtonInfo info;
        info.foodName = foodName;
        info.foodType = menuname;
        info.current_page = 0;
        info.price = price;
        info.x = buttonX;
        info.y = buttonY + 135;
        // 检查'foodButtonInfos'中是否已经存在同名的食品
        bool found = false;
        for (const auto &existingInfo : foodButtonInfos)
        {
            if (existingInfo.foodName == info.foodName)
            {
                found = true;
                break;
            }
        }

        // 只有当foodButtonInfo不存在时才添加它
        if (!found)
        {
            foodButtonInfos.push_back(info);
        }
    }
    for (const auto &info : foodButtonInfos)
    {
        if (info.foodType != menuname)
            continue;
        if (info.current_page == 0)
        {
            bool find = false;
            for (const auto &sf : selectFood)
            {
                if (sf.foodType == info.foodType && sf.foodName == info.foodName && sf.s_page == 0)
                {
                    find = true;
                    bmp.show_color(lcd, 0xFFFFFFFF, info.x + 10, info.y - 48, info.x + 125, info.y);
                    font.set_text(info.x + 10, info.y - 45, ("数量: " + to_string(sf.num)).c_str(), getColor(0, 0, 0, 0), 0);
                    break;
                }
            }
            if (!find)
            {
                bmp.show_color(lcd, 0xFFFFFFFF, info.x, info.y - 48, info.x + 125, info.y);
                font.set_text(info.x + 10, info.y - 45, "数量: 0", getColor(0, 0, 0, 0), 0);
            }
        }
    }
}

void Menu::showMenuFoodImages(font_utf8 &font, const string menuname, const vector<string> &imagePaths, int maxCount, int &x, int &y)
{
    init_menu_food(font, menuname);

    int totalPages = (imagePaths.size() + maxCount - 1) / maxCount;
    int startX = 160, startY = 45;
    size_t currentPage = 0; // 当前页数

    while (true)
    {
        int slideDirection = touch.getSlide(x, y);
        if (slideDirection == 0) // 点击
        {
            if ((x < 150) || (y < 45 || y > 420))
            {
                return;
            }
            for (const auto &info : foodButtonInfos)
            {
                if (x > info.x + 70 && x < info.x + 125 && y > info.y && y < info.y + 50 && info.foodType == menuname && info.current_page == currentPage)
                {
                    bool foodExist = false;
                    for (auto &sf : selectFood)
                    {
                        if (sf.foodName == info.foodName && sf.foodType == info.foodType && sf.s_page == currentPage)
                        {
                            cout << info.x << "\t" << info.y << endl;
                            sf.num++;
                            foodExist = true;
                            bmp.show_color(lcd, 0xFFFFFFFF, info.x, info.y - 48, info.x + 125, info.y);
                            font.set_text(info.x + 8, info.y - 45, ("数量: " + to_string(sf.num)).c_str(), getColor(0, 0, 0, 0), 0);
                            break;
                        }
                    }
                    if (!foodExist)
                    {
                        SelectFood sf;
                        sf.foodType = info.foodType;
                        sf.foodName = info.foodName;
                        sf.price = info.price;
                        sf.s_page = info.current_page;
                        sf.num = 1;
                        selectFood.push_back(sf);
                        bmp.show_color(lcd, 0xFFFFFFFF, info.x, info.y - 48, info.x + 125, info.y);
                        font.set_text(info.x + 10, info.y - 45, ("数量: " + to_string(sf.num)).c_str(), getColor(0, 0, 0, 0), 0);
                    }
                }
                if (x > info.x && x < info.x + 50 && y > info.y && y < info.y + 50 && info.foodType == menuname && info.current_page == currentPage)
                {
                    bool foodExist = false;
                    vector<list<SelectFood>::iterator> itemsToRemove;

                    for (auto it = selectFood.begin(); it != selectFood.end(); ++it)
                    {
                        if (it->foodName == info.foodName && it->foodType == info.foodType && it->s_page == currentPage)
                        {
                            if (it->num > 0)
                            {
                                it->num--;
                                bmp.show_color(lcd, 0xFFFFFFFF, info.x, info.y - 48, info.x + 125, info.y);
                                font.set_text(info.x + 8, info.y - 45, ("数量: " + to_string(it->num)).c_str(), getColor(0, 0, 0, 0), 0);

                                if (it->num == 0)
                                {
                                    itemsToRemove.push_back(it);
                                }
                                foodExist = true;
                                break;
                            }
                        }
                    }

                    for (auto it : itemsToRemove)
                    {
                        selectFood.erase(it);
                    }

                    if (!foodExist)
                    {
                        // 如果所选食物不在selectFood列表中，则这是无效的操作
                        continue;
                    }
                }
            }
            showSelectedFood();

            calculateTotalPrice(font); // 计算已选食物的总价格

            continue;
        }
        if (slideDirection == 2 && currentPage > 0) // 向下滑动
        {
            currentPage--;
        }
        else if (slideDirection == 1 && currentPage < totalPages - 1) // 向上滑动
        {
            currentPage++;
        }
        else if (slideDirection == 3 || slideDirection == 4)
        {
            return;
        }

        // 刷新显示的食物页面
        bmp.show_color(lcd, 0xFFFFFFFF, 150, 45, 800, 420);
        cout << "当前页数为：" << currentPage << endl;

        size_t startIndex = currentPage * maxCount;
        size_t endIndex = min(startIndex + maxCount, imagePaths.size());

        for (size_t i = startIndex; i < endIndex && i < imagePaths.size(); i++)
        {
            string imagePath = imagePaths[i];
            int imageIndex = i % maxCount;
            int currentStartX = startX + (imageIndex % 2) * 315;
            int currentStartY = startY + (imageIndex / 2) * 175;

            bmp.showBMP(imagePath, lcd.getAddr(), currentStartX, currentStartY);

            // 使用字体库显示食品信息
            // 获取和显示食品信息
            string foodName = food.get_food_name(i, menuname);
            char price_str[50];
            sprintf(price_str, "%.2f", food.get_food_price(i, menuname));
            string price(price_str);

            // 根据图像的坐标，在图像的右侧显示食品信息
            int infoX = currentStartX + 190;
            int infoY = currentStartY;

            // 根据食物项目selectFood list中,用字体显示数量信息
            string quantityStr;
            for (const auto &sf : selectFood)
            {
                if (sf.foodName == foodName && sf.foodType == menuname && sf.s_page == currentPage)
                {
                    quantityStr = "数量: " + to_string(sf.num);
                    break;
                }
            }

            bmp.show_color(lcd, 0xFFFFFFFF, infoX, infoY + 90, infoX + 125, infoY + 135);
            showFoodInfo(font, foodName, price, quantityStr, infoX, infoY);

            // 根据食物项目是否存在于selectFood list中，显示带数量或不带数量的按钮
            bool displayQuantity = false;
            for (const auto &sf : selectFood)
            {
                if (sf.foodName == foodName && sf.foodType == menuname && sf.s_page == currentPage)
                {
                    displayQuantity = true;
                    break;
                }
            }
            int buttonX = currentStartX + 180;
            int buttonY = currentStartY;
            if (!displayQuantity)
            {
                bmp.show_color(lcd, 0xFFFFFFFF, infoX, infoY + 90, infoX + 125, infoY + 135);
                showFoodInfo(font, foodName, price, ("数量: " + to_string(0)).c_str(), infoX, infoY);
            }
            showButton(buttonX, buttonY);

            // 存储每个食品项目的按钮位置信息和食品名称
            FoodButtonInfo info;
            info.foodName = foodName;
            info.foodType = menuname;
            info.current_page = currentPage;
            info.price = price;
            info.x = buttonX;
            info.y = buttonY + 135;

            // 检查foodButtonInfo是否已存在于foodButtenInfos中
            bool found = false;
            for (const auto &existingInfo : foodButtonInfos)
            {
                if (existingInfo.foodName == info.foodName)
                {
                    found = true;
                    break;
                }
            }

            // 仅在foodButtonInfo不存在时添加它
            if (!found)
            {
                foodButtonInfos.push_back(info);
            }
        }

        startX = 160;
        startY = 45;
    }
}

// 自定义比较函数，比较两个 SelectFood 对象
bool compareSelectFood(const SelectFood &a, const SelectFood &b)
{
    // 根据需要的比较逻辑编写比较操作
    // 这里假设比较的属性是 foodName
    return a.foodName < b.foodName;
}

// 展示购物车中的的食物信息
void Menu::showPayInfo(font_utf8 &font, int &x, int &y)
{
    // 每页显示的项目数量和字体设置
    const int itemsPerPage = 4;

    // 处理触摸屏事件
    while (true)
    {
        const int fontX = 170, fontYStart = 110, fontYStep = 80;
        int startingIndex = currentPage * itemsPerPage;
        int endingIndex = min<int>(startingIndex + itemsPerPage, static_cast<int>(selectFood.size()));

        bmp.show_color(lcd, 0xFFFFFFFF, 160, 90, 600, 420);

        int fontY = fontYStart;
        int count = 0; // 记录已显示的食物数量

        for (const auto &sf : selectFood)
        {
            if (count >= startingIndex && count < endingIndex)
            {
                displayMenuInfo(font, sf.foodType, sf.foodName, sf.price, sf.num, fontX, fontY);
                fontY += fontYStep; // 切换到下一行
            }
            count++;
        }

        int slideDirection = touch.getSlide(x, y);
        if (slideDirection == 1 && endingIndex < selectFood.size())
        { // 上滑 && 还有下一页数据
            currentPage++;
            break;
        }
        else if (slideDirection == 2 && currentPage > 0)
        { // 下滑 && 还有上一页数据
            currentPage--;
            break;
        }
        else if (slideDirection == 0)
        {
            if (x > 0 && x < 150 && y > 0 && y < 420)
            {
                return;
            }
            if ((x > 650 && x < 800) && (y > 420 && y < 480))
            {
                return;
            }
            handleTouchClick(font, x, y, startingIndex);
            continue;
        }
    }
}

void Menu::displayMenuInfo(font_utf8 &font, const string &foodType, const string &foodName,
                           const string &price, int quantity, int fontX, int fontY)
{
    font.set_text(fontX, fontY, (foodType + " - " + foodName).c_str(), getColor(0, 0, 0, 0), 0);

    string priceText = "价格: " + price;
    font.set_text(fontX + 220, fontY, priceText.c_str(), getColor(0, 0, 0, 0), 0);

    string quantityText = "数量: " + to_string(quantity);
    font.set_text(fontX + 350, fontY, quantityText.c_str(), getColor(0, 0, 0, 0), 0);
}

void Menu::handleTouchClick(font_utf8 &font, int x, int y, int startingIndex)
{
    const int fontYStart = 110, fontYStep = 80;

    // 添加食物数量
    if (x > 750 && x < 800)
    {
        if (y > 100 && y < 140) // 第一个菜单项的范围
        {
            int rowIndex = startingIndex;

            if (rowIndex >= startingIndex && rowIndex < selectFood.size())
            {
                auto it = selectFood.begin();
                advance(it, rowIndex);

                it->num++;
                calculateTotalPrice(font);
            }
        }
        else if (y > 180 && y < 220) // 第二个菜单项的范围
        {
            int rowIndex = startingIndex + 1;

            if (rowIndex >= startingIndex && rowIndex < selectFood.size())
            {
                auto it = selectFood.begin();
                advance(it, rowIndex);

                it->num++;
                calculateTotalPrice(font);
            }
        }
        else if (y > 260 && y < 300) // 第三个菜单项的范围
        {
            int rowIndex = startingIndex + 2;

            if (rowIndex >= startingIndex && rowIndex < selectFood.size())
            {
                auto it = selectFood.begin();
                advance(it, rowIndex);

                it->num++;
                calculateTotalPrice(font);
            }
        }
        else if (y > 340 && y < 380) // 第四个菜单项的范围
        {
            int rowIndex = startingIndex + 3;

            if (rowIndex >= startingIndex && rowIndex < selectFood.size())
            {
                auto it = selectFood.begin();
                advance(it, rowIndex);

                it->num++;
                calculateTotalPrice(font);
            }
        }
    }
    else if (x > 650 && x < 700) // 减少食物数量
    {
        if (y > 100 && y < 140) // 第一个菜单项的范围
        {
            int rowIndex = startingIndex;

            if (rowIndex >= startingIndex && rowIndex < selectFood.size())
            {
                auto it = selectFood.begin();
                advance(it, rowIndex);

                it->num--;
                if (it->num == 0)
                {
                    selectFood.erase(it);
                }
                calculateTotalPrice(font);
            }
        }
        else if (y > 180 && y < 220) // 第二个菜单项的范围
        {
            int rowIndex = startingIndex + 1;

            if (rowIndex >= startingIndex && rowIndex < selectFood.size())
            {
                auto it = selectFood.begin();
                advance(it, rowIndex);

                it->num--;
                if (it->num == 0)
                {
                    selectFood.erase(it);
                }
                calculateTotalPrice(font);
            }
        }
        else if (y > 260 && y < 300) // 第三个菜单项的范围
        {
            int rowIndex = startingIndex + 2;

            if (rowIndex >= startingIndex && rowIndex < selectFood.size())
            {
                auto it = selectFood.begin();
                advance(it, rowIndex);

                it->num--;
                if (it->num == 0)
                {
                    selectFood.erase(it);
                }
                calculateTotalPrice(font);
            }
        }
        else if (y > 340 && y < 380) // 第四个菜单项的范围
        {
            int rowIndex = startingIndex + 3;

            if (rowIndex >= startingIndex && rowIndex < selectFood.size())
            {
                auto it = selectFood.begin();
                advance(it, rowIndex);

                it->num--;
                if (it->num == 0)
                {
                    selectFood.erase(it);
                }
                calculateTotalPrice(font);
            }
        }
    }
}

void Menu::payBills(font_utf8 &font, int &x, int &y)
{

    int slideDirection;
    while (1)
    {
        slideDirection = touch.getSlide(x, y);
        if (slideDirection == 0)
        {
            if ((x > 300 && x < 400) && (y > 240 && y < 290))
            {
                bmp.showBMP("./image/pay.bmp", lcd.getAddr(), 150, 40, 800, 420);
                showPayInfo(font, x, y);
                return;
            }
            else if ((x > 400 && x < 500) && (y > 240 && y < 290))
            {
                for (const auto &sf : selectFood)
                {
                    float price = stof(sf.price);
                    totalPrice += sf.num * price;
                }
                if (totalPrice > 0)
                {
                    // 清除掉已选的食物链表中的所有内容
                    clearSelectFood();
                    calculateTotalPrice(font);
                    bmp.showBMP("./image/pay.bmp", lcd.getAddr(), 150, 40, 800, 420);
                }
                else
                {
                    cout << "你还没有选择食物" << endl;
                    bmp.showBMP("./image/pay.bmp", lcd.getAddr(), 150, 40, 800, 420);
                    continue;
                }
            }
            else
            {
                return;
            }
        }
    }
}

int main()
{
    Menu menu;

    if (!menu.lcd.openLcd())
    {
        cerr << "Failed to open LCD" << endl;
        return -1;
    }

    if (!menu.lcd.mapMemory())
    {
        menu.lcd.closeLcd();
        return -1;
    }

    // 定义字库对象并初始化
    font_utf8 font(menu.lcd);
    // 设置字体大小
    font.set_font(28);

    menu.showstart();
    

    int currentIndex = 0;                             // 当前显示的图片索引
    menu.bmp.showBMP("menu.bmp", menu.lcd.getAddr()); // 显示初始图片 点餐界面

    menu.food.init_food();

    // 显示字体
    font.set_text(45, 70, "精品菜式", getColor(0, 255, 255, 255), 0);
    font.set_text(45, 145, "主厨推荐", getColor(0, 255, 255, 255), 0);
    font.set_text(45, 220, "肉食盛宴", getColor(0, 255, 255, 255), 0);
    font.set_text(45, 295, "酒水饮料", getColor(0, 255, 255, 255), 0);
    font.set_text(45, 370, "美味拉面", getColor(0, 255, 255, 255), 0);
    font.set_text(45, 445, " 支 付 ", getColor(0, 255, 255, 255), 0);

    int x = 0, y = 0;
    string FoodType; // 当前选中的菜单栏目

    menu.touch.getCoordinate(x, y);
    int maxCount = 4; // 每页显示的最大图像数
    while (1)
    {
        if ((x > 0 && x < 150) && (y > 45 && y < 120)) // 点击精品菜式
        {
            FoodType = "精品菜式";
            cout << "========== " << FoodType << " ==========" << endl;
            vector<string> imagePaths = menu.food.getFoodImagePaths(FoodType);

            menu.showMenuFoodImages(font, FoodType, imagePaths, maxCount, x, y);
        }
        else if ((x > 0 && x < 150) && (y > 120 && y < 195)) // 点击主厨推荐
        {
            FoodType = "主厨推荐";
            cout << "========== " << FoodType << " ==========" << endl;
            vector<string> imagePaths = menu.food.getFoodImagePaths(FoodType);

            menu.showMenuFoodImages(font, FoodType, imagePaths, maxCount, x, y);
        }
        else if ((x > 0 && x < 150) && (y > 195 && y < 270)) // 点击肉食盛宴
        {
            FoodType = "肉食盛宴";
            cout << "========== " << FoodType << " ==========" << endl;
            vector<string> imagePaths = menu.food.getFoodImagePaths(FoodType);

            menu.showMenuFoodImages(font, FoodType, imagePaths, maxCount, x, y);
        }
        else if ((x > 0 && x < 150) && (y > 270 && y < 345)) // 点击酒水饮料
        {
            FoodType = "酒水饮料";
            cout << "========== " << FoodType << " ==========" << endl;
            vector<string> imagePaths = menu.food.getFoodImagePaths(FoodType);

            menu.showMenuFoodImages(font, FoodType, imagePaths, maxCount, x, y);
        }
        else if ((x > 0 && x < 150) && (y > 345 && y < 420)) // 点击美味拉面
        {
            FoodType = "美味拉面";
            cout << "========== " << FoodType << " ==========" << endl;
            vector<string> imagePaths = menu.food.getFoodImagePaths(FoodType);

            menu.showMenuFoodImages(font, FoodType, imagePaths, maxCount, x, y);
        }
        else if ((x > 0 && x < 650) && (y > 420 && y < 480)) // 点击支付
        {
            cout << "支付" << endl;
            menu.bmp.showBMP("./image/pay.bmp", menu.lcd.getAddr(), 150, 40, 800, 420);
            menu.showPayInfo(font, x, y);
        }
        else if ((x > 650 && x < 800) && (y > 420 && y < 480)) // 点击结算
        {
            cout << "结算" << endl;
            menu.bmp.showBMP("./image/paybills.bmp", menu.lcd.getAddr());
            menu.payBills(font, x, y);
        }
        else
        {
            menu.touch.getSlide(x, y);
            continue;
        }
    }

    menu.lcd.unmapMemory();
    menu.lcd.closeLcd();

    return 0;
}
