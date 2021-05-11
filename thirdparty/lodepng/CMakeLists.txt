cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_library(lodepng STATIC
	lodepng.cpp
	lodepng.h
)
