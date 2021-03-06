// Program: DIP
// FileName:mhd_writer.cpp
// Author:  Lichun Zhang
// Date:    2017/5/20 上午9:38
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#include <fstream>
#include <iostream>
#include <type_traits>
#include "mhd_writer.h"

MHDWriter::MHDWriter(const char *name/* = nullptr*/) : MHD_IO(name) {}

MHDWriter::~MHDWriter() {}


/**
 * @brief 输出mhd图像文件
 * @param name 输出文件名字,有后缀.mhd
 */
void MHDWriter::WriteFile(const char *name) {
    // Ordinary format (*.mhd)
//    ObjectType = Image
//    NDims = 3
//    BinaryData = True
//    BinaryDataByteOrderMSB = False
//    TransformMatrix = 1 0 0 0 1 0 0 0 1
//    Offset = 0 0 0
//    CenterOfRotation = 0 0 0
//    ElementSpacing = 0.78 0.78 2.50
//    DimSize = 512 512 61
//    AnatomicalOrientation = RAI
//    ElementSize = 1 1 1
//    ElementType = MET_UCHAR
//    ElementDataFile = abell5mm_reorder.raw
    if (!name) return;
    std::string name_str = name;
    auto index = name_str.find_last_of(".");
    if(index == std::string::npos)
        name_str += ".mhd";
    else
        name_str = name_str.substr(0,index+1)+"mhd";
    _fileName = name_str;
    if (!_imData || _dataType.empty() || !_dimY || !_dimY || !_dimZ)
        return;
    WriteHeader(_fileName.c_str());
    std::string str_raw_name = _fileName.substr(0, _fileName.find_last_of(".") + 1);
    str_raw_name += "raw";
    WriteRaw(str_raw_name.c_str());
}

void MHDWriter::WriteHeader(const char *headerName, const char *rawName /* = nullptr*/) {
    if (!headerName) return;
    std::ofstream out(headerName);
    if (!out) {
        std::cout << "Error! Can't Save File " << _fileName << std::endl;
        out.close();
        return;
    }
    out << "ObjectType = Image\n" << "NDims = 3\n"
        << "BinaryData = True\n" << "BinaryDataByteOrderMSB = False\n"
        << "TransformMatrix = 1 0 0 0 1 0 0 0 1\n"
        << "Offset = 0 0 0\n" << "CenterOfRotation = 0 0 0\n"
        << "ElementSpacing = " << _spacingX << " " << _spacingY << " " << _spacingZ << "\n"
        << "DimSize = " << _dimX << " " << _dimY << " " << _dimZ << "\n"
        << "AnatomicalOrientation = RAI\n"
        << "ElementSize = 1 1 1\n"
        << "ElementType = " << _dataType << "\n"
        << "ElementDataFile = ";
    // ElementDataFile
    std::string str_raw_name;
    if (rawName)
        str_raw_name = rawName;
    else {
        str_raw_name = _fileName.substr(0, _fileName.find_last_of(".") + 1);
        str_raw_name += "raw";
    }
    out << str_raw_name << "\n";
    out.close();
}

void MHDWriter::WriteRaw(const char *name) {
    if (!name || !_imData) return;
    std::FILE *fn = std::fopen(name, "wb");
    fwrite(_imData, sizeof(unsigned char), _dimX * _dimY * _dimZ, fn);
    fclose(fn);
    std::fclose(fn);
}


