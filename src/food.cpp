#include "food.h"
using namespace std;

void Food::show_food() const
{
    cout << "食物名称：" << name << "\t数量：" << num << "\t价格：" << price << endl;
}

string Food::get_image_path() const
{
    return image_path;
}

string Food::get_food_name() const
{
    return this->name;
}

float Food::get_food_price() const
{
    return this->price;
}

int Food::get_food_quantity() const
{
    return this->num;
}

string Food::get_food_name(int index, const string &menuName) const
{
    int i = 0;
    map<string, list<Food>>::const_iterator it = menu.find(menuName);

    if (it != menu.end())
    {
        const list<Food> &foodList = it->second;
        for (const Food &food : foodList)
        {
            if (i == index)
            {
                return food.get_food_name();
            }
            i++;
        }
    }

    return "";
}

float Food::get_food_price(int index, const string &menuName) const
{
    int i = 0;
    map<string, list<Food>>::const_iterator it = menu.find(menuName);

    if (it != menu.end())
    {
        const list<Food> &foodList = it->second;
        for (const Food &food : foodList)
        {
            if (i == index)
            {
                return food.get_food_price();
            }
            i++;
        }
    }

    return 0.0;
}

int Food::get_food_quantity(int index, const string &menuName) const
{
    int i = 0;
    map<string, list<Food>>::const_iterator it = menu.find(menuName);

    if (it != menu.end())
    {
        const list<Food> &foodList = it->second;
        for (const Food &food : foodList)
        {
            if (i == index)
            {
                return food.get_food_quantity();
            }
            i++;
        }
    }

    return 0;
}

void displayMenu(const map<string, list<Food>> &menu)
{
    for (const auto &pair : menu)
    {
        cout << "==================== " << pair.first << " ====================" << endl;
        for (const Food &food : pair.second)
        {
            food.show_food();
        }
        cout << endl;
    }
}

void Food::init_food()
{
    // 添加菜单栏和与之对应的菜品
    readFoodInfo("./foodInfo/food1.txt", "精品菜式");

    readFoodInfo("./foodInfo/food2.txt", "主厨推荐");

    readFoodInfo("./foodInfo/food3.txt", "肉食盛宴");

    readFoodInfo("./foodInfo/food4.txt", "酒水饮料");

    readFoodInfo("./foodInfo/food5.txt", "美味拉面");

    // 显示菜单
    displayMenu(menu);

    return;
}

vector<string> Food::getFoodImagePaths(const string &menuName)
{
    vector<string> imagePaths;
    map<string, list<Food>>::const_iterator it = menu.find(menuName);

    if (it != menu.end())
    {
        for (const Food &food : it->second)
        {
            imagePaths.push_back(food.get_image_path());
        }
    }

    return imagePaths;
}

map<string, list<Food>> &Food::getMenu()
{
    return this->menu;
}

// 从文本中获取食物信息
void Food::readFoodInfo(const string &filename, const string &menuName)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Failed to open file: " << filename << endl;
        return;
    }

    list<Food> foodList;
    string line;
    while (getline(file, line))
    {
        stringstream ss(line);
        string name;
        int num, status;
        float price;
        string image_path;
        char comma;

        if (getline(ss, name, ',') &&
            (ss >> num >> comma >> price >> comma >> status >> comma >> image_path))
        {
            foodList.push_back(Food(name, num, price, status, image_path));
        }
    }

    menu[menuName] = foodList;
    file.close();
}
