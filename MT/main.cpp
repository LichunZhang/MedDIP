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
    if (flag)
        reader->SaveAs(output);
    delete reader;
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: inputname outputname\n";
        return 1;
    }
    std::cout << "Functions:\n"
              << "0: Erosion\n";
    size_t index = 0;
    std::cin >> index;
    switch (index) {
        case 0: {
            bool s1[3]={0,1,0};
            bool s2[3]={1,1,1};
            bool s3[3]={0,1,0};
            bool* structure[3]={s1,s2,s3};
            return TestErosion(argv[1], argv[2],2,structure,3);
        }
        default:
            return 1;
    }
}