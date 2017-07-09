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

    clock_t t_bg = clock();
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
        case 5:
            flag = ::Contour(reader->GetImData(), reader->GetImWidth(),
                             reader->GetImHeight(), reader->GetImSlice());
            break;
        case 6:
            flag = ::Trace(reader->GetImData(), reader->GetImWidth(),
                           reader->GetImHeight(), reader->GetImSlice());
            break;
        case 7:
            flag = ::Fill(reader->GetImData(), reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice(),
                          reader->GetImWidth() / 2, reader->GetImHeight() / 2, 128);
            break;
        case 8:
            flag = ::Fill2(reader->GetImData(), reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice(),
                           reader->GetImWidth() / 2, reader->GetImHeight() / 2, 0);
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
              << "0: Robert Edge Detection\n"
              << "1: Sobel Edge Detection\n"
              << "2: Prewitt Edge Detection\n"
              << "3: Krisch Edge Detection\n"
              << "4: Gaussian Laplace Edge Detection\n"
              << "5: Contour\n"
              << "6: Trace Contour\n"
              << "7: Seed fill\n"
              << "8: Seed fill 2\n";
    size_t index = 0;
    std::cin >> index;
    return TestEdgeDetection(argv[1], argv[2], index);

}