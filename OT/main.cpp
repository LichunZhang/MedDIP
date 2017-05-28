// Program: DIP
// FileName:main.cpp
// Author:  Lichun Zhang
// Date:    2017/5/27 下午1:34
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#include <mhd_reader.h>
#include <iostream>
#include "ortho_trans.h"

void TestFFT(const char *input, const char *output);

void TestDCT(const char *input, const char *output);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Parameters error!\n";
        return -1;
    }
    std::cout << "Functions:\n"
              << "1: FFT\n"
              << "2: DCT\n";
    size_t index = 0;
    std::cin >> index;
    switch (index) {
        case 1:
            TestFFT(argv[1], argv[2]);
            break;
        case 2:
            TestDCT(argv[1], argv[2]);
            break;
        default:
            break;
    }
    return 0;

}

void TestFFT(const char *input, const char *output) {
    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return;
    }
    bool flag =
            ::Fourier(reader->GetImData(),
                      reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice());
    if (flag)
        reader->Write(output);
    delete reader;
}

void TestDCT(const char *input, const char *output) {
    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return;
    }
    bool flag =
            ::DiscretCosin(reader->GetImData(), reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice());
    if (flag)
        reader->Write(output);
    delete reader;
}
