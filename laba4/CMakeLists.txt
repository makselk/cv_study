cmake_minimum_required(VERSION 3.16)
project(cv_laba_4)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_laba_4
	main.cpp
	src/pipeline.cpp
)
	
target_link_libraries(cv_laba_4 ${OpenCV_LIBS})
