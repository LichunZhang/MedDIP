// Program: DIP
// FileName:main.cpp
// Author:  Lichun Zhang
// Date:    2017/6/18 上午11:26
// Copyright (c) 2017 Lichun Zhang. All rights reserved.



#include <iostream>
#include <mhd_reader.h>
#include "morphology_trans.h"

int TestErosion(const char *input, const char *output, int mode = 0,
                bool **structure = nullptr, int size = 0) {
    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return -1;
    }
    bool flag =
            ::Erosion(reader->GetImData(), reader->GetImWidth(),
                      reader->GetImHeight(), reader->GetImSlice(), mode,
                      structure, size);
    if (flag) {
        reader->SaveAs(output);
        delete reader;
        return 0;
    } else {
        delete reader;
        return -1;
    }
}

int TestDilation(const char *input, const char *output, int mode = 0,
                 bool **structure = nullptr, int size = 0) {
    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return -1;
    }
    bool flag =
            ::Dilation(reader->GetImData(), reader->GetImWidth(),
                       reader->GetImHeight(), reader->GetImSlice(), mode,
                       structure, size);
    if (flag)
        reader->SaveAs(output);
    delete reader;
    return 0;
}

int TestOpen(const char *input, const char *output, int mode = 0,
             bool **structure = nullptr, int size = 0) {
    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return -1;
    }
    bool flag =
            ::Open(reader->GetImData(), reader->GetImWidth(),
                   reader->GetImHeight(), reader->GetImSlice(), mode,
                   structure, size);
    if (flag) {
        reader->SaveAs(output);
        delete reader;
        return 0;
    } else {
        delete reader;
        return -1;
    }
}

int TestClose(const char *input, const char *output, int mode = 0,
              bool **structure = nullptr, int size = 0) {
    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return -1;
    }
    bool flag =
            ::Close(reader->GetImData(), reader->GetImWidth(),
                    reader->GetImHeight(), reader->GetImSlice(), mode,
                    structure, size);
    if (flag) {
        reader->SaveAs(output);
        delete reader;
        return 0;
    } else {
        delete reader;
        return -1;
    }
}

int TestThining(const char *input, const char *output) {
    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return -1;
    }
    bool flag =
            ::Thining(reader->GetImData(), reader->GetImWidth(),
                      reader->GetImHeight(), reader->GetImSlice());
    if (flag) {
        reader->SaveAs(output);
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
    bool s1[3] = {0, 1, 0};
    bool s2[3] = {1, 1, 1};
    bool s3[3] = {0, 1, 0};
    bool *structure[3] = {s1, s2, s3};
    switch (index) {
        case 0:
            return TestErosion(argv[1], argv[2], 2, structure, 3);
        case 1:
            return TestDilation(argv[1], argv[2], 2, structure, 3);
        case 2:
            return TestOpen(argv[1], argv[2], 2, structure, 3);
        case 3:
            return TestClose(argv[1], argv[2], 2, structure, 3);
        case 4:
            return TestThining(argv[1], argv[2]);
        default:
            return 1;
    }
}