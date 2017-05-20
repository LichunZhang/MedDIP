cmake_minimum_required(VERSION 2.8)
project(DIP)

set(CMAKE_CXX_STANDARD 11)
add_subdirectory(MHDIO)
add_subdirectory(PT)
add_subdirectory(TT)
add_subdirectory(GT)
add_subdirectory(FFT)