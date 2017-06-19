// Program: DIP
// FileName:morphology_trans.h
// Author:  Lichun Zhang
// Date:    2017/6/18 上午11:25
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#ifndef DIP_MORPHOLOGY_TRANS_H
#define DIP_MORPHOLOGY_TRANS_H

#include <cstddef>
#include <new>
#include <iostream>


/**
 * @brief 图形腐蚀。结构元素为水平方向或垂直方向的3个点，中间点位于原点；或者自定义3*3结构元素。目标图像为而二值图.
 * @tparam T 图像数据类型
 * @param im 图像数据指针
 * @param width 图像宽度
 * @param height 图像高度
 * @param slice 图像切片数
 * @param mode 腐蚀方式，0为水平方向，1为垂直方向，2为自定义元素
 * @param structure 自定义的结构元素
 * @param size 结构元素大小 奇数 长高一样
 * @return 腐蚀操作是否成功
 */
template<typename T>
bool Erosion(T *im, size_t width, size_t height, size_t slice, int mode,
             bool **structure, size_t size) {
    if (!im) return false;
    T *new_im = nullptr;
    try {
        new_im = new T[width * height];
    }
    catch (std::bad_alloc) {
        std::cout << "Failed to alloc memory!\n";
        return false;
    }
    // 初始化分配内存 (白色)
    T vmax = std::numeric_limits<T>::max();
    memset(new_im, vmax, sizeof(T) * width * height);

    // 水平方向 1*3结构元素
    if (mode == 0) {
        for (size_t k = 0; k < slice; ++k) {
            size_t p0 = k * width * height;
            for (size_t i = 0; i < height; ++i) {
                for (size_t j = 1; j < width - 1; ++j) {    //防止越界 不处理最左和最右两边
                    // 判断是否为二值图
                    size_t p1 = i * width + j;
                    size_t t = p0 + p1;
                    if (im[t] != 0 && im[t] != vmax)
                        return false;
                    // 目标图像当前点先赋为黑色
                    new_im[p1] = 0;
                    // 若源图当前点或左右有一个点不是黑色,则将目标图像中当前点赋白色
                    if (im[t] == vmax || im[t - 1] == vmax || im[t + 1] == vmax)
                        new_im[p1] = vmax;
//                    for (auto n = 0; n < 3; ++n) {
//                        if (im[t + n - 1] == vmax) {
//                            new_im[p1] = vmax;
//                            break;
//                        }
//                    }
                }
            }
            memcpy(im + p0, new_im, sizeof(T) * width * height);
            memset(new_im, vmax, sizeof(T) * width * height);
        }
    } else if (mode == 1) {   //垂直方向 3*1
        for (size_t k = 0; k < slice; ++k) {
            size_t p0 = k * width * height;
            for (size_t i = 1; i < height - 1; ++i) {   //防止越界 不处理最上和最下两边
                for (size_t j = 0; j < width; ++j) {
                    // 判断是否为二值图
                    size_t p1 = i * width + j;
                    size_t t = p0 + p1;
                    if (im[t] != 0 && im[t] != vmax)
                        return false;
                    // 目标图像当前点先赋为黑色
                    new_im[p1] = 0;
                    // 若源图当前点或上下有一个点不是黑色,则将目标图像中当前点赋白色
                    if (im[t] == vmax || im[t - width] == vmax || im[t + width] == vmax)
                        new_im[p1] = vmax;
                }
            }
            memcpy(im + p0, new_im, sizeof(T) * width * height);
            memset(new_im, vmax, sizeof(T) * width * height);
        }
    } else if (mode == 2) {    //自定义方向
        if (!structure || !size || !(size % 2)) return false;
        for (size_t k = 0; k < slice; ++k) {
            size_t p0 = k * width * height;
            for (size_t i = size / 2; i < height - size / 2; ++i) {
                for (size_t j = size / 2; j < width - size / 2; ++j) {
                    // 判断是否为二值图
                    size_t p1 = i * width + j;
                    size_t t = p0 + p1;
                    if (im[t] != 0 && im[t] != vmax)
                        return false;
                    // 目标图像当前点先赋为黑色
                    new_im[p1] = 0;

                    // 若源图对应结构元素中为黑色的那些点中有一个不是黑色
                    // 则将目标图像中当前点赋白色
                    for (size_t m = 0; m < size; ++m) {
                        for (size_t n = 0; n < size; ++n) {
                            if (structure[m][n] == 0)
                                continue;
                            size_t t1 = t + (m - size / 2) * width + (n - size / 2);
                            if (im[t1] == vmax) {
                                new_im[p1] = vmax;
                                break;
                            }
                        }
                    }
                }
            }
            memcpy(im + p0, new_im, sizeof(T) * width * height);
            memset(new_im, vmax, sizeof(T) * width * height);
        }
    }
    return true;
}

#endif //DIP_MORPHOLOGY_TRANS_H
