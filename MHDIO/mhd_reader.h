// Program: DIP
// FileName:MHDReader.h
// Author:  Lichun Zhang
// Date:    2017/4/5 下午8:28
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#ifndef DIP_MHD_READER_H
#define DIP_MHD_READER_H


#include <string>
#include "mhd_io.h"

class MHDReader :public MHD_IO{
public:
    MHDReader(const char *name = nullptr);

    virtual ~MHDReader();

    void ReadFile(const char* name);
    void SaveAs(const char *name);

private:
    std::string _raw_name;
    void ReadHeader(const char *name);
    void ReadRaw(const char* name);
    void ConstructData(std::string type, FILE *fp);
};


#endif //DIP_MHD_READER_H