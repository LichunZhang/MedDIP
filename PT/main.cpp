// Program: DIP
// FileName:main.cpp
// Author:  Lichun Zhang
// Date:    2017/6/17 上午12:31
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#include <mhd_reader.h>
#include "point_trans.h"

int TestThresholdTrans(const char *input, const char *output, int threshold) {
    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return -1;
    }

    bool flag =
            ::ThresholdTrans(reader->GetImData(), reader->GetImWidth(),
                             reader->GetImHeight(), reader->GetImSlice(), threshold);
    if (flag)
        reader->SaveAs(output);
    delete reader;
    return 0;
}

int TestHisEqualize(const char *input, const char *output) {
    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return -1;
    }

    bool flag =
            ::HisEqualize(reader->GetImData(), reader->GetImWidth(),
                          reader->GetImHeight(), reader->GetImSlice());
    if (flag)
        reader->SaveAs(output);
    delete reader;
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: inputname outputname\n";
        return -1;
    }
    std::cout << "Functions:\n"
              << "0: Threshold Trans\n"
              << "2: Histogram Equalize\n";
    size_t index = 0;
    std::cin >> index;
    switch (index) {
        case 0:
            return TestThresholdTrans(argv[1], argv[2], atoi(argv[3]));
        case 1:
            return TestHisEqualize(argv[1],argv[2]);
        default:
            return 1;
    }
}