// Program: DIP
// FileName:point_trans.cpp
// Author:  Lichun Zhang
// Date:    2017/5/20 下午4:44
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#include <mhd_reader.h>
#include "point_trans.h"

int main(int argc, char *argv[]) {
    if (argc <3){
        std::cout<<"Parameters error!\n";
        return -1;
    }

    MHDReader *reader = new MHDReader(argv[1]);
    if (!reader->GetImData()) {
        std::cout << "Read input failed!\n";
        delete reader;
        return -1;
    }

    bool flag =
            ::HisEqualize(reader->GetImData(), reader->GetImWidth(),
                          reader->GetImHeight(), reader->GetImSlice());
    if (flag)
        reader->Write(argv[2]);
    delete reader;
    return 0;
}