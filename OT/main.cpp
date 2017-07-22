// Program: DIP
// FileName:main.cpp
// Author:  Lichun Zhang
// Date:    2017/5/27 下午1:34
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#include <mhd_reader.h>
#include <iostream>
#include "ortho_trans.h"


int TestOrthogonal(int index, const char *inname, const char *outname) {
    if (!inname || !outname) return 1;
    MHDReader *reader = new MHDReader(inname);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return -1;
    }
    bool flag = false;
    clock_t t_bg = clock();
    switch (index) {
        case 0:
            flag = ::Fourier(reader->GetImData(),
                             reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice());
            break;
        case 1:
            flag = ::DiscretCosin(reader->GetImData(),
                                  reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice());
            break;
        default:
            break;
    }

    clock_t t_ed = clock();
    if (flag) {
        std::cout << "Time: " << double(t_ed - t_bg) / 1000 << " ms\n";
        reader->SaveAs(outname);
        delete reader;
        return 0;
    } else {
        delete reader;
        return -1;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Parameters error!\n";
        return -1;
    }
    std::cout << "Functions:\n"
              << "0: FFT\n"
              << "1: DCT\n";
    size_t index = 0;
    std::cin >> index;
    return TestOrthogonal(index, argv[1], argv[2]);
}


