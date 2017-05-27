// Program: DIP
// FileName:ot_includes.h.h
// Author:  Lichun Zhang
// Date:    2017/5/27 下午1:35
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#ifndef DIP_OT_INCLUDES_H_H
#define DIP_OT_INCLUDES_H_H

#include <ccomplex>

//using namespace std;
using std::complex;

//void FFT_DC(double *TD, double *FD, int r);

void FFT(complex<double> *TD, complex<double> *FD, int r);

template<typename T>
bool Fourier(T *im, size_t width, size_t height, size_t slice) {

    if (!im)
        return false;
    int w = 1.0, h = 1.0, wp = 0, hp = 0;
    while (w * 2 <= width) {
        w *= 2;
        ++wp;
    }

    while (h * 2 <= height) {
        h *= 2;
        ++hp;
    }

    complex<double> *FD = new complex<double>[w * h];
    complex<double> *TD = new complex<double>[w * h];

    T *lpSrc = nullptr;
    for (int k = 0; k < slice; ++k) {
        auto p = k * width * height;

        //对时域根据原图像进行赋值
        for (int i = 0; i < h; ++i) {       //行
            for (int j = 0; j < w; ++j) {   //列
                lpSrc = &im[p + i * width + j];
                TD[w * i + j] = complex<double>(*lpSrc, 0);
            }
        }

        //先对每一行做一维FFT(原图y方向)
        for (int i = 0; i < h; ++i) {
            FFT(&TD[w * i], &FD[w * i], wp);
        }

        //保存变换结果 调换方向
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                TD[j * h + i] = FD[i * w + j];
            }
        }

        //对原图x方向进行一维FFT
        for (int i = 0; i < w; ++i) {
            FFT(&TD[i * h], &FD[i * h], hp);
        }

        //计算频谱
        double value = 0.0;
        for (int i = 0; i < h; ++i) {       //行
            for (int j = 0; j < w; ++j) {   //列
                long index = j * h + i;
                value = sqrt(FD[index].real() * FD[index].real() +
                             FD[index].imag() * FD[index].imag()) / 100;
                if (value > 255)
                    value = 255;
                int i_2 = i < h / 2 ? i + h / 2 : i - h / 2;
                int j_2 = j < w / 2 ? j + w / 2 : j - w / 2;
                lpSrc = &im[p + i_2 * width + j_2];
                *(lpSrc) = value;
            }
        }
    }
    delete[] TD;
    delete[] FD;
    return true;
}

void DCT(double *f, double *F, int r);

template<typename T>
bool DiscretCosin(T *im, size_t width, size_t height, size_t slice) {
    if (!im)
        return false;
    int w = 1.0, h = 1.0, wp = 0, hp = 0;
    while (w * 2 <= width) {
        w *= 2;
        ++wp;
    }

    while (h * 2 <= height) {
        h *= 2;
        ++hp;
    }
    double *f = new double[w * h];
    double *F = new double[w * h];
    T *lpSrc = nullptr;

    for (size_t k = 0; k < slice; ++k) {
        auto p = k * width * height;

        // 对时域根据原图像进行赋值
        for (size_t i = 0; i < h; ++i) {
            for (size_t j = 0; j < w; ++j) {
                lpSrc = &im[p + i * width + j];
                f[w * i + j] = *(lpSrc);
            }
        }

        // y方向离散傅里叶变换
        for (size_t i = 0; i < h; ++i)
            DCT(&f[i * w], &F[i * w], wp);

        // 保存计算结果 调换方向
        for (size_t i = 0; i < h; ++i) {
            for (size_t j = 0; j < w; ++j) {
                f[j * h + i] = F[w * i + j];
            }
        }

        // x方向离散傅里叶变换
        for (size_t i = 0; i < w; ++i)
            DCT(&f[i * h], &F[i * h], hp);

        //计算频谱
        double value = 0.0;
        for (int i = 0; i < h; ++i) {       //行
            for (int j = 0; j < w; ++j) {   //列
                long index = j * h + i;
                value = fabs(F[j * h + i]);
                if (value > 255)
                    value = 255;
                lpSrc = &im[p + i * width + j];
                *(lpSrc) = value;
            }
        }
    }
    delete[] f;
    delete[] F;
    return true;
}

#endif //DIP_OT_INCLUDES_H_H
