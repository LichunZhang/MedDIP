// Program: DIP
// FileName:mhd_writer.h
// Author:  Lichun Zhang
// Date:    2017/5/20 上午9:38
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#ifndef DIP_MHD_WRITER_H
#define DIP_MHD_WRITER_H


#include <string>
#include "mhd_io.h"

class MHDWriter : public MHD_IO {
public:
    MHDWriter(const char *name = nullptr);

    virtual ~MHDWriter();

    void WriteFile(const char *name);

    template<typename T>
    void SetImgData(const void *data, const size_t *dims, const double *spacing = nullptr,
                    const std::string type = "");

private:
    void SetImgDims(const size_t *dims) {
        if (!dims) return;
        _dimX = dims[0];
        _dimY = dims[1];
        _dimZ = dims[2];
    }

    void SetImgSpacing(const double *spacing) {
        if (!spacing) return;
        _spacingX = spacing[0];
        _spacingY = spacing[1];
        _spacingZ = spacing[2];
    }


    void SetImgType(const std::string type) {
        if (type.empty()) return;
        _dataType = type;
    }

    void WriteHeader(const char *headerName, const char *rawName = nullptr);

    void WriteRaw(const char *name);
};


#endif //DIP_MHD_WRITER_H