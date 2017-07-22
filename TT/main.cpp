// Program: DIP
// FileName:main.cpp
// Author:  Lichun Zhang
// Date:    2017/6/17 上午12:01
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#include "template_trans.h"
#include "../MHDIO/mhd_reader.h"


int TestTemplateTrans(int index, const char *inname, const char *outname) {
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
            flag = ::FilterMedian<unsigned char>(reader->GetImData(), reader->GetImWidth(),
                                                 reader->GetImHeight(), reader->GetImSlice(),
                                                 3, 3, 1, 1);
            break;
        case 1: {
            double para[9] = {1, 1, 1,
                              1, 1, 1,
                              1, 1, 1};
            flag = ::Template(reader->GetImData(), reader->GetImWidth(),
                              reader->GetImHeight(), reader->GetImSlice(),
                              3, 3, 1, 1,
                              para, (double) 1 / 9);
            break;
        }
        case 2: {
            double para[9] = {1, 2, 1,
                              2, 4, 2,
                              1, 2, 1};
            flag = ::Template(reader->GetImData(), reader->GetImWidth(),
                              reader->GetImHeight(), reader->GetImSlice(),
                              3, 3, 1, 1,
                              para, (double) 1 / 16);
            break;
        }
        case 3:
            flag = ::LaplaceSharpen(reader->GetImData(), reader->GetImWidth(),
                                    reader->GetImHeight(), reader->GetImSlice());
            break;
        case 4: {
            int threshold;
            std::cout << "Enter the threshold:\t";
            std::cin >> threshold;
            t_bg = clock();
            flag = ::GradSharp(reader->GetImData(), reader->GetImWidth(),
                               reader->GetImHeight(), reader->GetImSlice(), threshold);
            break;
        }
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
              << "0: Median Smooth\n"
              << "1: Average Smooth\n"
              << "2: Gaussian Smooth\n"
              << "3: GradSharp Sharp\n"
              << "4: LaplaceSharp Sharp\n";
    size_t index = 0;
    std::cin >> index;
    return TestTemplateTrans(index, argv[1], argv[2]);
}
