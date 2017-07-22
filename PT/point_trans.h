// Program: DIP
// FileName:point_trans.h
// Author:  Lichun Zhang
// Date:    2017/4/23 下午4:19
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#ifndef DIP_POINT_TRANS_H
#define DIP_POINT_TRANS_H

#include <cstddef>
#include <limits>
#include <iostream>

//#include <map>

/**
 * @brief 阈值变换 低于阈值变为0，高于阈值均为最大值
 * @tparam T 图像数据类型
 * @param im 图像指针
 * @param width  图像宽度
 * @param height 图像高度
 * @param slice  图像切片数
 * @param threshold 阈值
 * @return 操作是否成功
 */
template<typename T>
bool ThresholdTrans(T *im, size_t width, size_t height, size_t slice, int threshold) {
    if (!im) return false;
    T vmax = std::numeric_limits<T>::max();
    for (size_t k = 0; k < slice; ++k) {
        size_t p = k * width * height;
        size_t t = 0;
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < width; ++j) {
                t = p + i * width + j;
                if (im[t] < threshold) im[t] = 0;
                else im[t] = vmax;
            }
        }
    }
    return true;
}


/**
 * @brief 灰度的窗口变换
 * @note x为原像素值, 变换后的f(x)= 0 (x<lowTh), x (lowTh=< x <=upTh), 最大值(x>upTh)
 * @tparam T 图像数据类型
 * @param im 图像指针
 * @param width  图像宽度
 * @param height 图像高度
 * @param slice  图像切片数
 * @param lowTh 低阈值 灰度低于阈值的像素点全为0
 * @param upTh 高阈值 灰度高于阈值的像素点全为最大值
 * @return 操作是否成功
 */
template<typename T>
bool WindowTrans(T *im, size_t width, size_t height, size_t slice,
                 int lowTh, int upTh) {
    if (!im) return false;
    size_t p0 = 0, t = 0;
    T max_val = std::numeric_limits<T>::max();
    for (int k = 0; k < slice; ++k) {
        p0 = k * width * height;
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                t = p0 + i * width + j;
                if (im[t] < lowTh) im[t] = 0;
                else if (im[t] > upTh) im[t] = max_val;

            }
        }
    }
    return true;
}

/**
 * @brief 对源图像进行灰度拉伸
 * @note 分段函数 映射调整源图像灰度值
 * @tparam T 图像数据类型
 * @param im 图像指针
 * @param width  图像宽度
 * @param height 图像高度
 * @param slice  图像切片数
 * @param x1 灰度变换图上第1个点x坐标(原灰度值1)
 * @param y1 灰度变换图上第1个点y坐标(变换后的灰度值1)
 * @param x2 灰度变换图上第2个点x坐标(原灰度值2)
 * @param y2 灰度变换图上第2个点y坐标(变换后的灰度值2)
 * @return 操作是否成功
 */
template<typename T>
bool GrayStretch(T *im, size_t width, size_t height, size_t slice,
                 int x1, int y1, int x2, int y2) {
    if (!im) return false;
    T *map = nullptr;   //灰度值映射表
    T max_val = std::numeric_limits<T>::max();
    try {
        map = new T[max_val + 1];
        memset(map, 0, sizeof(T) * (max_val + 1));
    }
    catch (std::bad_alloc) {
        std::cout << "Failed to allocate memory!\n";
        return false;
    }

    size_t p0 = 0;
    // 分段函数第一段变换
    for (int i = 0; i <= x1; ++i)
        map[i] = (x1 > 0) ? (y1 * i) / x1 : 0;  //判断x1是否大于0(防止分母为0)

    // 分段函数第二段变换
    for (int i = x1 + 1; i <= x2; ++i)
        map[i] = (x2 != x1) ? (y1 + (y2 - y1) * (i - x1) / (x2 - x1))   //防止分母为0
                            : y1;
    // 分段函数第三段变换
    for (int i = x2 + 1; i < max_val; ++i)
        map[i] = y2 + (max_val - y2) * (i - x2) / (max_val - x2);
    map[max_val] = max_val;

    // 按照映射表映射
    for (int k = 0; k < slice; ++k) {
        p0 = k * width * height;
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j)
                im[p0 + i * height + j] = map[im[p0 + i * height + j]];
        }
    }
    delete[] map;
    return true;
}

/**
 * @brief 直方图均衡化
 * @tparam T 图像数据类型
 * @param im 图像指针
 * @param width  图像宽度
 * @param height 图像高度
 * @param slice  图像切片数
 * @return 是否操作成功
 */
template<class T>
bool HisEqualize(T *im, size_t width, size_t height, size_t slice) {

    if (!im || width <= 0 || height <= 0 || slice <= 0)
        return false;
    long size = width * height * slice;

    //统计灰度级范围
    T gray_floor = im[0], gray_roof = im[0];
    for (long i = 1; i < size; ++i) {
        if (im[i] < gray_floor) gray_floor = im[i];
        else if (im[i] > gray_roof) gray_roof = im[i];
    }
    long range = gray_roof - gray_floor + 1;
    long *value_count = new long[range];
    T *value_map = new T[range];
    auto max = std::numeric_limits<T>::max();
    for (int i = 0; i < range; ++i) {
        value_count[i] = 0;
        value_map[i] = 0;
    }

    //统计各灰度级的像素个数
    for (long i = 0; i < size; ++i)
        ++value_count[im[i] - gray_floor];

    //计算直方图均衡化的灰度映射表
    long count = 0;
    for (int i = 0; i < range; ++i) {
        count += value_count[i];
        auto value = (count * range / size + gray_floor + 0.5);
        if (value >= max) value = max;
        value_map[i] = (T) value;
    }

    //对图像像素设值（灰度映射表）
    for (long i = 0; i < size; ++i)
        im[i] = value_map[im[i] - gray_floor];

//    for (int i = 0; i < range; ++i) {
//        printf("%d: %d\n", i, value_map[i]);
//    std::cout << i << ": " << value_map[i] << std::endl;
//    }

    delete[] value_count;
    delete[] value_map;

    return true;
}

#endif //DIP_POINT_TRANS_H
