// Program: DIP
// FileName:Utiles.cpp
// Author:  Lichun Zhang
// Date:    2017/4/6 下午2:57
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#include <string>
#include <fstream>
#include <iostream>
#include "utiles.h"

void WriteMHDHeader(const std::string &name,
                    size_t dims[], double spacing[]) {

    std::ofstream out(name);
    std::string raw_name = name.substr(0, name.find_last_of(".") + 1);
    raw_name += "raw";
    out << "ObjectType = Image\n" << "NDims = 3\n"
        << "BinaryData = True\n" << "BinaryDataByteOrderMSB = False\n"
        << "TransformMatrix = 1 0 0 0 1 0 0 0 1\n"
        << "Offset = 0 0 0\n" << "CenterOfRotation = 0 0 0\n"
        << "ElementSpacing = " << spacing[0] << " " << spacing[1] << " " << spacing[2] << "\n"
        << "DimSize = " << dims[0] << " " << dims[1] << " " << dims[2] << "\n"
        << "AnatomicalOrientation = RAI\n"
        << "ElementSize = 1 1 1\n"
        << "ElementType = MET_UCHAR\n"
        << "ElementDataFile = " << raw_name << "\n";
    out.close();
}

void WriteMHDRaw(const std::string &name, const unsigned char *data,
                 size_t x, size_t y, size_t z) {
    if (!data) {
        std::cout << "Image data is nullptr!\n";
        return;
    }
    std::FILE *fn = std::fopen(name.c_str(), "wb");
    fwrite(data, sizeof(unsigned char), x * y * z, fn);
    fclose(fn);
    std::fclose(fn);
}

void WriteMHD(const char *name, const unsigned char *data,
              size_t dims[], double spacing[]) {
    std::string mhd_name = std::string(name) + ".mhd";
    std::string raw_name = std::string(name) + ".raw";
    WriteMHDHeader(mhd_name, dims, spacing);
    WriteMHDRaw(raw_name, data, dims[0], dims[1], dims[2]);
}
