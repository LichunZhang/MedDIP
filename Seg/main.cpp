// Program: DIP
// FileName:main.cpp
// Author:  Lichun Zhang
// Date:    2017/7/12 下午8:15
// Copyright (c) 2017 Lichun Zhang. All rights reserved.



#include <iostream>
#include <mhd_reader.h>
#include "segmentation.h"

bool TestSeg(const char *inname, const char *outname, size_t index) {
    if (!inname || !outname) return 1;
    MHDReader *reader = new MHDReader(inname);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return -1;
    }
    bool flag = false;
    size_t x = 0, y = 0;
    clock_t t_bg = clock();
    int n = 0;
    switch (index) {
        case 0:
            std::cout << "Enter the threshold: \t";
            std::cin >> n;
            t_bg = clock();

            flag = ::RobertsSeg(reader->GetImData(), reader->GetImWidth(),
                                reader->GetImHeight(), reader->GetImSlice(), n);
            break;
        case 1:
            std::cout << "Enter the threshold: \t";
            std::cin >> n;
            t_bg = clock();

            flag = ::SobelSeg(reader->GetImData(), reader->GetImWidth(),
                              reader->GetImHeight(), reader->GetImSlice(), n);
            break;
        case 2:
            std::cout << "Enter the threshold: \t";
            std::cin >> n;
            t_bg = clock();

            flag = ::PrewittSeg(reader->GetImData(), reader->GetImWidth(),
                                reader->GetImHeight(), reader->GetImSlice(), n);
            break;
        case 3:
            std::cout << "Enter the threshold: \t";
            std::cin >> n;
            t_bg = clock();

            flag = ::LaplacianSeg(reader->GetImData(), reader->GetImWidth(),
                                  reader->GetImHeight(), reader->GetImSlice(), n);
            break;
        case 4:
            std::cout << "Enter the threshold:\t";
            std::cin >> n;
            t_bg = clock();
            flag = ::EdgeTrack(reader->GetImData(), reader->GetImWidth(),
                               reader->GetImHeight(), reader->GetImSlice(), n);
            break;
        case 5:
            std::cout << "Enter the number:\t";
            std::cin >> n;
            t_bg = clock();
            flag = ::RegionAdaptiveSeg(reader->GetImData(), reader->GetImWidth(),
                                       reader->GetImHeight(), reader->GetImSlice(), n);
            break;
        case 6:
            std::cout << "Enter the position:\nX: ";
            std::cin >> x;
            std::cout << "Y: ";
            std::cin >> y;
            std::cout << "Enter the threshold:\t";
            std::cin >> n;
            t_bg = clock();
            flag = ::RegionGrow(reader->GetImData(), reader->GetImWidth(),
                                reader->GetImHeight(), reader->GetImSlice(),
                                x, y, n);
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
              << "0: RobertOperator Seg\n"
              << "1: SobelOperator Seg\n"
              << "2: PrewittOperator Seg\n"
              << "3: Laplace Seg\n"
              << "4: Track Edge\n"
              << "5: Region Adaptive Seg\n"
              << "6: Region Grow\n";
    size_t index = 0;
    std::cin >> index;
    return TestSeg(argv[1], argv[2], index);
//    return 0;
}

