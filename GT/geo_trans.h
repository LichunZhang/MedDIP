// Program: DIP
// FileName:geotrans.h
// Author:  Lichun Zhang
// Brief:   Image geometry transformations including translation,mirror,
//          transpose,zoom,rotate.
// Date:    2017/4/18 下午1:06
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#ifndef DIP_GEOTRANS_H
#define DIP_GEOTRANS_H

#include <cstddef>
#include <cstring>
#include <climits>
#include <cmath>

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
                memcpy(&im[p0 + (height - 1 - i) * width], new_im, sizeof(T) * width);
            }
        }
        delete[] new_im;
    }
    return true;
}

/**!
 * @brief 图像转置变换 利用位置关系 图像长宽会调换
 * new_im[y，x] = im[x,y]
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @return 是否操作成功
 */
template<typename T>
bool Transpose(T *im, size_t width, size_t height, size_t slice) {
    if (!im) return false;
    T *new_im = new T[width * height];
    if (!new_im) return false;
    for (size_t k = 0; k < slice; ++k) {
        int p0 = k * width * height;
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < width; ++j) {
                new_im[j * height + i] = im[p0 + i * width + j];
            }
        }
        memcpy(&im[p0], new_im, sizeof(T) * width * height);
    }
    delete[] new_im;
    return true;
}

/**!
 * @brief 图像缩放 利用源和新图映射关系 new[i,j] = im[i/rationx,J/rationY]
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param rationX
 * @param rationY
 * @return 返回缩放后的图像 大小有变化
 */
template<typename T>
T *Zoom(T *im, size_t width, size_t height, size_t slice,
        float rationX, float rationY) {
    if (!im) return nullptr;
    size_t new_w = width * rationX + 0.5;
    size_t new_h = height * rationY + 0.5;
    T *new_im = new T[new_w * new_h * slice];
    if (!new_im) return nullptr;
    for (size_t k = 0; k < slice; ++k) {
        int p0 = k * width * height, p1 = k * new_w * new_h;
        int i0 = 0, j0 = 0;
        for (size_t i = 0; i < new_h; ++i) {
            for (size_t j = 0; j < new_w; ++j) {
                i0 = i / rationY + 0.5;
                j0 = j / rationX + 0.5;
                if ((j0 >= 0) && (j0 < width) && (i0 >= 0) && (i0 < height))
                    new_im[p1 + i * new_w + j] = im[p0 + i0 * width + j0];
                else
                    new_im[p1 + i * new_w + j] = 0;
            }
        }
    }
    return new_im;
}

/// \brief 图像旋转(最近邻插值) 利用了新图与源图的映射关系 改变图像大小
/// \tparam T 源图像数据类型
/// \param im 源图像指针
/// \param width 源图像宽（像素）
/// \param height 源图像高（像素）
/// \param slice 源图像切片数
/// \param angle 旋转角度
/// \param new_w 新图像宽度（像素）
/// \param new_h 新图像高度（像素）
/// \return 新图像指针

template<typename T>
T *Rotate(T *im, size_t width, size_t height, size_t slice, int angle,
          size_t &new_w, size_t &new_h) {
    if (!im) return nullptr;

    double sin_angle = sin(angle);
    double cos_angle = cos(angle);

    // 原图4个角坐标(图像中心为坐标原点)
    double pt_src1X = -(double) (width - 1) / 2;
    double pt_src1Y = (double) (height - 1) / 2;
    double pt_src2X = (double) (width - 1) / 2;
    double pt_src2Y = (double) (height - 1) / 2;
    double pt_src3X = -(double) (width - 1) / 2;
    double pt_src3Y = -(double) (height - 1) / 2;
    double pt_src4X = (double) (width - 1) / 2;
    double pt_src4Y = -(double) (height - 1) / 2;

    // 新图4个角坐标(图像中心坐标原点)
    double pt_dst1X = cos_angle * pt_src1X + sin_angle * pt_src1Y;
    double pt_dst1Y = -sin_angle * pt_src1X + cos_angle * pt_src1Y;
    double pt_dst2X = cos_angle * pt_src2X + sin_angle * pt_src2Y;
    double pt_dst2Y = -sin_angle * pt_src2X + cos_angle * pt_src2Y;
    double pt_dst3X = cos_angle * pt_src3X + sin_angle * pt_src3Y;
    double pt_dst3Y = -sin_angle * pt_src3X + cos_angle * pt_src3Y;
    double pt_dst4X = cos_angle * pt_src4X + sin_angle * pt_src4Y;
    double pt_dst4Y = -sin_angle * pt_src4X + cos_angle * pt_src4Y;

    // 新图像宽度和高度
    new_w = fmax(fabs(pt_dst4X - pt_dst1X), fabs(pt_dst3X - pt_dst2X)) + 1.5;
    new_h = fmax(fabs(pt_dst4Y - pt_dst1Y), fabs(pt_dst3Y - pt_dst2Y)) + 1.5;

    T *new_im = new T[slice * new_w * new_h];
    if (!new_im) return nullptr;

    // 以图左上角为原点
    // x0=x1cosθ+y1sinθ-ccosθ-dsinθ+a
    // y0=-x1sinθ+y1cosθ+csinθ-dcosθ+b
    // a=(width-1)/2, b=(height-1)/2
    // c=(new_w-1)/2, d=(new_h-1)/2

    // f1=-cosθ-dsinθ+a --> x0=x1cosθ+y1sinθ+f1
    // f2=csinθ-dcosθ+b --> y0=-x1sinθ+y1cosθ+f2

    double f1 = double(-0.5 * (new_w - 1) * cos_angle -
                       0.5 * (new_h - 1) * sin_angle + 0.5 * (width - 1));
    double f2 = double(0.5 * (new_w - 1) * sin_angle -
                       0.5 * (new_h - 1) * cos_angle + 0.5 * (height - 1));
    // i0->y0,j0->x0
    int i0 = 0, j0 = 0;
    int p1 = new_w * new_h, p0 = width * height;
    for (size_t k = 0; k < slice; ++k) {
        for (size_t i = 0; i < new_h; ++i) {
            for (size_t j = 0; j < new_w; ++j) {
                i0 = -(double) j * sin_angle + (double) i * cos_angle + f2 + 0.5;
                j0 = (double) j * cos_angle + (double) i * sin_angle + f1 + 0.5;
                if ((i0 >= 0) && (i0 < height) && (j0 >= 0) && (j0 < width))
                    new_im[k * p1 + i * new_w + j] = im[k * p0 + i0 * width + j0];
                else
                    new_im[k * p1 + i * new_w + j] = 0;
            }
        }
    }
    return new_im;

}

/**
 * @brief 双线性插值
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param x 插值元素x坐标
 * @param y 插值元素y坐标
 * @return 插值计算结果
 */
template<typename T>
T BilinearInterpolation(T *im, size_t width, size_t height, size_t slice,
                        double x, double y, int z) {
    if ((x < 0) || (x >= width) || (y < 0) || (y >= height) || (z < 0) || (z >= slice)) return 0;
    // 计算4个最邻近插值元素的坐标
    size_t i1 = y;
//    size_t i2 = y + 1;
    size_t j1 = x;
//    size_t j2 = x + 1;

    auto p = z * width * height + i1 * width + j1;
    // 根据公式:f(x,y) = f(x,0)+y*[f(x,1)-f(x,0)]    (0<x<1,0<y<1)
    // 其中    f(x,0) = f(0,0)+ x*[f(1,0)-f(0,0)]
    //        f(x,1) = f(0,1) + x*[f(1,1)-f(0,1)]
    // 计算4个最近邻像素值
    T f1 = im[p] + (x - j1) * (im[p + width] - im[p]);   //f(x,0)
    T f2 = im[p + 1] + (x - j1) * (im[p + width + 1] - im[p + 1]);   //f(x,1)
    return f1 + (y - i1) * (f2 - f1);

}


/**
 * @brief 图像旋转2(双线性插值)
 * @tparam T 源图像数据类型
 * @param im 源图像指针
 * @param width 源图像宽度(像素)
 * @param height 源图像高度(像素)
 * @param slice 源图像切片数
 * @param angle 旋转角度
 * @param new_w 新图像宽度
 * @param new_h 新图像高度
 * @return 新图像指针
 */
template<typename T>
T *Rotate2(T *im, size_t width, size_t height, size_t slice, int angle,
           size_t &new_w, size_t &new_h) {
    if (!im) return nullptr;
    double sin_angle = sin(angle);
    double cos_angle = cos(angle);

    // 原图4个角坐标(图像中心为坐标原点)
    double pt_src1X = -(double) (width - 1) / 2;
    double pt_src1Y = (double) (height - 1) / 2;
    double pt_src2X = (double) (width - 1) / 2;
    double pt_src2Y = (double) (height - 1) / 2;
    double pt_src3X = -(double) (width - 1) / 2;
    double pt_src3Y = -(double) (height - 1) / 2;
    double pt_src4X = (double) (width - 1) / 2;
    double pt_src4Y = -(double) (height - 1) / 2;

    // 新图4个角坐标(图像中心坐标原点)
    double pt_dst1X = cos_angle * pt_src1X + sin_angle * pt_src1Y;
    double pt_dst1Y = -sin_angle * pt_src1X + cos_angle * pt_src1Y;
    double pt_dst2X = cos_angle * pt_src2X + sin_angle * pt_src2Y;
    double pt_dst2Y = -sin_angle * pt_src2X + cos_angle * pt_src2Y;
    double pt_dst3X = cos_angle * pt_src3X + sin_angle * pt_src3Y;
    double pt_dst3Y = -sin_angle * pt_src3X + cos_angle * pt_src3Y;
    double pt_dst4X = cos_angle * pt_src4X + sin_angle * pt_src4Y;
    double pt_dst4Y = -sin_angle * pt_src4X + cos_angle * pt_src4Y;

    // 新图像宽度和高度
    new_w = fmax(fabs(pt_dst4X - pt_dst1X), fabs(pt_dst3X - pt_dst2X)) + 1.5;
    new_h = fmax(fabs(pt_dst4Y - pt_dst1Y), fabs(pt_dst3Y - pt_dst2Y)) + 1.5;

    T *new_im = new T[slice * new_w * new_h];
    if (!new_im) return nullptr;
    // 以图左上角为原点
    // x0=x1cosθ+y1sinθ-ccosθ-dsinθ+a
    // y0=-x1sinθ+y1cosθ+csinθ-dcosθ+b
    // a=(width-1)/2, b=(height-1)/2
    // c=(new_w-1)/2, d=(new_h-1)/2

    // f1=-cosθ-dsinθ+a --> x0=x1cosθ+y1sinθ+f1
    // f2=csinθ-dcosθ+b --> y0=-x1sinθ+y1cosθ+f2

    double f1 = double(-0.5 * (new_w - 1) * cos_angle -
                       0.5 * (new_h - 1) * sin_angle + 0.5 * (width - 1));
    double f2 = double(0.5 * (new_w - 1) * sin_angle -
                       0.5 * (new_h - 1) * cos_angle + 0.5 * (height - 1));
    // i0->y0,j0->x0
    int i0 = 0, j0 = 0;
//    int p1 = new_w * new_h, p0 = width * height;
    for (size_t k = 0; k < slice; ++k) {
        for (size_t i = 0; i < new_h; ++i) {
            for (size_t j = 0; j < new_w; ++j) {
                i0 = -(double) j * sin_angle + (double) i * cos_angle + f2 + 0.5;
                j0 = (double) j * cos_angle + (double) i * sin_angle + f1 + 0.5;
                new_im[k * new_w * new_h + i * new_w + j] =
                        BilinearInterpolation(im, width, height, slice, j0, i0,k);
            }
        }
    }
    return new_im;
}

#endif //DIP_GEOTRANS_H
