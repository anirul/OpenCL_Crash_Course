cmake_minimum_required(VERSION 3.14)

project(OpenCL_Crash_Course)

if(WIN32)
    add_compile_options("/std:c++latest")
    # Needed for gprc.
    add_compile_options("/D WINVER=0x0A00")
    add_compile_options("/D _WIN32_WINNT=0x0A00")
endif(WIN32)

add_executable(Histogram
	cl_histogram.cpp
	cl_histogram.hpp
	main.cpp
	histogram.cl
)
