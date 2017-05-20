PROJECT(MHDIO)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(CMAKE_CXX_STANDARD 11)

SET(SOURCE_FILES
        mhd_io.h
        mhd_io.cpp
        mhd_reader.h
        mhd_reader.cpp
        mhd_writer.h
        mhd_writer.cpp
        utiles.h
        utiles.cpp
        )

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
ADD_LIBRARY(MHDIO SHARED ${SOURCE_FILES})