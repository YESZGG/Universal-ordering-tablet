#include "bmp.h"
#include "lcd.h"

using namespace std;

#define BUFFER_SIZE 800 * 480
#define lcdWidth 800
#define lcdHeight 480

BMP::BMP()
{
}

void BMP::show_color(LCD &lcd, unsigned int color, int startX, int startY, int endX, int endY)
{
    for (int i = startY; i < endY; i++)
    {
        for (int j = startX; j < endX; j++)
        {
            *(lcd.getAddr() + i * 800 + j) = color;
        }
    }
}

void BMP::showBMP(const string pathname, unsigned int *addr)
{
    // 打开BMP文件
    FILE *bmp_fp = fopen(pathname.c_str(), "rb");
    if (bmp_fp == NULL)
    {
        perror("Failed to open BMP file");
        return;
    }

    // 读取BMP标头信息
    BMPHeader bmpInfo;
    fread(&bmpInfo, sizeof(BMPHeader), 1, bmp_fp);

    // 检查BMP文件的有效性
    if (memcmp(bmpInfo.signature, "BM", 2) != 0)
    {
        cerr << "Invalid BMP file" << endl;
        fclose(bmp_fp);
        return;
    }

    // 打印图像信息
    // cout << "Width: " << bmpInfo.width << endl;
    // cout << "Height: " << bmpInfo.height << endl;
    // cout << "BMP SIZE: " << (double)bmpInfo.fileSize / 1024.00 << " KB" << endl;
    int bmpWidth = bmpInfo.width;
    int bmpHeight = bmpInfo.height;

    // 验证BMP图像和LCD屏幕尺寸之间的兼容性
    if (bmpWidth > lcdWidth || bmpHeight > lcdHeight)
    {
        cerr << "BMP图像尺寸超过LCD屏幕尺寸" << endl;
        fclose(bmp_fp);
        return;
    }

    fseek(bmp_fp, bmpInfo.dataOffset, SEEK_SET);

    vector<unsigned char> color(3 * bmpWidth * bmpHeight);
    fread(color.data(), 1, bmpWidth * bmpHeight * 3, bmp_fp);

    // int offset_x = (lcdWidth - bmpWidth) / 2;   // 计算图像居中的水平偏移
    // int offset_y = (lcdHeight - bmpHeight) / 2; // 计算图像居中的垂直偏移

    int offset_x = (lcdWidth - bmpInfo.width) / 2;
    int offset_y = (lcdHeight - bmpInfo.height) / 2;

    for (int y = 0; y < bmpInfo.height; y++)
    {
        for (int x = 0; x < bmpInfo.width; x++)
        {
            unsigned char blue = color[(y * bmpInfo.width + x) * 3];
            unsigned char green = color[(y * bmpInfo.width + x) * 3 + 1];
            unsigned char red = color[(y * bmpInfo.width + x) * 3 + 2];

            *(addr + (offset_y + bmpInfo.height - 1 - y) * lcdWidth + offset_x + x) =
                (red << 16) | (green << 8) | blue;
        }
    }

#if 0
    for (int y = 0; y < bmpHeight; y++)
    {
        for (int x = 0; x < bmpWidth; x++)
        {
            // 遇到白色就跳过，不绘画，可将0xffffff修改成不需要的颜色
            //  if (*(addr + (offset_y + bmpHeight - 1 - y) * 800 + offset_x + x) == 0x00ffffff)
            //  {
            //      continue;
            //  }
            //  将读取出来的图片数据的每个像素点 BGR ---转换成 ARGB
            unsigned char blue = color[(y * bmpWidth + x) * 3];
            unsigned char green = color[(y * bmpWidth + x) * 3 + 1];
            unsigned char red = color[(y * bmpWidth + x) * 3 + 2];

            *(addr + (offset_y + bmpHeight - 1 - y) * 800 + offset_x + x) =
                (red << 16) | (green << 8) | blue;
        }
    }
#endif
#if 0
    char color32[lcdWidth * lcdHeight * 4] = {0};

    for (int y = 0; y < bmpHeight; y++)
    {
        for (int x = 0; x < bmpWidth; x++)
        {
            int sourceIndex = ((bmpHeight - y - 1) * bmpWidth + x) * 3;
            int targetIndex = ((offset_y + y) * lcdWidth + offset_x + x) * 4;
            color32[targetIndex + 0] = color[sourceIndex + 0];
            color32[targetIndex + 1] = color[sourceIndex + 1];
            color32[targetIndex + 2] = color[sourceIndex + 2];
            color32[targetIndex + 3] = 0;
        }
    }

    int(*d_lcd)[lcdWidth] = (int(*)[lcdWidth])addr;
    int(*d_color)[lcdWidth] = (int(*)[lcdWidth])color32;

    for (int y = 0; y < lcdHeight; y++)
    {
        for (int x = 0; x < lcdWidth; x++)
        {
            d_lcd[y][x] = d_color[y][x];
        }
    }
#endif
    // 关闭文件描述符
    fclose(bmp_fp);
}

// 指定位置显示图片
void BMP::showBMP(const string pathname, unsigned int *addr, int startX, int startY)
{
    // 打开BMP文件
    FILE *bmp_fp = fopen(pathname.c_str(), "rb");
    if (bmp_fp == NULL)
    {
        perror("Failed to open BMP file");
        return;
    }

    // 读取BMP标头信息
    BMPHeader bmpInfo;
    fread(&bmpInfo, sizeof(BMPHeader), 1, bmp_fp);

    // 检查BMP文件的有效性
    if (memcmp(bmpInfo.signature, "BM", 2) != 0)
    {
        cerr << "Invalid BMP file" << endl;
        fclose(bmp_fp);
        return;
    }

    // 打印图像信息
    // cout << "Width: " << bmpInfo.width << endl;
    // cout << "Height: " << bmpInfo.height << endl;
    // cout << "BMP SIZE: " << (double)bmpInfo.fileSize / 1024.00 << " KB" << endl;

    int bmpWidth = bmpInfo.width;
    int bmpHeight = bmpInfo.height;

    // 验证BMP图像和LCD屏幕尺寸之间的兼容性
    if (bmpWidth > 800 || bmpHeight > 480)
    {
        cerr << "BMP image dimensions exceed the LCD screen dimensions" << endl;
        fclose(bmp_fp);
        return;
    }

    fseek(bmp_fp, bmpInfo.dataOffset, SEEK_SET);

    vector<unsigned char> color(bmpWidth * bmpHeight * 3);
    fread(color.data(), 1, bmpWidth * bmpHeight * 3, bmp_fp);

    int offset_x = startX; // 指定图像的水平起始位置
    int offset_y = startY; // 指定图像的垂直起始位置

#if 1
    //  使用指针映射
    for (int y = 0; y < bmpHeight; y++)
    {
        for (int x = 0; x < bmpWidth; x++)
        {
            // 遇到白色就跳过，不绘画，可将0xffffff修改成不需要的颜色
            // if (*(addr + (offset_y + bmpHeight - 1 - y) * 800 + offset_x + x) == 0x00000000)
            // {
            //     continue;
            // }
            unsigned char blue = color[(y * bmpWidth + x) * 3];
            unsigned char green = color[(y * bmpWidth + x) * 3 + 1];
            unsigned char red = color[(y * bmpWidth + x) * 3 + 2];

            *(addr + (offset_y + bmpHeight - 1 - y) * 800 + offset_x + x) =
                (red << 16) | (green << 8) | blue;
        }
    }
#endif
#if 0
    char color32[lcdWidth * lcdHeight * 4] = {0};

    for (int y = 0; y < bmpHeight; y++)
    {
        for (int x = 0; x < bmpWidth; x++)
        {
            int sourceIndex = ((bmpHeight - y - 1) * bmpWidth + x) * 3;
            int targetIndex = ((offset_y + y) * lcdWidth + offset_x + x) * 4;
            color32[targetIndex + 0] = color[sourceIndex + 0];
            color32[targetIndex + 1] = color[sourceIndex + 1];
            color32[targetIndex + 2] = color[sourceIndex + 2];
            color32[targetIndex + 3] = 0;
        }
    }

    int(*d_lcd)[lcdWidth] = (int(*)[lcdWidth])addr;
    int(*d_color)[lcdWidth] = (int(*)[lcdWidth])color32;

    for (int y = 0; y < lcdHeight; y++)
    {
        for (int x = 0; x < lcdWidth; x++)
        {
            d_lcd[y][x] = d_color[y][x];
        }
    }
#endif
    // 关闭文件描述符
    fclose(bmp_fp);
}

// 指定位置显示图片
void BMP::showBMP(const string pathname, unsigned int *addr, int startX, int startY, int endX, int endY)
{
    // 打开BMP文件
    FILE *bmp_fp = fopen(pathname.c_str(), "rb");
    if (bmp_fp == nullptr)
    {
        perror("Failed to open BMP file");
        return;
    }

    // 读取BMP标头信息
    BMPHeader bmpInfo;
    fread(&bmpInfo, sizeof(BMPHeader), 1, bmp_fp);

    // 检查BMP文件的有效性
    if (memcmp(bmpInfo.signature, "BM", 2) != 0)
    {
        cerr << "Invalid BMP file" << endl;
        fclose(bmp_fp);
        return;
    }

    int bmpWidth = bmpInfo.width;
    int bmpHeight = bmpInfo.height;

    // 验证BMP图像尺寸和指定显示区域的有效性
    if (endX > bmpWidth || endY > bmpHeight || startX >= endX || startY >= endY)
    {
        cerr << "Invalid display region" << endl;
        fclose(bmp_fp);
        return;
    }

    fseek(bmp_fp, bmpInfo.dataOffset, SEEK_SET);

    vector<unsigned char> color(bmpWidth * bmpHeight * 3);
    fread(color.data(), 1, bmpWidth * bmpHeight * 3, bmp_fp);

    for (int y = startY; y < endY; y++)
    {
        for (int x = startX; x < endX; x++)
        {
            unsigned char blue = color[(y * bmpWidth + x) * 3];
            unsigned char green = color[(y * bmpWidth + x) * 3 + 1];
            unsigned char red = color[(y * bmpWidth + x) * 3 + 2];

            *(addr + (bmpHeight - 1 - y) * bmpWidth + x) =
                (red << 16) | (green << 8) | blue;
        }
    }

    // 关闭文件描述符
    fclose(bmp_fp);
}

// 缩放图片
void BMP::showBMP(const string pathname, unsigned int *addr, int scale)
{
    memset(addr, 0, BUFFER_SIZE * 4);

    FILE *bmp_fp = fopen(pathname.c_str(), "rb"); // 打开BMP文件
    if (bmp_fp == NULL)                           // 检查文件是否成功打开
    {
        perror("Failed to open BMP file");
        return;
    }

    BMPHeader bmpInfo;
    fread(&bmpInfo, sizeof(BMPHeader), 1, bmp_fp); // 读取BMP文件头信息

    if (memcmp(bmpInfo.signature, "BM", 2) != 0) // 检查BMP文件的有效性
    {
        cerr << "Invalid BMP file" << endl;
        fclose(bmp_fp);
        return;
    }

    cout << "Original Width: " << bmpInfo.width << endl;   // 输出原始图像宽度
    cout << "Original Height: " << bmpInfo.height << endl; // 输出原始图像高度
    cout << "BMP SIZE: " << (double)bmpInfo.fileSize << endl;
    int originalWidth = bmpInfo.width;
    int originalHeight = bmpInfo.height;

    // 计算缩放尺寸
    int scaledWidth = originalWidth / scale;
    int scaledHeight = originalHeight / scale;

    // 根据LCD屏幕尺寸验证缩放尺寸
    if (scaledWidth > lcdWidth || scaledHeight > lcdHeight)
    {
        cerr << "缩放图像尺寸超过LCD屏幕尺寸" << endl;
        fclose(bmp_fp);
        return;
    }

    // 获取图片数据的起始位置
    fseek(bmp_fp, bmpInfo.dataOffset, SEEK_SET);

    vector<unsigned char> color(3 * originalWidth * originalHeight);    // 用于存储原始未压缩 BMP 图像中每个像素的 RGB 值
    fread(color.data(), 1, originalWidth * originalHeight * 3, bmp_fp); // 读取BMP图像数据

    int offset_x = (lcdWidth - scaledWidth) / 2;   // 计算用于居中的水平偏移量
    int offset_y = (lcdHeight - scaledHeight) / 2; // 计算居中的垂直偏移量

    for (int y = 0; y < scaledHeight; y++)
    {
        for (int x = 0; x < scaledWidth; x++)
        {
            // 计算相应像素在原始图像中的位置
            int originalX = x * scale;
            int originalY = y * scale;

            // 初始化每个通道的平均颜色值
            unsigned int avgRed = 0, avgGreen = 0, avgBlue = 0;

            // 计算x尺度区域内的平均颜色
            for (int i = 0; i < scale; i++)
            {
                for (int j = 0; j < scale; j++)
                {
                    unsigned char blue = color[((originalY + i) * originalWidth + originalX + j) * 3];
                    unsigned char green = color[((originalY + i) * originalWidth + originalX + j) * 3 + 1];
                    unsigned char red = color[((originalY + i) * originalWidth + originalX + j) * 3 + 2];

                    // 平均值
                    avgRed += red;
                    avgGreen += green;
                    avgBlue += blue;
                }
            }

            // 通过将平均颜色值除以比例^2来规范化它们
            avgRed /= (scale * scale);
            avgGreen /= (scale * scale);
            avgBlue /= (scale * scale);

            // 从RGB转换为ARGB并设置像素值
            *(addr + (offset_y + scaledHeight - 1 - y) * lcdWidth + offset_x + x) = (avgRed << 16) | (avgGreen << 8) | avgBlue;
        }
    }

    fclose(bmp_fp); // 关闭文件描述符
}

// 指定位置缩放图片
void BMP::showBMP(const string pathname, unsigned int *addr, int scale, int posX, int posY)
{
    FILE *bmp_fp = fopen(pathname.c_str(), "rb"); // 打开BMP文件
    if (bmp_fp == NULL)                           // 检查文件是否成功打开
    {
        perror("Failed to open BMP file");
        return;
    }

    BMPHeader bmpInfo;
    fread(&bmpInfo, sizeof(BMPHeader), 1, bmp_fp); // 读取BMP文件头信息

    if (memcmp(bmpInfo.signature, "BM", 2) != 0) // 检查BMP文件的有效性
    {
        cerr << "Invalid BMP file" << endl;
        fclose(bmp_fp);
        return;
    }

    cout << "Original Width: " << bmpInfo.width << endl;   // 输出原始图像宽度
    cout << "Original Height: " << bmpInfo.height << endl; // 输出原始图像高度
    cout << "BMP SIZE: " << (double)bmpInfo.fileSize / 1024.00 << " KB" << endl;

    int originalWidth = bmpInfo.width;
    int originalHeight = bmpInfo.height;

    // 计算缩放尺寸
    int scaledWidth = originalWidth / scale;
    int scaledHeight = originalHeight / scale;

    // 根据LCD屏幕尺寸验证缩放尺寸和位置
    if (posX < 0 || posY < 0 || posX + scaledWidth > lcdWidth || posY + scaledHeight > lcdHeight)
    {
        cerr << "缩放图像尺寸超过LCD屏幕尺寸或位置无效" << endl;
        fclose(bmp_fp);
        return;
    }

    // 获取图片数据的起始位置
    fseek(bmp_fp, bmpInfo.dataOffset, SEEK_SET);

    vector<unsigned char> color(3 * originalWidth * originalHeight);    // 用于存储原始未压缩 BMP 图像中每个像素的 RGB 值
    fread(color.data(), 1, originalWidth * originalHeight * 3, bmp_fp); // 读取BMP图像数据

    int offset_x = (lcdWidth - scaledWidth) / 2;   // 计算用于居中的水平偏移量
    int offset_y = (lcdHeight - scaledHeight) / 2; // 计算居中的垂直偏移量

    for (int y = 0; y < scaledHeight; y++)
    {
        for (int x = 0; x < scaledWidth; x++)
        {
            // 计算相应像素在原始图像中的位置
            int originalX = x * scale;
            int originalY = y * scale;

            // 初始化每个通道的平均颜色值
            unsigned int avgRed = 0, avgGreen = 0, avgBlue = 0;

            // 计算x尺度区域内的平均颜色
            for (int i = 0; i < scale; i++)
            {
                for (int j = 0; j < scale; j++)
                {
                    unsigned char blue = color[((originalY + i) * originalWidth + originalX + j) * 3];
                    unsigned char green = color[((originalY + i) * originalWidth + originalX + j) * 3 + 1];
                    unsigned char red = color[((originalY + i) * originalWidth + originalX + j) * 3 + 2];

                    // 平均值
                    avgRed += red;
                    avgGreen += green;
                    avgBlue += blue;
                }
            }

            // 通过将平均颜色值除以比例^2来规范化它们
            avgRed /= (scale * scale);
            avgGreen /= (scale * scale);
            avgBlue /= (scale * scale);

            // 从RGB转换为ARGB并设置像素值，将像素绘制到指定位置
            *(addr + (posY + scaledHeight - 1 - y) * lcdWidth + posX + x) = (avgRed << 16) | (avgGreen << 8) | avgBlue;
        }
    }

    fclose(bmp_fp); // 关闭文件描述符
}

void BMP::showBMP(const string pathname, unsigned int *addr, float scaleX, float scaleY)
{
    memset(addr, 0, BUFFER_SIZE * 4);

    FILE *bmp_fp = fopen(pathname.c_str(), "rb"); // 打开BMP文件
    if (bmp_fp == NULL)                           // 检查文件是否成功打开
    {
        perror("Failed to open BMP file");
        return;
    }

    BMPHeader bmpInfo;
    fread(&bmpInfo, sizeof(BMPHeader), 1, bmp_fp); // 读取BMP文件头信息

    if (memcmp(bmpInfo.signature, "BM", 2) != 0) // 检查BMP文件的有效性
    {
        cerr << "Invalid BMP file" << endl;
        fclose(bmp_fp);
        return;
    }

    cout << "Original Width: " << bmpInfo.width << endl;   // 输出原始图像宽度
    cout << "Original Height: " << bmpInfo.height << endl; // 输出原始图像高度
    cout << "BMP SIZE: " << (double)bmpInfo.fileSize / 1024.00 << " KB" << endl;

    int originalWidth = bmpInfo.width;
    int originalHeight = bmpInfo.height;
    // 计算缩放尺寸
    int scaledWidth = static_cast<int>(originalWidth * scaleX);
    int scaledHeight = static_cast<int>(originalHeight * scaleY);

    // 根据LCD屏幕尺寸验证缩放尺寸
    if (scaledWidth > lcdWidth || scaledHeight > lcdHeight)
    {
        cerr << "缩放图像尺寸超过LCD屏幕尺寸" << endl;
        fclose(bmp_fp);
        return;
    }

    // 获取图片数据的起始位置
    fseek(bmp_fp, bmpInfo.dataOffset, SEEK_SET);

    vector<unsigned char> color(3 * originalWidth * originalHeight);    // 用于存储原始未压缩 BMP 图像中每个像素的 RGB 值
    fread(color.data(), 1, originalWidth * originalHeight * 3, bmp_fp); // 读取BMP图像数据

    int offset_x = (lcdWidth - scaledWidth) / 2;   // 计算用于居中的水平偏移量
    int offset_y = (lcdHeight - scaledHeight) / 2; // 计算居中的垂直偏移量

    for (int y = 0; y < scaledHeight; y++)
    {
        for (int x = 0; x < scaledWidth; x++)
        {
            // 计算相应像素在原始图像中的位置
            int originalX = static_cast<int>(x / scaleX);
            int originalY = static_cast<int>(y / scaleY);

            // 初始化每个通道的平均颜色值
            unsigned int avgRed = 0, avgGreen = 0, avgBlue = 0;

            // 计算x尺度区域内的平均颜色
            for (int i = 0; i < scaleY; i++)
            {
                for (int j = 0; j < scaleX; j++)
                {
                    unsigned char blue = color[((originalY + i) * originalWidth + originalX + j) * 3];
                    unsigned char green = color[((originalY + i) * originalWidth + originalX + j) * 3 + 1];
                    unsigned char red = color[((originalY + i) * originalWidth + originalX + j) * 3 + 2];

                    // 平均值
                    avgRed += red;
                    avgGreen += green;
                    avgBlue += blue;
                }
            }

            // 通过将平均颜色值除以比例^2来规范化它们
            avgRed /= (scaleY * scaleX);
            avgGreen /= (scaleY * scaleX);
            avgBlue /= (scaleY * scaleX);

            // 从RGB转换为ARGB并设置像素值
            *(addr + (offset_y + scaledHeight - 1 - y) * lcdWidth + offset_x + x) = (avgRed << 16) | (avgGreen << 8) | avgBlue;
        }
    }

    fclose(bmp_fp); // 关闭文件描述符
}

void BMP::showBMP(const string pathname, unsigned int *addr, float scaleX, float scaleY, int posX, int posY)
{
    memset(addr, 0, BUFFER_SIZE * 4);

    FILE *bmp_fp = fopen(pathname.c_str(), "rb"); // 打开BMP文件
    if (bmp_fp == NULL)                           // 检查文件是否成功打开
    {
        perror("Failed to open BMP file");
        return;
    }

    BMPHeader bmpInfo;
    fread(&bmpInfo, sizeof(BMPHeader), 1, bmp_fp); // 读取BMP文件头信息

    if (memcmp(bmpInfo.signature, "BM", 2) != 0) // 检查BMP文件的有效性
    {
        cerr << "Invalid BMP file" << endl;
        fclose(bmp_fp);
        return;
    }

    cout << "Original Width: " << bmpInfo.width << endl;   // 输出原始图像宽度
    cout << "Original Height: " << bmpInfo.height << endl; // 输出原始图像高度
    cout << "BMP SIZE: " << (double)bmpInfo.fileSize / 1024.00 << " KB" << endl;

    int originalWidth = bmpInfo.width;
    int originalHeight = bmpInfo.height;
    // 计算缩放尺寸
    int scaledWidth = static_cast<int>(originalWidth * scaleX);
    int scaledHeight = static_cast<int>(originalHeight * scaleY);

    // 根据LCD屏幕尺寸验证缩放尺寸和位置
    if (posX < 0 || posY < 0 || posX + scaledWidth > lcdWidth || posY + scaledHeight > lcdHeight)
    {
        cerr << "缩放图像尺寸超过LCD屏幕尺寸或位置无效" << endl;
        fclose(bmp_fp);
        return;
    }

    // 获取图片数据的起始位置
    fseek(bmp_fp, bmpInfo.dataOffset, SEEK_SET);

    vector<unsigned char> color(3 * originalWidth * originalHeight);    // 用于存储原始未压缩 BMP 图像中每个像素的 RGB 值
    fread(color.data(), 1, originalWidth * originalHeight * 3, bmp_fp); // 读取BMP图像数据

    int offset_x = (lcdWidth - scaledWidth) / 2;   // 计算用于居中的水平偏移量
    int offset_y = (lcdHeight - scaledHeight) / 2; // 计算居中的垂直偏移量

    for (int y = 0; y < scaledHeight; y++)
    {
        for (int x = 0; x < scaledWidth; x++)
        {
            // 计算相应像素在原始图像中的位置
            int originalX = static_cast<int>(x / scaleX);
            int originalY = static_cast<int>(y / scaleY);
            // 初始化每个通道的平均颜色值
            unsigned int avgRed = 0, avgGreen = 0, avgBlue = 0;

            // 计算x尺度区域内的平均颜色
            for (int i = 0; i < scaleY; i++)
            {
                for (int j = 0; j < scaleX; j++)
                {
                    unsigned char blue = color[((originalY + i) * originalWidth + originalX + j) * 3];
                    unsigned char green = color[((originalY + i) * originalWidth + originalX + j) * 3 + 1];
                    unsigned char red = color[((originalY + i) * originalWidth + originalX + j) * 3 + 2];

                    // 平均值
                    avgRed += red;
                    avgGreen += green;
                    avgBlue += blue;
                }
            }

            // 通过将平均颜色值除以比例^2来规范化它们
            avgRed /= (scaleY * scaleX);
            avgGreen /= (scaleY * scaleX);
            avgBlue /= (scaleY * scaleX);

            // 从RGB转换为ARGB并设置像素值，将像素绘制到指定位置
            *(addr + (posY + scaledHeight - 1 - y) * lcdWidth + posX + x) = (avgRed << 16) | (avgGreen << 8) | avgBlue;
        }
    }

    // ...
}
