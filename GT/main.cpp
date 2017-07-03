// Program: DIP
// FileName:main.cpp
// Author:  Lichun Zhang
// Date:    2017/5/27 下午11:39
// Copyright (c) 2017 Lichun Zhang. All rights reserved.


#include <iostream>
#include <mhd_reader.h>
#include <mhd_writer.h>
#include "geometry_trans.h"

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
        reader->SaveAs(output);
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
        reader->SaveAs(output);
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
        writer->SetImgData(reader->GetImData(), dims);
        writer->WriteFile(output);
        delete writer;
    }
    delete reader;
}

void GetZoomParameters(float &rationX, float &rationY) {
    std::cout << "Please enter the zoom ration:\nX:";
    std::cin >> rationX;
    std::cout << "Y:";
    std::cin >> rationY;
}

void TestZoom(const char *input, const char *output, float rationX, float rationY) {
    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return;
    }
    unsigned char *data = ::Zoom(reader->GetImData(),
                                 reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice(),
                                 rationX, rationY);
    if (data) {
        size_t new_w = reader->GetImWidth() * rationX + 0.5;
        size_t new_h = reader->GetImHeight() * rationY + 0.5;
        size_t dims[3] = {new_w, new_h, reader->GetImSlice()};
        MHDWriter *writer = new MHDWriter(output);
        writer->SetImgData(data, dims);
        writer->WriteFile(output);
        delete writer;
        delete data;
    }
    delete reader;
}

void GetRotateParameters(double &angle, bool &type) {
    std::cout << "Please enter the angle:\n";
    std::cin >> angle;
    std::cout << "Please enter the type of method (0 or 1):\n";
    std::cin >> type;
}

void TestRotate(const char *input, const char *output, double angle, bool type = 1) {
    MHDReader *reader = new MHDReader(input);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return;
    }

    size_t new_w = 0, new_h = 0;
    unsigned char *data = nullptr;
    if (type == 0)
        data = ::Rotate(reader->GetImData(),
                        reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice(),
                        angle, new_w, new_h);
    else
        data = ::Rotate2(reader->GetImData(),
                         reader->GetImWidth(), reader->GetImHeight(), reader->GetImSlice(),
                         angle, new_w, new_h);
    if (data) {
        size_t dims[3] = {new_w, new_h, reader->GetImSlice()};
        MHDWriter *writer = new MHDWriter(output);
        writer->SetImgData(data, dims);
        writer->WriteFile(output);
        delete writer;
        delete data;
    }
    delete reader;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: inputname outputname\n";
        return 1;
    }
    std::cout << "Functions:\n"
              << "1: Translation\n"
              << "2: Mirror\n"
              << "3: Transpose\n"
              << "4: Zoom\n"
              << "5: Ratate\n";
    int index = 0;
    std::cin >> index;
    clock_t t_bg = clock();
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
        case 4: {
            float rationX = 0.0, rationY = 0.0;
            GetZoomParameters(rationX, rationY);
            TestZoom(argv[1], argv[2], rationX, rationY);
            break;
        }
        case 5: {
            double angle = 0.0;
            bool type;
            GetRotateParameters(angle, type);
            TestRotate(argv[1], argv[2], angle, type);
        }
        default:
            break;
    }
    clock_t t_ed = clock();
    std::cout << "Time: " << double(t_ed - t_bg) / 1000 << " ms\n";
    return 0;
}
