// Program: DIP
// FileName:template_trans.cpp
// Author:  Lichun Zhang
// Date:    2017/5/20 下午4:53
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#include "template_trans.h"
#include "../MHDIO/mhd_reader.h"

void TestMedianFilter(const char *input, const char *output);
void TestAverageFilter(const char *input, const char *output);
void TestGaussianFilter(const char *input, const char *output);
void TestLaplaceSharpen(const char *input, const char *output);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Parameter error!\n";
        return 1;
    }
    std::cout << "Functions:\n"
              << "1: Median Smooth\n"
              << "2: Average Smooth\n"
              << "3: Gaussian Smooth\n"
              << "4: LaplaceSharp Sharp\n";
    size_t index = 0;
    std::cin >> index;
    switch (index) {
        case 1:TestMedianFilter(argv[1], argv[2]);
            break;
        case 2:TestAverageFilter(argv[1], argv[2]);
            break;
        case 3:TestGaussianFilter(argv[1], argv[2]);
            break;
        case 4:TestLaplaceSharpen(argv[1], argv[2]);
            break;

        default:break;
    }
    return 0;
}

void TestMedianFilter(const char *input, const char *output) {
    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return;
    }

    bool flag =
            ::FilterMedian<unsigned char>(reader->GetImData(), reader->GetImWidth(),
                                          reader->GetImHeight(), reader->GetImSlice(),
                                          3, 3, 1, 1);
    if (flag)
        reader->Write(output);
    delete reader;
}

void TestAverageFilter(const char *input, const char *output) {
    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return;
    }
    double para[9] = {1, 1, 1,
                      1, 1, 1,
                      1, 1, 1};
    bool flag =
            ::Template(reader->GetImData(), reader->GetImWidth(),
                       reader->GetImHeight(), reader->GetImSlice(),
                       3, 3, 1, 1,
                       para, (double) 1 / 9);
    if (flag)
        reader->Write(output);
    delete reader;
}

void TestGaussianFilter(const char *input, const char *output) {
    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return;
    }
    double para[9] = {1, 2, 1,
                      2, 4, 2,
                      1, 2, 1};
    bool flag =
            ::Template(reader->GetImData(), reader->GetImWidth(),
                       reader->GetImHeight(), reader->GetImSlice(),
                       3, 3, 1, 1,
                       para, (double) 1 / 16);
    if (flag)
        reader->Write(output);
    delete reader;
}

void TestLaplaceSharpen(const char *input, const char *output) {
    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return;
    }

    bool flag =
            ::LaplaceSharpen(reader->GetImData(), reader->GetImWidth(),
                             reader->GetImHeight(), reader->GetImSlice());
    if (flag)
        reader->Write(output);
    delete reader;
}

