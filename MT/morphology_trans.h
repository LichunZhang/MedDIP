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
 * @brief 图形腐蚀。输入输出为二值图。
 * 结构元素为水平方向或垂直方向的3个点，中间点位于原点；或者自定义3*3结构元素。目标图像为二值图.
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
    delete[] new_im;
    return true;
}

/**
 * @brief 图形膨胀。输入输出为二值图。
 * 结构元素为水平方向或垂直方向的3个点，中间点位于原点；或者自定义3*3结构元素。目标图像为二值图.
 * @tparam T 图像数据类型
 * @param im 图像数据指针
 * @param width 图像宽度
 * @param height 图像高度
 * @param slice 图像切片数
 * @param mode 腐蚀方式，0为水平方向，1为垂直方向，2为自定义元素
 * @param structure 自定义的结构元素
 * @param size 结构元素大小 奇数 长高一样
 * @return 操作是否成功
 */
template<typename T>
bool Dilation(T *im, size_t width, size_t height, size_t slice, int mode,
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
                    // 目标图像当前点先赋为白色
                    new_im[p1] = vmax;
                    // 若源图当前点或左右有一个点是黑色,则将目标图像中当前点赋黑色
                    if (im[t] == 0 || im[t - 1] == 0 || im[t + 1] == 0)
                        new_im[p1] = 0;
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
                    // 目标图像当前点先赋为白色
                    new_im[p1] = vmax;
                    // 若源图当前点或上下有一个点是黑色,则将目标图像中当前点赋黑色
                    if (im[t] == 0 || im[t - width] == 0 || im[t + width] == 0)
                        new_im[p1] = 0;
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
                    // 目标图像当前点先赋为白色
                    new_im[p1] = vmax;

                    // 若源图对应结构元素中为黑色的那些点中有一个是黑色
                    // 则将目标图像中当前点赋黑色
                    for (size_t m = 0; m < size; ++m) {
                        for (size_t n = 0; n < size; ++n) {
                            if (structure[m][n] == 0)
                                continue;
                            size_t t1 = t + (m - size / 2) * width + (n - size / 2);
                            if (im[t1] == 0) {
                                new_im[p1] = 0;
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
    delete[] new_im;
    return true;
}

/**
 * @brief 图形开运算 输入输出为二值图 先腐蚀 后膨胀
 * @tparam T 图像数据类型
 * @param im 图像数据指针
 * @param width 图像宽度
 * @param height 图像高度
 * @param slice 图像切片数
 * @param mode 腐蚀方式，0为水平方向，1为垂直方向，2为自定义元素
 * @param structure 自定义的结构元素
 * @param size 结构元素大小 奇数 长高一样
 * @return 操作是否成功
 */
template<typename T>
bool Open(T *im, size_t width, size_t height, size_t slice, int mode,
          bool **structure, size_t size) {
    if (Erosion(im, width, height, slice, mode, structure, size))
        return Dilation(im, width, height, slice, mode, structure, size);
    else
        return false;
}

/**
 * @brief 图形闭运算 输入输出为二值图 先膨胀 后腐蚀
 * @tparam T 图像数据类型
 * @param im 图像数据指针
 * @param width 图像宽度
 * @param height 图像高度
 * @param slice 图像切片数
 * @param mode 腐蚀方式，0为水平方向，1为垂直方向，2为自定义元素
 * @param structure 自定义的结构元素
 * @param size 结构元素大小 奇数 长高一样
 * @return 操作是否成功
 */
template<typename T>
bool Close(T *im, size_t width, size_t height, size_t slice, int mode,
           bool **structure, size_t size) {
    if (Dilation(im, width, height, slice, mode, structure, size))
        return Erosion(im, width, height, slice, mode, structure, size);
    else
        return false;
}

/**
 * @brief 图像细化 输入输出为二值图
 * 奇怪的细化，和zhang细化方法很相似，但细节处不同
 * @tparam T 图像数据类型
 * @param im 图像数据指针
 * @param width 图像宽度
 * @param height 图像高度
 * @param slice 图像切片数
 * @return 操作是否成功
 */
template<typename T>
bool Thining(T *im, size_t width, size_t height, size_t slice) {
    if (!im) return false;
    T *new_im = nullptr;
    try {
        new_im = new T[width * height];
    }
    catch (std::bad_alloc) {
        std::cout << "Failed to alloc memory!\n";
        return false;
    }

    T vmax = std::numeric_limits<T>::max();
    memset(new_im, vmax, sizeof(T) * width * height);

    bool neighbour[5][5] = {0};
    unsigned char ncount = 0;

    bool condition1 = false, condition2 = false, condition3 = false, condition4 = false;
    for (size_t k = 0; k < slice; ++k) {
        bool modified = true;
        size_t p0 = k * width * height;
        while (modified) {
            modified = false;
            memset(new_im, vmax, sizeof(T) * width * height);
            // 结构元素为5*5 防止越界
            for (size_t i = 2; i < height - 2; ++i) {
                for (size_t j = 2; j < width - 2; ++j) {
                    condition1 = condition2 = condition3 = condition4 = false;

                    // 判断是否为二值图
                    size_t p1 = i * width + j;
                    size_t t = p0 + p1;
                    if (im[t] != 0 && im[t] != vmax)
                        return false;
                    else if (im[t] == vmax)
                        continue;

                    // 逐个判断4个条件

                    // 条件1：8邻域内黑色点个数 2<= ncount <=6
                    // 获得当前点相邻的5*5区域内像素值,白色用0代表，黑色为1
                    for (int m = 0; m < 5; ++m)
                        for (int n = 0; n < 5; ++n)
                            neighbour[m][n] = !im[t + (m - 2) * width + (n - 2)];
                    // 判断当前点8邻域内黑色点的个数
                    for (int m = 1; m <= 3; ++m) {
                        for (int n = 1; n <= 3; ++n)
                            ncount += neighbour[m][n];
                    }
                    if (ncount >= 2 && ncount <= 6) condition1 = true;

                    // 条件2:以p2、p3……为序出现01的次数 Z0(p1)=1
                    // /1
                    // p3 p2 p9     /1
                    // p4 p1 p8
                    // p5 p6 p7
                    ncount = 0;
                    if (neighbour[1][2] == 0 && neighbour[1][1] == 1)
                        ++ncount;
                    if (neighbour[1][1] == 0 && neighbour[2][1] == 1)
                        ++ncount;
                    if (neighbour[2][1] == 0 && neighbour[3][1] == 1)
                        ++ncount;
                    if (neighbour[3][1] == 0 && neighbour[3][2] == 1)
                        ++ncount;
                    if (neighbour[3][2] == 0 && neighbour[3][3] == 1)
                        ++ncount;
                    if (neighbour[3][3] == 0 && neighbour[2][3] == 1)
                        ++ncount;
                    if (neighbour[2][3] == 0 && neighbour[1][3] == 1)
                        ++ncount;
                    if (neighbour[1][3] == 0 && neighbour[1][2] == 1)
                        ++ncount;
                    if (ncount == 1) condition2 = true;

                    // 条件3:判断p2*p4*p8=0 or Z0(p2)!=1
                    if (neighbour[1][2] * neighbour[2][1] * neighbour[2][3] == 0) {
                        condition3 = true;
                    } else {
                        ncount = 0;
                        if (neighbour[0][2] == 0 && neighbour[0][1] == 1)
                            ++ncount;
                        if (neighbour[0][1] == 0 && neighbour[1][1] == 1)
                            ++ncount;
                        if (neighbour[1][1] == 0 && neighbour[2][1] == 1)
                            ++ncount;
                        if (neighbour[2][1] == 0 && neighbour[2][2] == 1)
                            ++ncount;
                        if (neighbour[2][2] == 0 && neighbour[2][3] == 1)
                            ++ncount;
                        if (neighbour[2][3] == 0 && neighbour[1][3] == 1)
                            ++ncount;
                        if (neighbour[1][3] == 0 && neighbour[0][3] == 1)
                            ++ncount;
                        if (neighbour[0][3] == 0 && neighbour[0][2] == 1)
                            ++ncount;
                        if (ncount != 1) condition3 = true;
                    }

                    // 条件4：判断p2*p4*p6=0 or Z0(p4)!=1
                    if (neighbour[1][2] * neighbour[2][1] * neighbour[3][2] == 0) {
                        condition4 = true;
                    } else {
                        ncount = 0;
                        if (neighbour[1][1] == 0 && neighbour[1][0] == 1)
                            ++ncount;
                        if (neighbour[1][0] == 0 && neighbour[2][0] == 1)
                            ++ncount;
                        if (neighbour[2][0] == 0 && neighbour[3][0] == 1)
                            ++ncount;
                        if (neighbour[3][0] == 0 && neighbour[3][1] == 1)
                            ++ncount;
                        if (neighbour[3][1] == 0 && neighbour[3][2] == 1)
                            ++ncount;
                        if (neighbour[3][2] == 0 && neighbour[2][2] == 1)
                            ++ncount;
                        if (neighbour[2][2] == 0 && neighbour[1][2] == 1)
                            ++ncount;
                        if (neighbour[1][2] == 0 && neighbour[1][1] == 1)
                            ++ncount;
                        if (ncount != 1) condition4 = true;
                    }
                    // 若四个条件都满足，去除当前点
                    if (condition1 && condition2 && condition3 && condition4) {
                        new_im[p1] = vmax;
                        modified = true;
                    } else {
                        new_im[p1] = 0;
                    }
                }   //j
            }   //i
            memcpy(im + p0, new_im, sizeof(T) * width * height);
        }   //while
    }   //k
    delete[] new_im;
    return true;
}

#endif //DIP_MORPHOLOGY_TRANS_H
