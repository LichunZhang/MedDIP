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

/**
 * @brief 轮廓提取运算 根据8领域 效率较低
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @return 操作是否成功
 */
template<typename T>
bool Contour(T *im, size_t width, size_t height, size_t slice) {
    if (!im) return false;
    T *new_im = nullptr;
    try {
        new_im = new T[width * height];
    }
    catch (std::bad_alloc) {
        std::cout << "Failed to alloc memory!\n";
        return false;
    }

    // 八个方向
    T n, s, w, e, nw, ne, sw, se;
    size_t p = 0, t = 0;
    T max = std::numeric_limits<T>::max();
    for (size_t k = 0; k < slice; ++k) {
        // 初始化全为白色
        memset(new_im, max, sizeof(T) * width * height);
        p = k * width * height;
        for (size_t i = 1; i < height - 1; ++i) {
            for (size_t j = 1; j < width - 1; ++j) {
                t = p + i * width + j;
                // 若当前点为黑 统计当前点的八个方向
                if (im[t] == 0) {
                    n = im[t - width];
                    nw = im[t - width - 1];
                    ne = im[t - width + 1];
                    s = im[t + width];
                    sw = im[t + width - 1];
                    se = im[t + width + 1];
                    w = im[t - 1];
                    e = im[t + 1];
                    // 若不全为黑(边界 非内部点) 则设黑
                    // 否则全黑(非边界 内部点) 仍然是初始值白
                    if (n || nw || ne || s || sw || se || w || e)
                        new_im[i * width + j] = 0;
                }
            }
        }
        memcpy(im + p, new_im, sizeof(T) * width * height);
    }
    delete[] new_im;
    return true;
}


struct Point2D {
    double height = 0.0;
    double width = 0.0;
};

/**
 * @brief 图像轮廓跟踪（非0点） 根据“探测准则“
 * 分8方向 45°一个 从左上开始顺时针 搜到下一个逆时针90° 直到返回开始点
 * @todo 解决跟踪至边界越界出错的问题
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 */
template<typename T>
bool Trace(T *im, size_t width, size_t height, size_t slice) {
    if (!im) return false;
    T *new_im = nullptr;
    try {
        new_im = new T[width * height];
    }
    catch (std::bad_alloc) {
        std::cout << "Failed to alloc memory!\n";
        return false;
    }

    // 8方向
    int directions[8][2] = {{-1, -1},
                            {0,  -1},
                            {1,  -1},
                            {1,  0},
                            {1,  1},
                            {0,  1},
                            {-1, 1},
                            {-1, 0}};

    bool findStartPt = false;
    Point2D startPt;
    size_t p = 0;
    T max = std::numeric_limits<T>::max();
    for (size_t k = 0; k < slice; ++k) {
        memset(new_im, 0, sizeof(T) * width * height);
        p = k * width * height;

        findStartPt = false;
        // 先找最左上方的边界点（非0点）
        for (size_t i = 1; i < height - 1 && !findStartPt; ++i) {
            for (size_t j = 1; j < width - 1 && !findStartPt; ++j) {
                if (im[p + i * width + j] > 0) {
                    findStartPt = true;
                    startPt.height = i;
                    startPt.width = j;
                    new_im[i * width + height] = max;
                }
            }
        }
        // 若此slice上无黑点 直接下一个slice
        if (!findStartPt) continue;

        // 初始方向为左上
        int direct = 0;
        Point2D currentPt;
        currentPt.height = startPt.height;
        currentPt.width = startPt.width;
        findStartPt = false;
        bool findBlackPt = false;
        while (!findStartPt) {
            findBlackPt = false;
            while (!findBlackPt) {
                size_t t = p + (currentPt.height + directions[direct][1]) * width +
                           currentPt.width + directions[direct][0];
                // 如果搜索方向上出现白点
                if (im[t] > 0) {
                    findBlackPt = true;
                    // 当前点移到搜索到的点上
                    currentPt.height += directions[direct][1];
                    currentPt.width += directions[direct][0];
                    // 判断是否回到最初搜索点上
                    if (currentPt.height == startPt.height && currentPt.width == startPt.width)
                        findStartPt = true;
                    // 设置目标图像当前点为白色
                    t = currentPt.height * width + currentPt.width;
                    new_im[t] = max;
                    // 扫描方向逆时针旋转两格(90°)
                    direct -= 2;
                    if (direct == -2) direct = 6;
                    else if (direct == -1) direct = 7;
                } else {  //搜索方向无白色 顺时针1格(45°)
                    ++direct;
                    if (direct == 8) direct = 0;
                }
            }
        }

        memcpy(im + p, new_im, sizeof(T) * width * height);
    }
    delete[] new_im;
    return true;
}

#endif //DIP_EDGECONTOUR_DETECT_H
