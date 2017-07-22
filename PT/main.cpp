// Program: DIP
// FileName:main.cpp
// Author:  Lichun Zhang
// Date:    2017/6/17 上午12:31
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#include <mhd_reader.h>
#include "point_trans.h"


int TestPointTrans(int index, const char *inname, const char *outname) {
    if (!inname || !outname) return 1;
    MHDReader *reader = new MHDReader(inname);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return -1;
    }
    bool flag = false;
    clock_t t_bg = clock();
    int th1 = 0, th2 = 0;
    switch (index) {
        case 0:
            std::cout << "Enter the threshold:\t";
            std::cin >> th1;
            t_bg = clock();
            flag = ::ThresholdTrans(reader->GetImData(), reader->GetImWidth(),
                                    reader->GetImHeight(), reader->GetImSlice(), th1);
            break;
        case 1:
            std::cout << "Enter the low threshold and up threshold:\t";
            std::cin >> th1 >> th2;
            t_bg = clock();
            flag = ::WindowTrans(reader->GetImData(), reader->GetImWidth(),
                                 reader->GetImHeight(), reader->GetImSlice(), th1, th2);
            break;
        case 2: {
            std::cout << "Enter the x1, y1, x2, y2 (x2 > x1, y2 > y1):\t";
            int x1, y1, x2, y2;
            std::cin >> x1 >> y1 >> x2 >> y2;
            t_bg = clock();
            flag = ::GrayStretch(reader->GetImData(), reader->GetImWidth(),
                                 reader->GetImHeight(), reader->GetImSlice(), x1, y1, x2, y2);
            break;
        }
        case 3:
            flag = ::HisEqualize(reader->GetImData(), reader->GetImWidth(),
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
        return -1;
    }
    std::cout << "Functions:\n"
              << "0: Threshold Trans\n"
              << "1: Window Trans\n"
              << "2: Gray Stretch\n"
              << "3: Histogram Equalize\n";
    size_t index = 0;
    std::cin >> index;
    return TestPointTrans(index, argv[1], argv[2]);
}