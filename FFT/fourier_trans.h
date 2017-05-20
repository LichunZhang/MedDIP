// Program: DIP
// FileName:fourier_trans.h
// Author:  Lichun Zhang
// Date:    2017/5/20 下午5:43
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#ifndef DIP_FOURIER_TRANS_H
#define DIP_FOURIER_TRANS_H

#include <ccomplex>

using namespace std;

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

#endif //DIP_FOURIER_TRANS_H
