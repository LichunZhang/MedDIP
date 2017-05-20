// Program: DIP
// FileName:MHD_IO.cpp
// Author:  Lichun Zhang
// Date:    2017/5/20 上午9:49
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#include "mhd_io.h"

MHD_IO::MHD_IO(const char *name) : _fileName(name),
                                   _dimX(0), _dimY(0), _dimZ(0),
                                   _spacingX(1.0), _spacingY(1.0), _spacingZ(1.0),
                                   _dataType("MET_UCHAR"),
                                   _imData(nullptr) {

}


MHD_IO::~MHD_IO() {
    if (_imData) delete[] _imData;
}
