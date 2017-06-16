// Program: DIP
// FileName:MHDReader.cpp
// Author:  Lichun Zhang
// Date:    2017/4/5 下午8:28
// Copyright (c) 2017 Lichun Zhang. All rights reserved.

#include "mhd_reader.h"
#include "utiles.h"

#include <fstream>
#include <iostream>
#include <sstream>

MHDReader::MHDReader(const char *name)
		: MHD_IO(name),_raw_name("") {
	if (name)
		ReadFile(name);
}

MHDReader::~MHDReader() {
}

void MHDReader::ReadFile(const char *name) {
	ReadHeader(name);
	if (_fileName.empty() || _raw_name.empty()|| _dataType.empty()) return;
	ReadRaw(_raw_name.c_str());
}

// Get the mhd DimSize, Type, DataFile(raw)
void MHDReader::ReadHeader(const char *name) {
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
	if (name == nullptr) return;
	_fileName = name;
	std::ifstream in(_fileName);
	if (!in) {
		std::cout << "Error! Can't Open File " << name << std::endl;
		in.close();
		return;
	}
	std::string line, raw_filename, temp1, temp2;
	while (std::getline(in, line)) {
		// Read Header Spacing
		if (line.find("ElementSpacing") != std::string::npos) {
			std::istringstream record(line);
			record >> temp1 >> temp2 >> _spacingX >> _spacingY >> _spacingZ;
			continue;
		}
			// Read Header DimSize
		else if (line.find("DimSize") != std::string::npos) {
			std::istringstream record(line);
			record >> temp1 >> temp2 >> this->_dimX >> this->_dimY >> this->_dimZ;
			continue;
		}
			// Read Header ElementSize
		else if (line.find("ElementType") != std::string::npos) {
			std::istringstream record(line);
			record >> temp1 >> temp2 >> _dataType;
			continue;
		}
			// Read Header DataFile
		else if (line.find("ElementDataFile") != std::string::npos) {
			std::istringstream record(line);
			record >> temp1 >> temp2 >> raw_filename;
            _raw_name=_fileName;
			size_t pos = _raw_name.find_last_of("\\");
			if (pos == std::string::npos) pos = _raw_name.find_last_of("/");
			_raw_name.erase(pos + 1);
			_raw_name += raw_filename;
		}
	}
	in.close();
}


void MHDReader::ReadRaw(const char *name) {
	FILE *fp = fopen(name, "rb");
	if (!fp) {
		std::cout << "Error! Can't Open File " << name << std::endl;
		fclose(fp);
		return;
	}
	ConstructData(_dataType, fp);
	fclose(fp);
}

void MHDReader::ConstructData(std::string type, FILE *fp) {
	if (!_dimX || !_dimY || !_dimZ
		|| type.empty() || !fp)
		return;
	if (type == "MET_UCHAR") {
		if (_imData) delete[] _imData;
		_imData = new unsigned char[_dimX * _dimY * _dimZ];
		fread(_imData, sizeof(unsigned char), _dimX * _dimY * _dimZ, fp);
	}
}

// name without suffix
void MHDReader::SaveAs(const char *name) {
	size_t dims[] = {_dimX,_dimY,_dimZ};
	double spacing[] = {_spacingX,_spacingY,_spacingZ};
	WriteMHD(name, _imData, dims, spacing);
}
