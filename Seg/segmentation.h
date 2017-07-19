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
bool RobertsSeg(T *im, size_t width, size_t height, size_t slice, int threshold) {
    if (!RobertOperator(im, width, height, slice)) return false;
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
bool SobelSeg(T *im, size_t width, size_t height, size_t slice, int threshold) {
    if (!SobelOperator(im, width, height, slice)) return false;
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
bool PrewittSeg(T *im, size_t width, size_t height, size_t slice, int threshold) {
    if (!PrewittOperator(im, width, height, slice)) return false;
    return ThresholdTrans(im, width, height, slice, threshold);
}

/**
 * @brief 并行边界分割 Laplace算子
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
bool LaplacianSeg(T *im, size_t width, size_t height, size_t slice, int threshold) {
    if (!LaplaceSharpen(im, width, height, slice)) return false;
    return ThresholdTrans(im, width, height, slice, threshold);
}

/**
 * @brief 串行边界分割 实现边界跟踪 (起始点 搜索准则 终止条件)
 * @note 将8领域中梯度最大的点作为边界，同时作为下个搜索起始点 当梯度小于某个阈值时搜索停止
 * @attention 搜索顺序很有讲究 依次为左下、左、左上、下、上、右下、右、右上
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param threshold 梯度阈值
 * @return 操作是否成功
 */
template<typename T>
bool EdgeTrack(T *im, size_t width, size_t height, size_t slice, int threshold) {
    if (!im) return false;
    // 为存储边界图像开辟内存空间
    T *new_im = nullptr;
    try {
        new_im = new T[width * height];
    }
    catch (std::bad_alloc) {
        std::cout << "Failed to alloc memory!\n";
        return false;
    }
    T mv = std::numeric_limits<T>::max();
    // 8邻域遍历的方向（必须）
    // 依次为左下、左、左上、下、上、右下、右、右上
    static int dirctx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    static int dircty[] = {1, 0, -1, 1, -1, 1, 0, -1};
    int p0 = 0, p1 = 0;
    int mx = 0, my = 0;  //最大梯度点所在坐标
    // Roberts算子求梯度 此时源图数据已变为梯度数值
    RobertOperator(im, width, height, slice);
    for (int k = 0; k < slice; ++k) {
        p0 = k * width * height;
        memset(new_im, 0, sizeof(T) * width * height);
        // 求出最大梯度点和值
        double max_grad = 0.0;
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                if (im[p0 + i * width + j] > max_grad) {
                    max_grad = im[p0 + i * width + j];
                    my = i;
                    mx = j;
                }
            }
        }
        int current_x = 0, current_y = 0;   //8领域中标记坐标
        int cmx = 0, cmy = 0;
        // 当满足阈值条件时 遍历跟踪边界
        while (max_grad > threshold) {
            new_im[my * width + mx] = mv;
//            std::cout << "(" << mx + 1 << ", " << my + 1 << "): " << max_grad << "\n";
            // 最大梯度点置0
            max_grad = 0.0;
            // 8领域找最大梯度点
            for (int i = 0; i < 8; ++i) {
                current_x = mx + dirctx[i];
                current_y = my + dircty[i];
                p1 = current_y * width + current_x;
                // 防止越界
                if (current_x >= 0 && current_x < width &&
                    current_y >= 0 && current_y < height) {
                    // 若大于此前最大梯度值 且并非边界
                    if ((im[p0 + p1] > max_grad) &&
                        (new_im[p1] == 0)) {
                        max_grad = im[p0 + p1];
                        cmx = current_x;
                        cmy = current_y;
                    }
                }
            }
            mx = cmx;
            my = cmy;
        }
        memcpy(im + p0, new_im, sizeof(T) * width * height);
    }

    delete[] new_im;
    return true;
}

/**
 * @brief 并行区域分割 自适应阈值分割函数 阈值不固定。
 * @note 把图像分成若干子图像 计算每个子图像均值 将其作为阈值应用于对应子图像上
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param count 分成的子图像数量
 */
template<typename T>
bool RegionAdaptiveSeg(T *im, size_t width, size_t height, size_t slice, int count) {
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


/**
 * @brief 串行区域分割 区域生长
 * @note 3个问题 种子点选取：输入 生长准则：四邻域像素灰度差小于阈值 终止条件：堆栈空
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param pos_x 种子点坐标x
 * @param pos_y 种子点坐标y
 * @param threshold 邻域点与种子点像素差阈值
 * @return 操作是否成功
 */
template<typename T>
bool RegionGrow(T *im, size_t width, size_t height, size_t slice,
                size_t pos_x, size_t pos_y,
                int threshold) {
    if (!im) return false;
    T *grow_region = nullptr;
    try {
        grow_region = new T[width * height];
    }
    catch (std::bad_alloc) {
        std::cout << "Failed to alloc memory!\n";
        return false;
    }
    std::stack<Seed> seeds;
    int current_x = 0, current_y = 0;
    T value = 0, max_value = std::numeric_limits<T>::max();
    size_t p0 = 0, p1 = 0, t = 0;
    for (size_t k = 0; k < slice; ++k) {
        memset(grow_region, 0, sizeof(T) * width * height);
        p0 = k * width * height;
        seeds.push(Seed(pos_y, pos_x));
        grow_region[pos_y * width + pos_x] = max_value;
        value = im[p0 + pos_y * width + pos_x];
        while (!seeds.empty()) {
            // 取出种子
            current_y = seeds.top().height;
            current_x = seeds.top().width;
            seeds.pop();
            p1 = current_y * width + current_x;
            t = p0 + p1;
            // 判断上下4领域四点与种子点像素差是否小于阈值，若是则压入堆栈，
            // 将生长区域对应点标记为1 注意防止越界
            // 左
            if (current_x > 0 && grow_region[p1 - 1] == 0 &&
                std::abs(value - im[t - 1]) < threshold) {
                seeds.push(Seed(current_y, current_x - 1));
                grow_region[p1 - 1] = max_value;
            }
            // 右
            if (current_x < width - 1 && grow_region[p1 + 1] == 0 &&
                std::abs(value - im[t + 1]) < threshold) {
                seeds.push(Seed(current_y, current_x + 1));
                grow_region[p1 + 1] = max_value;
            }
            // 上
            if (current_y > 0 && grow_region[p1 - width] == 0 &&
                std::abs(value - im[t - width]) < threshold) {
                seeds.push(Seed(current_y - 1, current_x));
                grow_region[p1 - width] = max_value;
            }
            // 下
            if (current_y < height - 1 && grow_region[p1 + width] == 0 &&
                std::abs(value - im[t + width]) < threshold) {
                seeds.push(Seed(current_y + 1, current_x));
                grow_region[p1 + width] = max_value;
            }
        }
        memcpy(im + p0, grow_region, sizeof(T) * width * height);
    }
    delete[] grow_region;
    return true;
}




#endif //DIP_SEGMENTATION_H
