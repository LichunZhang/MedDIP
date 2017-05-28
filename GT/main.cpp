// Program: DIP
// FileName:main.cpp
// Author:  Lichun Zhang
// Date:    2017/5/27 下午11:39
// Copyright (c) 2017 Lichun Zhang. All rights reserved.


#include <iostream>
#include <mhd_reader.h>
#include "geo_trans.h"

void GetTransParameters(size_t &index, size_t &offsetX, size_t &offsetY);

void TestTranslation(const char *input, const char *output, size_t offsetX, size_t offsetY);

void TestTranslation2(const char *input, const char *output, size_t offsetX, size_t offsetY);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: inputname outputname\n";
        return 1;
    }
    std::cout << "Functions:\n"
              << "1: Translation\n";
    auto index = 0;
    std::cin >> index;
    switch (index) {
        case 1:
            size_t t, offset_x, offset_y;
            GetTransParameters(t, offset_x, offset_y);
            if (t == 1)
                TestTranslation(argv[1], argv[2], offset_x, offset_y);
            else if (t == 2)
                TestTranslation2(argv[1], argv[2], offset_x, offset_y);
            else
                return 1;
            break;
        case 2:
            break;
        default:
            break;
    }
    return 0;
}

void GetTransParameters(size_t &index, size_t &offsetX, size_t &offsetY) {
    std::cout << "Please enter the translation method: 1 or 2\n";
    std::cin >> index;
    std::cout << "Please enter the translation parameters:\n"
              << "X:";
    std::cin >> offsetX;
    std::cout << "Y:";
    std::cin >> offsetY;
}

void TestTranslation(const char *input, const char *output, size_t offsetX, size_t offsetY) {

    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return;
    }

    bool flag =
            ::Translation(reader->GetImData(),
                          reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice(),
                          offsetX, offsetY);
    if (flag)
        reader->Write(output);
    delete reader;
}

void TestTranslation2(const char *input, const char *output, size_t offsetX, size_t offsetY) {

    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return;
    }

    bool flag =
            ::Translation2(reader->GetImData(),
                           reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice(),
                           offsetX, offsetY);
    if (flag)
        reader->Write(output);
    delete reader;
}
