cmake_minimum_required(VERSION 3.16)
project(cv_laba_2)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_laba_2 
	main.cpp
	src/filters.cpp)
	
target_link_libraries(cv_laba_2 ${OpenCV_LIBS})
