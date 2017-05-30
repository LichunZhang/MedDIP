// Program: DIP
// FileName:main.cpp
// Author:  Lichun Zhang
// Date:    2017/5/27 下午11:39
// Copyright (c) 2017 Lichun Zhang. All rights reserved.


#include <iostream>
#include <mhd_reader.h>
#include <mhd_writer.h>
#include "geo_trans.h"

void GetTransParameters(size_t &offsetX, size_t &offsetY, bool &type);

void TestTranslation(const char *input, const char *output,
                     size_t offsetX, size_t offsetY, bool type);

void GetMirrorParameters(bool &direction, bool &type);

void TestMirror(const char *input, const char *output, bool direction, bool type);

void TestTranspose(const char *input, const char *output);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: inputname outputname\n";
        return 1;
    }
    std::cout << "Functions:\n"
              << "1: Translation\n"
              << "2: Mirror\n"
              << "3: Transpose\n";
    int index = 0;
    std::cin >> index;
    switch (index) {
        case 1: {
            bool t = 1;
            size_t offset_x = 0, offset_y = 0;
            GetTransParameters(offset_x, offset_y, t);
            TestTranslation(argv[1], argv[2], offset_x, offset_y, t);
            break;
        }
        case 2: {
            bool t = 0, drt = 0;
            GetMirrorParameters(drt, t);
            TestMirror(argv[1], argv[2], drt, t);
            break;
        }
        case 3: {
            TestTranspose(argv[1], argv[2]);
            break;
        }
        default:
            break;
    }
    return 0;
}

void GetTransParameters(size_t &offsetX, size_t &offsetY, bool &type) {
    std::cout << "Please enter the translation parameters:\n"
              << "X:";
    std::cin >> offsetX;
    std::cout << "Y:";
    std::cin >> offsetY;
    std::cout << "Please enter the translation method: 0 or 1\n";
    std::cin >> type;
}

void TestTranslation(const char *input, const char *output,
                     size_t offsetX, size_t offsetY,
                     bool type = 1) {

    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return;
    }
    bool flag = false;
    if (type == 1)
        flag =
                ::Translation2(reader->GetImData(),
                               reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice(),
                               offsetX, offsetY);
    else
        flag =
                ::Translation(reader->GetImData(),
                              reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice(),
                              offsetX, offsetY);
    if (flag)
        reader->Write(output);
    delete reader;
}

void GetMirrorParameters(bool &direction, bool &type) {
    std::cout << "Please enter the direction: 0(vertical)/1(horizontal)\n";
    std::cin >> direction;
    std::cout << "Please enter the mirror method: 0 or 1\n";
    std::cin >> type;
}

void TestMirror(const char *input, const char *output, bool direction, bool type = 1) {
    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return;
    }
    bool flag = false;
    if (type == 0)
        flag = ::Mirror(reader->GetImData(),
                        reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice(),
                        direction);
    else
        flag = ::Mirror2(reader->GetImData(),
                         reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice(),
                         direction);
    if (flag)
        reader->Write(output);
    delete reader;
}

void TestTranspose(const char *input, const char *output) {

    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return;
    }

    bool flag = false;
    flag = ::Transpose(reader->GetImData(),
                       reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice());
    if (flag) {
        MHDWriter *writer = new MHDWriter(output);
        size_t dims[3] = {reader->GetImHeight(), reader->GetImWidth(), reader->GetImSlice()};
//        size_t* dims = new size_t[3];
//        dims[0] =reader->GetImHeight();
//        dims[1] =reader->GetImWidth();
//        dims[2] = reader->GetImSlice();
        writer->SetImgData(reader->GetImData(), dims);
        writer->WriteFile(output);
        delete writer;
    }
    delete reader;
}
