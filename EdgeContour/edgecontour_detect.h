// Program: DIP
// FileName:edgecontour_detect.h
// Author:  Lichun Zhang
// Date:    2017/6/21 下午7:26
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#ifndef DIP_EDGECONTOUR_DETECT_H
#define DIP_EDGECONTOUR_DETECT_H

#include <cstddef>
#include <new>
#include <iostream>
#include <cmath>
#include <template_trans.h>

/**
 * @brief 用Robert边缘检测算子对图像进行边缘检测。目标图像为灰度图像。
 * g(x,y)=|f(x,y))-f(x+1,y+1)|+|f(x,y+1)-f(x+1,y)|
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @return 操作是否成功
 */
template<typename T>
bool Robert(T *im, size_t width, size_t height, size_t slice) {
    if (!im) return false;
    T *new_im = nullptr;
    try {
        new_im = new T[width * height];
    }
    catch (std::bad_alloc) {
        std::cout << "Failed to alloc memory!\n";
        return false;
    }

    double result = 0.0;
    size_t p0 = 0, p1 = 0, t = 0;
    for (int k = 0; k < slice; ++k) {
        p0 = k * width * height;
        // 模板为2*2 防止越界，不处理最下与最右
        // 0 1
        // 2 3
        for (int i = 0; i < height - 1; ++i) {
            for (int j = 0; j < width - 1; ++j) {
                p1 = i * width + j;
                t = p0 + p1;
                result = std::abs(im[t] - im[t + width + 1]) + std::abs(im[t + 1] - im[t + width]);
                new_im[p1] = result;
            }
        }
        memcpy(im + p0, new_im, sizeof(T) * width * height);
    }
    delete[] new_im;
    return true;
}

/**
 * @brief Sobel算子边缘检测 目标图像为灰度图
 * 源图中点用垂直、水平2个方向核做卷积,最大值为输出值
 * 垂直方向 -1 -2 -1        水平方向 -1 0 1
 *         0  0  0                -2 0 2
 *         1  2  1                -1 0 1
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @return 操作是否成功
 */
template<typename T>
bool Sobel(T *im, size_t width, size_t height, size_t slice) {
    if (!im) return false;
    T *new_im1 = nullptr;
    T *new_im2 = nullptr;
    try {
        new_im1 = new T[width * height];
        new_im2 = new T[width * height];
    }
    catch (std::bad_alloc) {
        std::cout << "Failed to alloc memory!\n";
        return false;
    }

    // Sobel模板高度 宽度 中心元素x坐标 y坐标
    int filterH = 3, filterW = 3, filterCX = 1, filterCY = 1;
    // 模板系数
    double coeff = 1.0;
    // 垂直方向模板数组
    double tempV[9] = {-1.0, -2.0, -1.0,
                       0.0, 0.0, 0.0,
                       1.0, 2.0, 1.0};
    // 水平方向模板数组
    double tempH[9] = {-1.0, 0.0, 1.0,
                       -2.0, 0.0, 2.0,
                       -1.0, 0.0, 1.0};

    size_t p0 = 0, p1 = 0;
    for (size_t k = 0; k < slice; ++k) {
        p0 = k * width * height;
        memcpy(new_im1, im + p0, sizeof(T) * width * height);
        memcpy(new_im2, im + p0, sizeof(T) * width * height);
        if (!Template(new_im1, width, height, 1,
                      filterW, filterH, filterCX, filterCY, tempV, coeff))
            return false;

        if (!Template(new_im2, width, height, 1,
                      filterW, filterH, filterCX, filterCY, tempH, coeff))
            return false;
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                p1 = i * width + j;
                if (new_im1[p1] < new_im2[p1]) new_im1[p1] = new_im2[p1];
            }
        }
        memcpy(im + p0, new_im1, sizeof(T) * width * height);
    }
    delete[] new_im2;
    delete[] new_im1;
    return true;

}

/**
 * @brief Prewitt算子边缘检测 目标图像为灰度图
 * 源图中点用垂直、水平2个方向核做卷积,最大值为输出值
 * 垂直方向 -1 -1 -1        水平方向  1 0 -1
 *         0  0  0                 1 0 -1
 *         1  1  1                 1 0 -1
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @return 操作是否成功
 */
template<typename T>
bool Prewitt(T *im, size_t width, size_t height, size_t slice) {
    if (!im) return false;
    T *new_im1 = nullptr;
    T *new_im2 = nullptr;
    try {
        new_im1 = new T[width * height];
        new_im2 = new T[width * height];
    }
    catch (std::bad_alloc) {
        std::cout << "Failed to alloc memory!\n";
        return false;
    }

    // Sobel模板高度 宽度 中心元素x坐标 y坐标
    int filterH = 3, filterW = 3, filterCX = 1, filterCY = 1;
    // 模板系数
    double coeff = 1.0;
    // 垂直方向模板数组
    double tempV[9] = {-1.0, -1.0, -1.0,
                       0.0, 0.0, 0.0,
                       1.0, 1.0, 1.0};
    // 水平方向模板数组
    double tempH[9] = {1.0, 0.0, -1.0,
                       1.0, 0.0, -1.0,
                       1.0, 0.0, -1.0};

    size_t p0 = 0, p1 = 0;
    for (size_t k = 0; k < slice; ++k) {
        p0 = k * width * height;
        memcpy(new_im1, im + p0, sizeof(T) * width * height);
        memcpy(new_im2, im + p0, sizeof(T) * width * height);
        if (!Template(new_im1, width, height, 1,
                      filterW, filterH, filterCX, filterCY, tempV, coeff))
            return false;

        if (!Template(new_im2, width, height, 1,
                      filterW, filterH, filterCX, filterCY, tempH, coeff))
            return false;
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                p1 = i * width + j;
                if (new_im1[p1] < new_im2[p1]) new_im1[p1] = new_im2[p1];
            }
        }
        memcpy(im + p0, new_im1, sizeof(T) * width * height);
    }
    delete[] new_im2;
    delete[] new_im1;
    return true;

}

/**
 * @brief Krisch算子边缘检测 目标图像为灰度图
 * 源图中点用8个方向核做卷积,最大值为输出值
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @return 操作是否成功
 */
template<typename T>
bool Krisch(T *im, size_t width, size_t height, size_t slice) {
    if (!im) return false;
    T *new_im1 = nullptr;
    T *new_im2 = nullptr;
    try {
        new_im1 = new T[width * height];
        new_im2 = new T[width * height];
    }
    catch (std::bad_alloc) {
        std::cout << "Failed to alloc memory!\n";
        return false;
    }

    // Sobel模板高度 宽度 中心元素x坐标 y坐标
    int filterH = 3, filterW = 3, filterCX = 1, filterCY = 1;
    // 模板系数
    double coeff = 1.0;
    // 1方向模板数组
    double temp1[9] = {5.0, 5.0, 5.0,
                       -3.0, 0.0, -3.0,
                       -3.0, -3.0, -3.0};
    // 2方向模板数组
    double temp2[9] = {-3.0, 5.0, 5.0,
                       -3.0, 0.0, 5.0,
                       -3.0, -3.0, -3.0};
    // 3方向模板数组
    double temp3[9] = {-3.0, -3.0, 5.0,
                       -3.0, 0.0, 5.0,
                       -3.0, -3.0, 5.0};
    // 4方向模板数组
    double temp4[9] = {-3.0, -3.0, -3.0,
                       -3.0, 0.0, 5.0,
                       -3.0, 5.0, 5.0};
    // 5方向模板数组
    double temp5[9] = {-3.0, -3.0, -3.0,
                       -3.0, 0.0, -3.0,
                       5.0, 5.0, 5.0};
    // 6方向模板数组
    double temp6[9] = {-3.0, -3.0, -3.0,
                       5.0, 0.0, -3.0,
                       5.0, 5.0, -3.0};
    // 7方向模板数组
    double temp7[9] = {5.0, -3.0, -3.0,
                       5.0, 0.0, -3.0,
                       5.0, -3.0, -3.0};
    // 8方向模板数组
    double temp8[9] = {5.0, 5.0, -3.0,
                       5.0, 0.0, -3.0,
                       -3.0, -3.0, -3.0};
    double *temps[8] = {temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8};

    const int N = 8;    //8个方向的模板数组
    size_t p0 = 0, p1 = 0;
    for (size_t k = 0; k < slice; ++k) {
        p0 = k * width * height;

        // 源图与第1个模板进行卷积运算
        memcpy(new_im1, im + p0, sizeof(T) * width * height);
        if (!Template(new_im1, width, height, 1,
                      filterW, filterH, filterCX, filterCY, temps[0], coeff))
            return false;

        // 再与7个模板进行卷积运算，将最大的值放入第1个模板的运算结果中
        for (int i = 1; i < N; ++i) {
            memcpy(new_im2, im + p0, sizeof(T) * width * height);
            if (!Template(new_im2, width, height, 1,
                          filterW, filterH, filterCX, filterCY, temps[i], coeff))
                return false;
            for (int i = 0; i < height; ++i) {
                for (int j = 0; j < width; ++j) {
                    p1 = i * width + j;
                    if (new_im1[p1] < new_im2[p1]) new_im1[p1] = new_im2[p1];
                }
            }
        }
        memcpy(im + p0, new_im1, sizeof(T) * width * height);

    }
    delete[] new_im2;
    delete[] new_im1;
    return true;

}

/**
 * @brief 高斯拉普拉斯算子进行边缘检测 目标图像为灰度图
 * 源图用5*5高斯拉普拉斯卷积核卷积。
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @return 操作是否成功
 */
template<typename T>
bool GaussLaplace(T *im, size_t width, size_t height, size_t slice) {

    if (!im) return false;
    T *new_im = nullptr;
    try {
        new_im = new T[width * height];
    }
    catch (std::bad_alloc) {
        std::cout << "Failed to alloc memory!\n";
        return false;
    }

    // Sobel模板高度 宽度 中心元素x坐标 y坐标
    int filterH = 5, filterW = 5, filterCX = 2, filterCY = 2;
    // 模板系数
    double coeff = 1.0;
    // 模板数组
    double temp[25] = {-2.0, -4.0, -4.0, -4.0, -2.0,
                       -4.0, 0.0, 8.0, 0.0, -4.0,
                       -4.0, 8.0, 24.0, 8.0, -4.0,
                       -4.0, 0.0, 8.0, 0.0, -4.0,
                       -2.0, -4.0, -4.0, -4.0, -2.0};
    size_t p0 = 0;
    for (size_t k = 0; k < slice; ++k) {
        p0 = k * width * height;
        memcpy(new_im, im + p0, sizeof(T) * width * height);
        if (!Template(new_im, width, height, 1,
                      filterW, filterH, filterCX, filterCY, temp, coeff))
            return false;
        memcpy(im + p0, new_im, sizeof(T) * width * height);
    }
    delete[] new_im;
    return true;
}

#endif //DIP_EDGECONTOUR_DETECT_H
