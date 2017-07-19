// Program: DIP
// FileName:canny.h
// Author:  Lichun Zhang
// Date:    2017/7/19 下午5:53
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#ifndef DIP_CANNY_H
#define DIP_CANNY_H

#include <cstddef>
#include <new>
#include <iostream>
#include <cmath>

void GetGauss(double sigma, double *pdKernel, int &windowSize);

/**
 * @brief Canny操作1: 高斯平滑函数
 * @note 先产生一维高斯数组 然后对每个slice的x和y方向进行模板操作
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param sigma 高斯函数标准差
 * @return 操作是否成功
 */
template<typename T>
bool GaussianSmooth(T *im, size_t width, size_t height, size_t slice,
                    double sigma) {
    if (!im) return false;
    T *new_im = nullptr;
    try {
        new_im = new T[width * height];
    }
    catch (std::bad_alloc) {
        std::cout << "Failed to allocate memory!\n";
        return false;
    }
    double *kernel = nullptr;
    int wsize = 0;
    GetGauss(sigma, kernel, wsize);
    int halfLen = wsize / 2;
    double value = 0.0;
    size_t p0 = 0;
    for (int k = 0; k < slice; ++k) {
        p0 = k * width * height;
        memset(new_im, 0, sizeof(T) * width * height);

        // x方向滤波
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                value = 0.0;
                for (int n = -halfLen; n <= halfLen; ++n) {
                    if ((n + j) >= 0 && (n + j) < width)
                        value += im[p0 + i * width + j + n] * kernel[halfLen + n];
                }
                new_im[i * width + j] = value;
            }
        }

        // y方向滤波
        for (int j = 0; j < width; ++j) {
            for (int i = 0; i < height; ++i) {
                value = 0.0;
                for (int n = -halfLen; n <= halfLen; ++n) {
                    if ((i + n) >= 0 && (i + n) < height)
                        value += im[p0 + (i + n) * width + j] * kernel[halfLen + n];
                }
                new_im[i * width + j] = value;
            }
        }

        memcpy(im + p0, new_im, sizeof(T) * width * height);
    }
    delete[] kernel;
    delete[] new_im;
    return true;
}

/**
 * @brief 方向导数计算
 * @note 微分算子为[-1 0 1]和[-1 0 1]’（转置）
 * @tparam T 源图像数据类型
 * @param im 进过高斯滤波后的图像
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param gradX x方向导数(输出) 交由客户端释放
 * @param gradY y方向导数(输出) 交由客户端释放
 */
template<typename T>
bool DirGrad(T *im, int *gradX, int *gradY, int width, int height, int slice) {
    if (!im) return false;
    if (gradX) delete[] gradX;
    if (gradY) delete[] gradY;
    try {
        gradX = new int[slice * width * height];
        gradY = new int[slice * width * height];
    }
    catch (std::bad_alloc) {
        std::cout << "Failed to allocate memory!\n";
        return false;
    }

    size_t p0 = 0, p1 = 0;
    for (int k = 0; k < slice; ++k) {
        p0 = k * width * height;
        for (int i = 0; i < height; ++i) {
            p1 = i * width;
            for (int j = 0; j < width; ++j) {
                gradX[p0 + p1 + j] = im[p0 + p1 + std::min(width - 1, j + 1)] -
                                     im[p0 + p1 + std::max(0, j - 1)];
                gradY[p0 + p1 + j] = im[p0 + std::min(height - 1, i + 1) * width + j] -
                                     im[p0 + std::max(0, i - 1) * width + j];
            }
        }
    }
    return true;
}

/**
 * @brief Canny操作2: 梯度计算
 * @tparam T 源数据类型
 * @note 利用方向导数,采用二范数计算梯度
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param gradX x方向导数(输入)
 * @param gradY y方向导数(输入)
 * @param grad 梯度(输出) 交由客户端释放
 * @return 操作是否成功
 */
template<typename T>
bool GradMagnitude(int *gradX, int *gradY, int *grad, int width, int height, int slice) {
    if (!gradX || !gradY) return false;
    if (grad) delete[] grad;
    try {
        grad = new int[slice * width * height];
        memset(grad, 0, sizeof(int) * slice * height * width);
    }
    catch (std::bad_alloc) {
        std::cout << "Failed to allocate memory!\n";
        return false;
    }

    size_t p0 = 0, p1 = 0, t = 0;
    for (int k = 0; k < slice; ++k) {
        p0 = k * width * height;
        for (int i = 0; i < height; ++i) {
            p1 = i * width;
            for (int j = 0; j < width; ++j) {
                t = p0 + p1 + j;
                grad[t] = ceil(sqrt(gradX[t] * gradX[t] +
                                    gradY[t] * gradY[t]));
            }
        }
    }

}


bool NonmaxSuppress(int *grad, int *gradX, int *gradY, unsigned char *im_result,
                    int width, int height, int slice);

template<typename T>
bool Canny(T *im, T *result, int width, int height, int slice,
           double sigma, double rationLow, double rationHigh) {
    if (!im) return false;

}

#endif //DIP_CANNY_H
