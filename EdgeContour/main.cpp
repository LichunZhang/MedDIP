// Program: DIP
// FileName:main.cpp
// Author:  Lichun Zhang
// Date:    2017/6/21 下午7:27
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#include <mhd_reader.h>
#include "edgecontour_detect.h"

int TestEdgeDetection(const char *inname, const char *outname, size_t index) {
    if (!inname || !outname) return 1;
    MHDReader *reader = new MHDReader(inname);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return -1;
    }
    bool flag = false;

    switch (index) {
        case 0:
            flag = ::Robert(reader->GetImData(), reader->GetImWidth(),
                            reader->GetImHeight(), reader->GetImSlice());
            break;
        case 1:
            flag = ::Sobel(reader->GetImData(), reader->GetImWidth(),
                           reader->GetImHeight(), reader->GetImSlice());
            break;
        case 2:
            flag = ::Prewitt(reader->GetImData(), reader->GetImWidth(),
                             reader->GetImHeight(), reader->GetImSlice());
            break;
        case 3:
            flag = ::Krisch(reader->GetImData(), reader->GetImWidth(),
                            reader->GetImHeight(), reader->GetImSlice());
            break;
        case 4:
            flag = ::GaussLaplace(reader->GetImData(), reader->GetImWidth(),
                                  reader->GetImHeight(), reader->GetImSlice());
            break;
        default:
            break;
    }
    if (flag) {
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
              << "0: Robert Edge Detection\n"
              << "1: Sobel Edge Detection\n"
              << "2: Prewitt Edge Detection\n"
              << "3: Krisch Edge Detection\n"
              << "4: Gaussian Laplace Edge Detection\n";
    size_t index = 0;
    std::cin >> index;
    return TestEdgeDetection(argv[1], argv[2], index);

}