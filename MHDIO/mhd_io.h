// Program: DIP
// FileName:MHD_IO.h
// Author:  Lichun Zhang
// Date:    2017/5/20 上午9:49
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#ifndef DIP_MHD_IO_H
#define DIP_MHD_IO_H


#include <string>

class MHD_IO {
public:
    MHD_IO(const char *name = nullptr);

    ~MHD_IO();

    std::string GetFileName() const { return _fileName; }

    size_t GetImWidth() const { return _dimX; }

    size_t GetImHeight() const { return _dimY; }

    size_t GetImSlice() const { return _dimZ; }

    double GetSpacingX() const { return _spacingX; }

    double GetSpacingY() const { return _spacingY; }

    double GetSpacingZ() const { return _spacingZ; }

    const std::string GetDataType() const { return _dataType; }

    //TODO: Return various type data
    unsigned char *GetImData() {
        return _dataType.empty() ? nullptr : _imData;
    }

protected:
    std::string _fileName;
    std::size_t _dimX, _dimY, _dimZ;
    double _spacingX, _spacingY, _spacingZ;
    std::string _dataType;
    unsigned char *_imData;
};


#endif //DIP_MHD_IO_H