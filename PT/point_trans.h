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
 * @brief  直方图均衡化
 * @tparam T 图像数据类型
 * @param im 图像指针
 * @param width  图像宽度
 * @param height 图像高度
 * @param slice  图像切片数
 * @return       是否操作成功
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
    value_map[i] = (T)value;
  }

  //对图像像素设值（灰度映射表）
  for (long i = 0; i < size; ++i)
    im[i] = value_map[im[i] - gray_floor];

  for (int i = 0; i < range; ++i) {
    printf("%d: %d\n",i,value_map[i]);
//    std::cout << i << ": " << value_map[i] << std::endl;
  }

  delete[] value_count;
  delete[] value_map;

  return true;
}

#endif //DIP_POINT_TRANS_H
