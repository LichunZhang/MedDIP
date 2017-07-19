// Program: DIP
// FileName:main.cpp
// Author:  Lichun Zhang
// Date:    2017/6/18 上午11:26
// Copyright (c) 2017 Lichun Zhang. All rights reserved.



#include <iostream>
#include <mhd_reader.h>
#include "morphology_trans.h"

int TestMorphologyTrans(int index, const char *inname, const char *outname) {
    if (!inname || !outname) return 1;
    MHDReader *reader = new MHDReader(inname);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return -1;
    }
    bool flag = false;
    bool s1[3] = {0, 1, 0};
    bool s2[3] = {1, 1, 1};
    bool s3[3] = {0, 1, 0};
    bool *structure[3] = {s1, s2, s3};
    int mode = 2;
    size_t size = 3;
    clock_t t_bg = clock();
    switch (index) {
        case 0:
            flag = ::Erosion(reader->GetImData(), reader->GetImWidth(),
                             reader->GetImHeight(), reader->GetImSlice(), mode,
                             structure, size);
            break;
        case 1:
            flag = ::Dilation(reader->GetImData(), reader->GetImWidth(),
                              reader->GetImHeight(), reader->GetImSlice(), mode,
                              structure, size);
            break;
        case 2:
            flag = ::Open(reader->GetImData(), reader->GetImWidth(),
                          reader->GetImHeight(), reader->GetImSlice(), mode,
                          structure, size);
            break;
        case 3:
            flag = ::Close(reader->GetImData(), reader->GetImWidth(),
                           reader->GetImHeight(), reader->GetImSlice(), mode,
                           structure, size);
            break;
        case 4:
            flag = ::Thining(reader->GetImData(), reader->GetImWidth(),
                             reader->GetImHeight(), reader->GetImSlice());
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
        std::cout << "Usage: inputname outputname\n";
        return 1;
    }
    std::cout << "Functions:\n"
              << "0: Erosion\n"
              << "1: Dilation\n"
              << "2: Open\n"
              << "3: Close\n"
              << "4: Thining\n";
    size_t index = 0;
    std::cin >> index;
    return TestMorphologyTrans(index, argv[1], argv[2]);
}