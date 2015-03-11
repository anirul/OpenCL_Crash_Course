cmake_minimum_required(VERSION 2.8)

project(OpenCL_Crash_Course)

include("cmake/FindCXXFeatures.cmake")
set(CMAKE_CXX_FLAGS " -g -O0 ${CMAKE_CXX_FLAGS} ${CXX11_COMPILER_FLAGS}")

if (APPLE)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD c++11)
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY libc++)
    set(CMAKE_OSX_ARCHITECTURES x86_64)
    set(CMAKE_LIBRARY_PREFIX /opt/local /usr/local)

    include_directories(
        /opt/local/include
        ${PROJECT_SOURCE_DIR}/Thirdparty/CL12/
    )

    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(OPENAL_LIBRARY OpenAL)
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(OPENCL_LIBRARY OpenCL)
endif(APPLE)

# read linux
if (UNIX AND NOT APPLE)

	include_directories(
		${PROJECT_SOURCE_DIR}/Thirdparty/CL11
	)

	find_library(OPENCL_LIBRARY OpenCL)
	find_library(OPENGL_LIBRARY GL)
	find_library(GLU_LIBRARY GLU)
endif (UNIX AND NOT APPLE)

find_package(Boost 1.54.0 REQUIRED COMPONENTS
    program_options
    filesystem
    system
    date_time
    chrono
)

if (NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
endif()

#OpenCV
find_library(
    OPENCV_CORE_LIB
    opencv_core
)

find_library(
    OPENCV_GPU_LIB
    opencv_gpu
)

find_library(
    OPENCV_HIGHGUI_LIB
    opencv_highgui
)

find_library(
    OPENCV_IMGPROC_LIB
    opencv_imgproc
)

find_library(
    GLUT_LIB
    glut
)

add_executable(
    Simple
    ${PROJECT_SOURCE_DIR}/Simple/simple.cpp
    ${PROJECT_SOURCE_DIR}/Simple/simple.cl
)

target_link_libraries(
    Simple
    ${OPENCL_LIBRARY}
)

add_executable(
    FloydWarshall
    ${PROJECT_SOURCE_DIR}/FloydWarshall/cl_floyd_warshall.cpp
    ${PROJECT_SOURCE_DIR}/FloydWarshall/cl_floyd_warshall.h
    ${PROJECT_SOURCE_DIR}/FloydWarshall/cl_util.cpp
    ${PROJECT_SOURCE_DIR}/FloydWarshall/cl_util.h
    ${PROJECT_SOURCE_DIR}/FloydWarshall/ewd_file.cpp
    ${PROJECT_SOURCE_DIR}/FloydWarshall/ewd_file.h
    ${PROJECT_SOURCE_DIR}/FloydWarshall/floyd_warshall.cl
    ${PROJECT_SOURCE_DIR}/FloydWarshall/main.cpp
)

target_link_libraries(
    FloydWarshall
    ${OPENCL_LIBRARY}
    ${Boost_LIBRARIES}
)

add_executable(
    Histogram
    ${PROJECT_SOURCE_DIR}/Histogram/cl_histogram.cpp
    ${PROJECT_SOURCE_DIR}/Histogram/cl_histogram.hpp
    ${PROJECT_SOURCE_DIR}/Histogram/main.cpp
    ${PROJECT_SOURCE_DIR}/Histogram/histogram.cl
)

target_link_libraries(
    Histogram
    ${OPENCV_CORE_LIB}
    ${OPENCV_GPU_LIB}
    ${OPENCV_HIGHGUI_LIB}
    ${OPENCV_IMGPROC_LIB}
    ${Boost_LIBRARIES}
    ${OPENCL_LIBRARY}
)

add_executable(
    Video
    ${PROJECT_SOURCE_DIR}/Video/cl_video.cpp
    ${PROJECT_SOURCE_DIR}/Video/cl_video.hpp
    ${PROJECT_SOURCE_DIR}/Video/glut_win.cpp
    ${PROJECT_SOURCE_DIR}/Video/glut_win.hpp
    ${PROJECT_SOURCE_DIR}/Video/main.cpp
    ${PROJECT_SOURCE_DIR}/Video/painter.cl
    ${PROJECT_SOURCE_DIR}/Video/video.cl
    ${PROJECT_SOURCE_DIR}/Video/win_video.cpp
    ${PROJECT_SOURCE_DIR}/Video/win_video.hpp
)

target_link_libraries(
    Video
    ${OPENCV_CORE_LIB}
    ${OPENCV_GPU_LIB}
    ${OPENCV_HIGHGUI_LIB}
    ${OPENCV_IMGPROC_LIB}
    ${Boost_LIBRARIES}
    ${OPENGL_LIBRARY}
    ${GLU_LIBRARY}
    ${GLUT_LIB}
    ${OPENCL_LIBRARY}
)
