// Program: DIP
// FileName:template_trans.h
// Author:  Lichun Zhang
// Date:    2017/4/13 下午8:18
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#ifndef DIP_TEMPLATE_TRANS_H
#define DIP_TEMPLATE_TRANS_H

#include <cstddef>
#include <new>
#include <iostream>
#include <climits>

template<class T>
void Exchange(T *a, int i, int j) {
  T temp = a[i];
  a[i] = a[j];
  a[j] = temp;
}

template<class T>
T GetMedian(T *a, int num) {
  for (int i = 1; i < num; ++i) {
    for (int j = i; j > 0 && a[j] < a[j - 1]; --j) {
      Exchange<T>(a, j, j - 1);
    }
  }
  if (num % 2)
    return (a[num / 2 - 1] + a[num / 2]) / 2;
  else
    return a[num / 2];
}

/**
 * @brief       时域空间滤波模版-平均、高斯、拉普拉斯
 * @tparam T    图像数据类型
 * @param im    图像数据指针
 * @param width 图像宽度
 * @param height    图像高度
 * @param slice 图像切片数
 * @param filterW   滤波器宽度
 * @param filterH   滤波器高度
 * @param filterCX  滤波器中心元素x坐标
 * @param filterCY  滤波器中心元素y坐标
 * @param para_array    指向模版数组的指针
 * @param coeff 模版系数
 * @return  是否操作成功
 */
template<class T>
bool Template(T *im, size_t width, size_t height, size_t slice,
              size_t filterW, size_t filterH,
              size_t filterCX, size_t filterCY,
              double *para_array, double coeff) {
  if (!im || width <= 0 || height <= 0 || slice <= 0)
    return false;
  T *new_im = nullptr;
  try {
    new_im = new T[width * height];
  }
  catch (std::bad_alloc) {
    std::cout << "Failed to alloc memory!\n";
    return false;
  }

  T *lp_src = nullptr;
  for (int k = 0; k < slice; ++k) {
    memcpy(new_im, im + k * width * height, width * height * sizeof(T));
    double result = 0.0;
    for (int j = filterCY; j < height - filterH + filterCY + 1; ++j) {
      for (int i = filterCX; i < width - filterW + filterCX + 1; ++i) {
        // 指向原图像滤波模版开始处
        lp_src = im + k * height * width + (j - filterCY) * width +
            i - filterCX;
        // 模版覆盖区计算
        for (int l = 0; l < filterH; ++l) {
          for (int m = 0; m < filterW; ++m) {
            result += (*(lp_src + l * width + m)) * para_array[l * filterW + m];
          }
        }
        result *= coeff;
        result = result > std::numeric_limits<T>::max()
                 ? std::numeric_limits<T>::max()
                 : result;
        new_im[j * width + i] = result;
      }
    }
    memcpy(im + k * width * height, new_im, width * height * sizeof(T));
  }
  delete[] new_im;
  return true;
}

/*!
 * @brief    3x3拉普拉斯锐化 中心系数为正数
 * @tparam T 图像数据类型
 * @param im 图像指针
 * @param width  图像宽度
 * @param height 图像高度
 * @param slice  图像切片数
 * @return       是否操作成功
 */
template<typename T>
bool LaplaceSharpen(T *im, size_t width, size_t height, size_t slice) {
  double para[9] = {-1, -1, -1,
                    -1, 9, -1,
                    -1, -1, -1};
  return Template<T>(im, width, height, slice, 3, 3, 1, 1, para, 1.0);
}

/*!
 * @brief   中值滤波
 * @tparam T    图像数据类型
 * @param im    图像数据指针
 * @param width 图像宽度
 * @param height    图像高度
 * @param slice 图像切片数
 * @param filterW   滤波器宽度
 * @param filterH   滤波器高度
 * @param filterCX  滤波器中心元素x坐标
 * @param filterCY  滤波器中心元素y坐标
 * @return 是否操作成功
 */
template<class T>
bool FilterMedian(T *im, size_t width, size_t height, size_t slice,
                  size_t filterW, size_t filterH,
                  size_t filterCX, size_t filterCY) {
  T *new_im = nullptr;
  try {
    new_im = new T[width * height];
  }
  catch (std::bad_alloc) {
    std::cout << "Failed to alloc memory!\n";
    return false;
  }

  T *lp_src = nullptr;
  for (int k = 0; k < slice; ++k) {
    memcpy(new_im, im + k * width * height, width * height * sizeof(T));
    T *hvalue = new T[filterW * filterH];
    for (int j = filterCY; j < height - filterH + filterCY + 1; ++j) {
      for (int i = filterCX; i < width - filterW + filterCX + 1; ++i) {
        lp_src = im + k * height * width + (j - filterCY) * width +
            i - filterCX;
        for (int l = 0; l < filterH; ++l) {
          for (int m = 0; m < filterW; ++m) {
            hvalue[l * filterW + m] = *(lp_src + l * width + m);
          }
        }
        new_im[j * width + i] =
            GetMedian<T>(hvalue, filterW * filterH);
      }
    }
    memcpy(im + k * width * height, new_im, width * height * sizeof(T));
  }
  delete[] new_im;
  return true;
}

#endif //DIP_TEMPLATE_TRANS_H
