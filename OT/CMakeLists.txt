PROJECT(OrthogonalTrans)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
set(CMAKE_MACOSX_RPATH 0)

SET(CMAKE_CXX_STANDARD 11)

SET(SOURCE_FILES fft.cpp dct.cpp main.cpp ortho_trans.h)
INCLUDE_DIRECTORIES(../MHDIO)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR})
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})
ADD_EXECUTABLE(OrthogonalTransTest ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} MHDIO)
TARGET_LINK_LIBRARIES(OrthogonalTransTest MHDIO)

