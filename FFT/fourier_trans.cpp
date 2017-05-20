// Program: DIP
// FileName:fourier_trans.cpp
// Author:  Lichun Zhang
// Date:    2017/5/20 下午5:43
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#include <iostream>
#include <mhd_reader.h>
#include "fourier_trans.h"

/**!
 * @brief 按频率抽取的FFT
 * @param TD 指向时域数组的指针
 * @param FD 指向频域数组的指针
 * @param r  2的幂 即蝶形流图的级数
// */
void FFT(complex<double> *TD, complex<double> *FD, int r) {

    long count = 1 << r;                    //傅里叶变换点数
    complex<double> *w, *x1, *x2;
    w = new complex<double>[count / 2];//加权系数
    x1 = new complex<double>[count];   //蝶形输入
    x2 = new complex<double>[count];   //蝶形输出

    // 加权系数
    double angle = 0.0;
    for (int i = 0; i < count / 2; ++i) {
        angle = -i * 2 * M_PI / count;
        w[i] = complex<double>(cos(angle), sin(angle));
    }

    // 将时域点写入x1
    memcpy(x1, TD, sizeof(complex<double>) * count);

    // 蝶形算法FFT
    for (int k = 0; k < r; ++k) {               //蝶形的级数
        int bfsize = 1 << (r - k);              //当级点数大小
        int bfcount = 1 << k;                   //当级蝶形套数
        for (int j = 0; j < bfcount; ++j) {
            int p = j * bfsize;
            for (int i = 0; i < bfsize / 2; ++i) {
                x2[i + p] = x1[i + p] + x1[i + p + bfsize / 2];
                x2[i + p + bfsize / 2] = (x1[i + p] - x1[i + p + bfsize / 2]) * w[i * bfcount];
            }
        }
        complex<double> *x = x1;
        x1 = x2;    //上一级输出作为下一级输入
        x2 = x;     //下一级的输出，将在下一级循环中覆盖，节省空间。
    }

    // 码位倒置 赋值FD
    for (int i = 0; i < count; ++i) {
        long p = 0;
        for (int j = 0; j < r; ++j) {
            if (i & (1 << j))
                p += 1 << (r - 1 - j);
        }
        FD[i] = x1[p];
    }

    delete[] w;
    delete[] x1;
    delete[] x2;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Parameters error!\n";
        return -1;
    }
    MHDReader *reader = new MHDReader(argv[1]);
//    reader->ReadFile(argv[1]);
    Fourier(reader->GetImData(), reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice());
    reader->Write(argv[2]);
    delete reader;
    return 0;
}
