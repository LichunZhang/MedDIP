// Program: DIP
// FileName:DCT.cpp
// Author:  Lichun Zhang
// Date:    2017/5/26 下午11:27
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#include "ot_includes.h"

/**!
 * @brief 离散余弦变换
 * @param f 指向时域的指针
 * @param F 指向频域的指针
 * @param r 2的幂 即蝶形流图的级数
 */
void DCT(double *f, double *F, int r) {
    long count = 1 << r;                //离散余弦变换点数
    complex<double> *X = new complex<double>[count * 2];  //需拓展成2N点进行FFT变换
    memset(X, 0, sizeof(complex<double>) * count * 2);   //赋初值0

    // 将时域点写入数组X (N点后均为0)
    for (long i = 0; i < count; ++i)
        X[i] = complex<double>(f[i], 0);
    FFT(X, X, r + 1);    //调用FFT 延拓为2N,因此为r+1
    F[0] = X[0].real() / sqrt(count);  //F[0]
    double temp = sqrt((double) 2 / count);

    // 频域赋值，只取实部
    for (long i = 1; i < count; ++i)
        F[i] = temp * (cos(i * M_PI / (2 * count)) * X[i].real() +
                       sin(i * M_PI / (2 * count)) * X[i].imag());
    delete[] X;
}
