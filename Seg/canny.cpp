// Program: DIP
// FileName:canny.cpp
// Author:  Lichun Zhang
// Date:    2017/7/19 下午5:53
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#include "canny.h"

/**
 * @brief 生成一维高斯数据
 * @note 此高斯函数均值为0, 6*sigma距离 最后归一化
 * @param sigma 高斯函数标准差
 * @param pdKernel 指向高斯数组的指针（输出） 需要由客户端释放内存
 * @param windowSize 数据的长度（输出） 取[-3*sigma, 3*sigma]
 */
void GetGauss(double sigma, double *pdKernel, int &windowSize) {
    windowSize = 1 + 2 * ceil(3 * sigma);   //数组长度
    int center = windowSize / 2;
    if (pdKernel) delete[] pdKernel;
    pdKernel = new double[windowSize];
    double dis = 0; //距离中心点距离
    double sum = 0; //和 用于归一化
    // 生成高斯数据
    for (int i = 0; i < windowSize; ++i) {
        dis = (double) (i - center);
        pdKernel[i] = exp(-dis * dis / (2 * sigma * sigma)) / (sqrt(2 * M_PI) * sigma);
        sum += pdKernel[i];
    }
    // 归一化
    for (int i = 0; i < windowSize; ++i)
        pdKernel[i] /= sum;
}

/**
 * @brief Canny操作3: 抑制局部像素非最大梯度点
 * @note 在Canny算子论文中 非最大值抑制是在0 45 90 135四个梯度方向上进行 判断当前点是否是邻域内最大的
 * 但这样的简化方法无法达到最好效果 自然图像中梯度方向不一定是沿这四个方向 亚像素点 不存在 因此需要进行插值
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param grad 梯度的数组(输入)
 * @param gradX x方向的导数数组(输入)
 * @param gradY y方向的导数数组(输入)
 * @return 包含边界点的数组(输出)
 */
bool NonmaxSuppress(int *grad, int *gradX, int *gradY, unsigned char *im_result, int width, int height, int slice) {
    if (!grad || !gradX || !gradY) return false;
    if (im_result) delete[] im_result;
    try {
        im_result = new unsigned char[slice * width * height];
    }
    catch (std::bad_alloc) {
        std::cout << "Failed to allocate memory!\n";
        return false;
    }
    memset(im_result, 0, sizeof(unsigned char) * width * height * slice);
    int p0 = 0, t = 0;  //临时变量 点位置
    int gx = 0, gy = 0; //临时变量 当前点x方向和y方向的导数值
    double temp = 0.0, temp1 = 0.0, temp2 = 0.0;    //当前点梯度 以及梯度方向上紧挨的两个邻域点(插值求取)
    int g1 = 0, g2 = 0, g3 = 0, g4 = 0; //临时变量 用来插值求取梯度方向上邻域点的
    double weight = 0.0;    //插值比例 (梯度斜率绝对值)

    // 图像边缘为不可能的边界点
    for (int k = 0; k < slice; ++k) {
        p0 = k * width * height;
        for (int j = 0; j < width; ++j) {
            im_result[p0 + j] = 0;
            im_result[p0 + (height - 1) * width + j] = 0;
        }
        for (int i = 0; i < height; ++i) {
            im_result[p0 + i * width] = 0;
            im_result[p0 + (i + 1) * width - 1] = 0;
        }

        for (int i = 1; i < height - 1; ++i) {
            for (int j = 1; j < width - 1; ++j) {
                t = p0 + i * width + j;
                // 若当前像素的梯度值为0，则不可能是边界点
                if (grad[t] == 0) {
                    im_result[t] = 0;
                    continue;
                }
                // 接下来为主要操作为插值求取梯度方向上紧挨的两个点
                temp = grad[t]; //当前像素梯度幅值
                gx = gradX[t];  //x方向导数
                gy = gradY[t];  //y方向导数

                // 若y方向导数分量比x方向大.
                if (abs(gy) > abs(gx)) {
                    weight = abs(gx) / abs(gy);
                    g2 = grad[t - width];
                    g4 = grad[t + width];
                    // 若x,y方向导数符号一致
                    //  g1 g2
                    //     c
                    //     g4 g3
                    if (gx * gy > 0) {
                        g1 = grad[t - width - 1];
                        g3 = grad[t + width + 1];
                    }
                        //若符号不一致
                        //      g2 g1
                        //      c
                        //  g3  g4
                    else {
                        g1 = grad[t - width + 1];
                        g3 = grad[t + width - 1];
                    }
                }
                    //若x方向导数比y方向大
                else {
                    weight = abs(gy) / abs(gx);
                    g2 = grad[t - 1];
                    g4 = grad[t + 1];
                    // 若x,y方向导数符号一致
                    //  g1
                    //  g2 c g4
                    //       g3
                    if (gx * gy > 0) {
                        g1 = grad[t - 1 - width];
                        g3 = grad[t + 1 + width];
                    }
                        // 若符号不一致
                        //       g3
                        //  g2 c g4
                        //  g1
                    else {
                        g1 = grad[t - 1 + width];
                        g3 = grad[t + 1 - width];
                    }
                }

                // 利用g1-g4对梯度进行插值
                temp1 = weight * g1 + (1 - weight) * g2;
                temp2 = weight * g3 + (1 - weight) * g4;

                // 若当前像素点的梯度是局部的最大值
                // 该点可能是个边界点
                if (temp >= temp1 && temp >= temp2)
                    im_result[t] = 128;
                else
                    im_result[t] = 0;
            }
        }
    }
    return true;
}

