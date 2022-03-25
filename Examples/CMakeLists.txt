cmake_minimum_required(VERSION 3.14)

project(OpenCLCrashCourse)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    include("~/Documents/GitHub/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()
if(UNIX AND NOT DEFINED VCPKG_TARGET_TRIPLET)
    set(VCPKG_TARGET_TRIPLET x64-linux)
endif()
if(WIN32 AND NOT DEFINED VCPKG_TARGET_TRIPLET)
    set(VCPKG_TARGET_TRIPLET x64-windows)
endif()
if(WIN32)
    add_compile_options("/std:c++latest")
    # Needed for gprc.
    add_compile_options("/D WINVER=0x0A00")
    add_compile_options("/D _WIN32_WINNT=0x0A00")
endif(WIN32)

find_package(Boost REQUIRED COMPONENTS system program_options date_time chrono)
find_package(OpenCV REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenCL REQUIRED)

add_executable(Simple
	Simple/simple.cpp
	Simple/simple.cl
)

target_link_libraries(Simple
  PUBLIC
	OpenCL::OpenCL	
)

add_executable(FloydWarshall
	FloydWarshall/cl_floyd_warshall.cpp
	FloydWarshall/cl_floyd_warshall.h
	FloydWarshall/cl_util.cpp
	FloydWarshall/cl_util.h
	FloydWarshall/ewd_file.cpp
	FloydWarshall/ewd_file.h
	FloydWarshall/floyd_warshall.cl
	FloydWarshall/main.cpp
)

target_link_libraries(FloydWarshall
  PUBLIC
	OpenCL::OpenCL
	Boost::system
	Boost::program_options
	Boost::date_time
	Boost::chrono
)

add_executable(Histogram
	Histogram/cl_histogram.cpp
	Histogram/cl_histogram.hpp
	Histogram/main.cpp
	Histogram/histogram.cl
)

target_link_libraries(Histogram
  PUBLIC
  	${OpenCV_LIBS}
	OpenCL::OpenCL
	Boost::system
	Boost::program_options
	Boost::date_time
	Boost::chrono
)

add_executable(Video
	Video/cl_video.cpp
	Video/cl_video.hpp
	Video/glut_win.cpp
	Video/glut_win.hpp
	Video/main.cpp
	Video/painter.cl
	Video/video.cl
	Video/win_video.cpp
	Video/win_video.hpp
)

target_link_libraries(Video
  PUBLIC
	GLUT::GLUT
	${OpenCV_LIBS}
	OpenCL::OpenCL
	Boost::system
	Boost::program_options
	Boost::date_time
	Boost::chrono
)
