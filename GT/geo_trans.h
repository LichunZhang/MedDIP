// Program: DIP
// FileName:geotrans.h
// Author:  Lichun Zhang
// Date:    2017/4/18 下午1:06
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#ifndef DIP_GEOTRANS_H
#define DIP_GEOTRANS_H

#include <cstddef>
#include <cstring>
#include <climits>
//#include <limits>

/**!
 * @brief 平移图像 基于目标图像源图像的像素点位置关系 逐点运算
 * x1 = x0 + offsetX, y1 = y0 + offsetY
 * 不改变图像大小 移出部分被截去 空白部分用黑色(0)填充
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param offsetX x轴平移量(像素)
 * @param offsetY y轴平移量(像素)
 * @return 平移操作成功与否
 */
template<typename T>
bool Translation(T *im, size_t width, size_t height, size_t slice,
                 int offsetX, int offsetY) {
    if (!im) return false;
    T *new_im = new T[width * height];  // 开辟内存，保存新图像
    if (!new_im) return false;
    T *lpDst = nullptr;
    int i0 = 0, j0 = 0;  // 像素在源图中的坐标
    for (size_t k = 0; k < slice; ++k) {
        int p = k * width * height;
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                lpDst = &new_im[i * width + j];
                // 计算改像素在源图像中的位置
                i0 = i - offsetY;
                j0 = j - offsetX;
                // 判断是否在范围内
                if ((j0 >= 0) && (j0 < width) && (i0 >= 0) && (i0 < height))
                    *lpDst = im[p + i0 * width + j0];
                else
                    *lpDst = 0; // 白色为:std::numeric_limits<T>::max()
            }
        }
        memcpy(&im[p], new_im, sizeof(T) * width * height);
    }
    delete[] new_im;
    return true;
}

/**!
 * @brief 平移图像 基于变换后的矩形区域位置与源图矩形区域位置关系 逐行运算
 * 不改变图像大小 移出部分被截去 空白部分用黑色(0)填充
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param offsetX x轴平移量(像素)
 * @param offsetY y轴平移量(像素)
 * @return 平移操作成功与否
 */
template<typename T>
bool Translation2(T *im, size_t width, size_t height, size_t slice,
                  int offsetX, int offsetY) {
    if (!im) return false;
    T *new_im = new T[width * height];
    if (!new_im) return false;

    // 分别是源图和新图中有图区域rect(矩形)的四个顶点
    int left_src = 0, right_src = width - 1, top_src = 0, buttom_src = height - 1;
    int left_dst = 0, right_dst = 0, top_dst = 0, buttom_dst = 0;
    bool visible = true;    // 指明图像是否可见。false表被全部移去，不可见

    if (offsetX <= int(-width)) {    // x轴全部移出
        visible = false;        // 不可见
    } else if (offsetX <= 0) {
        left_dst = 0;               // 有图区域左上角x坐标
        right_dst = width + offsetX;// 有图区域右下角x坐标
    } else if (offsetX < width) {
        left_dst = offsetX;
        right_dst = width;
    } else {
        visible = false;
    }

    left_src = left_dst - offsetX;
    right_src = right_dst - offsetX;

    if (offsetY <= int(-height)) {    // x轴全部移出
        visible = false;        // 不可见
    } else if (offsetY <= 0) {
        top_dst = 0;               // 有图区域左上角x坐标
        buttom_dst = height + offsetY;// 有图区域右下角x坐标
    } else if (offsetY < height) {
        top_dst = offsetY;
        buttom_dst = height;
    } else {
        visible = false;
    }

    top_src = top_dst - offsetY;
    buttom_src = buttom_dst - offsetY;
    size_t h = buttom_src - top_src;
    size_t w = right_src - left_src;
    if (visible) {
        for (size_t k = 0; k < slice; ++k) {
            memset(new_im, 0, sizeof(T) * width * height);
            int p = k * width * height;
            for (size_t i = 0; i < h; ++i) {
                memcpy(&new_im[(top_dst + i) * width + left_dst],
                       &im[p + (top_src + i) * width + left_src],
                       sizeof(T) * w);
            }
            memcpy(&im[p], new_im, sizeof(T) * width * height);
        }
    } else {
        memset(new_im, 0, sizeof(T) * width * height);
        for (size_t k = 0; k < slice; ++k) {
            memcpy(&im[k * width * height], new_im, sizeof(T) * width * height);
        }
    }
    return true;

}

/**!
 * @brief 镜像变换。根据新图与源图点的关系。可指定镜像方向为水平/垂直。
 * (水平)x1 = width - x0, y1= height; (垂直)x1 = x0, y1 = height - y0
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param drt 镜像方向 true为水平 false为垂直
 * @return 操作是否成功
 */
template<typename T>
bool Mirror(T *im, size_t width, size_t height, size_t slice, bool drt) {
    if (!im) return false;
    T *new_im = new T[width * height];
    if (!new_im) return false;
    size_t i0 = 0, j0 = 0;
    int p0 = 0, p1 = 0;
    // 判断方向 true为水平 false为垂直
    for (size_t k = 0; k < slice; ++k) {
        p0 = k * width * height;
        for (size_t i = 0; i < height; ++i) {
            p1 = i * width;
            for (size_t j = 0; j < width; ++j) {
                if (drt) {
                    i0 = i;
                    j0 = width - j;
                } else {
                    i0 = height - i;
                    j0 = j;
                }
                new_im[p1 + j] = im[p0 + i0 * width + j0];
            }
        }
        memcpy(&im[p0], new_im, sizeof(T) * width * height);
    }
    return true;
}

/**!
 * @brief 镜像变换。根据位置关系。可指定镜像方向为水平/垂直。
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param drt 镜像方向 true为水平 false为垂直
 * @return 操作是否成功
 */
template<typename T>
bool Mirror2(T *im, size_t width, size_t height, size_t slice, bool drt) {
    if (!im) return false;
    int p0 = 0, p1 = 0;
    // 水平镜像
    if (drt) {
        for (int k = 0; k < slice; ++k) {
            p0 = k * width * height;
            for (size_t i = 0; i < height; ++i) {
                p1 = i * width;
                for (size_t j = 0; j < width / 2; ++j) {
                    // 第i行第j个像素
                    T temp = im[p0 + p1 + j];
                    // 第i行倒数第j个像素
                    im[p0 + p1 + j] = im[p0 + width * (i + 1) - (j + 1)];
                    im[p0 + width * (i + 1) - (j + 1)] = temp;
                }
            }
        }   //垂直镜像
    } else {
        T *new_im = new T[width];
        if (!new_im) return false;
        for (size_t k = 0; k < slice; ++k) {
            p0 = k * width * height;
            for (size_t i = 0; i < height / 2; ++i) {
                // 第i行
                memcpy(new_im, &im[p0 + i * width], sizeof(T) * width);
                // 倒数第i行
                memcpy(&im[p0 + i * width], &im[p0 + (height - 1 - i) * width], sizeof(T) * width);
                memcpy(&im[p0 + (height - 1 - i)*width], new_im, sizeof(T) * width);
            }
        }
        delete[] new_im;
    }
    return true;
}

void Rotate(void *im, size_t width, size_t height, size_t slice, int angle);

#endif //DIP_GEOTRANS_H
