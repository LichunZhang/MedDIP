// Program: DIP
// FileName:segmentation.h
// Author:  Lichun Zhang
// Date:    2017/7/12 下午8:15
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#ifndef DIP_SEGMENTATION_H
#define DIP_SEGMENTATION_H


#include <cstddef>
#include <limits>
#include <cstring>
#include <edgecontour_detect.h>
#include <point_trans.h>

/**
 * @brief 并行边界分割 Robert
 * @note Robert算子+阈值分割
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param threshold 阈值
 * @return 操作是否成功
 */
template<typename T>
bool RobertsOperator(T *im, size_t width, size_t height, size_t slice, int threshold) {
    if (!Robert(im, width, height, slice)) return false;
    return ThresholdTrans(im, width, height, slice, threshold);
}

/**
 * @brief 并行边界分割 Sobel
 * @note Sobel算子+阈值分割
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param threshold 阈值
 * @return 操作是否成功
 */
template<typename T>
bool SobelOperator(T *im, size_t width, size_t height, size_t slice, int threshold) {
    if (!Sobel(im, width, height, slice)) return false;
    return ThresholdTrans(im, width, height, slice, threshold);
}

/**
 * @brief 并行边界分割 Prewitt算子
 * @note Prewitt算子+阈值分割
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param threshold 阈值
 * @return 操作是否成功
 */
template<typename T>
bool PrewittOperator(T *im, size_t width, size_t height, size_t slice, int threshold) {
    if (!Prewitt(im, width, height, slice)) return false;
    return ThresholdTrans(im, width, height, slice, threshold);
}

/**
 * @brief 并行边界 Laplace算子
 * @note Laplace算子+阈值分割
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param threshold 阈值
 * @return 操作是否成功
 */
template<typename T>
bool LaplacianOperator(T *im, size_t width, size_t height, size_t slice, int threshold) {
    if (!LaplaceSharpen(im, width, height, slice)) return false;
    return ThresholdTrans(im, width, height, slice, threshold);
}


/**
 * @brief 自适应阈值分割函数 阈值不固定。
 * @note 把图像分成若干子图像 计算每个子图像均值 将其作为阈值应用于对应子图像上
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param count 分成的子图像数量
 */
template<typename T>
bool RegionSegAdaptive(T *im, size_t width, size_t height, size_t slice, int count) {
    if (!im) return false;
    T max = std::numeric_limits<T>::max();
    size_t w = width / count;
    size_t h = height / count;

    long threshold = 0;
    size_t p0 = 0, p1 = 0;
    for (int k = 0; k < slice; ++k) {
        p0 = k * width * height;
        // 中心完整部分子图像阈值处理
        for (int i = 0; i < count - 1; ++i) {
            for (int j = 0; j < count - 1; ++j) {
                threshold = 0;
                p1 = (i * h) * width + j * w;   //每个子块开始地址

                // 每个子图像中的平均值
                for (int n = 0; n < h; ++n)
                    for (int m = 0; m < w; ++m)
                        threshold += im[p0 + p1 + n * width + m];
                threshold /= w * h;
//                std::cout << threshold << "\n";
                // 根据阈值局部二值化
                for (int n = 0; n < h; ++n) {
                    for (int m = 0; m < w; ++m)
                        im[p0 + p1 + n * width + m] =
                                (im[p0 + p1 + n * width + m] > threshold) ? max : 0;
                }
            }

        }

        // 行边缘
        for (int i = 0; i < count - 1; ++i) {
            threshold = 0;
            p1 = (i * h) * width;
            for (int n = 0; n < h; ++n)
                for (int m = (count - 1) * w; m < width; ++m)
                    threshold += im[p0 + p1 + n * width + m];
            threshold /= (width - count * w + w) * h;
//            std::cout << threshold << "\n";
            for (int n = 0; n < h; ++n) {
                for (int m = (count - 1) * w; m < width; ++m)
                    im[p0 + p1 + n * width + m] =
                            (im[p0 + p1 + n * width + m] > threshold) ? max : 0;
            }
        }

        // 列边缘
        for (int j = 0; j < count - 1; ++j) {
            threshold = 0;
            p1 = j * w;
            for (int n = (count - 1) * h; n < height; ++n)
                for (int m = 0; m < w; ++m)
                    threshold += im[p0 + n * width + p1 + m];

            threshold /= (height - count * h + h) * w;
//            std::cout << threshold << "\n";
            for (int n = (count - 1) * h; n < height; ++n) {
                for (int m = 0; m < w; ++m)
                    im[p0 + n * width + p1 + m] =
                            (im[p0 + n * width + p1 + m] > threshold) ? max : 0;
            }
        }

        // 右下角
        threshold = 0;
        for (int n = (count - 1) * h; n < height; ++n)
            for (int m = (count - 1) * w; m < width; ++m)
                threshold += im[p0 + n * width + m];
        threshold /= (height - count * h + h) * (width - count * w + w);
//        std::cout << threshold << "\n";
        for (int n = (count - 1) * h; n < height; ++n) {
            for (int m = (count - 1) * w; m < width; ++m)
                im[p0 + n * width + m] =
                        (im[p0 + n * width + m] > threshold) ? max : 0;
        }
    }
    return true;
}


#endif //DIP_SEGMENTATION_H
