cmake_minimum_required(VERSION 3.14)

project(OpenCL_Crash_Course)

if(WIN32)
    add_compile_options("/std:c++latest")
    # Needed for gprc.
    add_compile_options("/D WINVER=0x0A00")
    add_compile_options("/D _WIN32_WINNT=0x0A00")
endif(WIN32)

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost REQUIRED COMPONENTS system program_options)
find_package(opencv REQUIRED)
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
	opencv::core
	opencv::gpu
	opencv::highgui
	opencv::imgproc
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
	opencv::core
	opencv::gpu
	opencv::highgui
	opencv::imgproc
	OpenCL::OpenCL
	Boost::system
	Boost::program_options
	Boost::date_time
	Boost::chrono
)

if(WIN32)
    # Standard MSVC flags.
    add_compile_options("/std:c++latest")
    add_compile_options("/D _WIN32_WINNT=0x0A00")
    # This is there to link static in case there is a -static in the variable.
    if(${VCPKG_TARGET_TRIPLET} MATCHES "-static")
        string(REPLACE 
            "/MD"
            "/MT" 
            CMAKE_CXX_FLAGS 
            ${CMAKE_CXX_FLAGS})
        string(REPLACE 
            "/MD"
            "/MT" 
            CMAKE_CXX_FLAGS_DEBUG 
            ${CMAKE_CXX_FLAGS_DEBUG})
        string(REPLACE 
            "/MD"
            "/MT" 
            CMAKE_CXX_FLAGS_RELEASE 
            ${CMAKE_CXX_FLAGS_RELEASE})
        string(REPLACE 
            "/MD"
            "/MT" 
            CMAKE_CXX_FLAGS_MINSIZEREL 
            ${CMAKE_CXX_FLAGS_MINSIZEREL})
        string(REPLACE 
            "/MD"
            "/MT" 
            CMAKE_CXX_FLAGS_RELWITHDEBINFO
            ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
    endif(${VCPKG_TARGET_TRIPLET} MATCHES "-static")
endif(WIN32)